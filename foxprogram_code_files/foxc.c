#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

void print_usage();
struct Config {
	/* MPI Files */
	MPI_File A_file, B_file, C_file;
	char *outfile;

	/* MPI Datatypes for matrix blocks */
	MPI_Datatype block;
        double block_data[3];
        
	/* Matrix data */
	double *A, *A_tmp, *B,*C;

	/* Cart communicators */
	MPI_Comm grid_comm;
	MPI_Comm row_comm;
	MPI_Comm col_comm;

	/* Cart communicator dim and ranks */
	int dim[2], coords[2];
	int world_rank,WORLD_SIZE, grid_rank;
	int row_rank, row_size, col_rank, col_size;

	/* Full matrix dim */
	int A_dims[2];
	int B_dims[2];
	int C_dims[2];
	int matrix_size;

	/* Process local matrix dim */
	int local_dims[2];
	int local_size;

        int my_row;
        int my_col;
};
struct Config config;
void multiply_matrix();
void multiply_matrixtemp();
void init_matmul(char *A_file, char *B_file, char *outfile,int world_size,int world_rank)
{
	/* Copy output file name to configuration */
          
          config.outfile=(char *)malloc(sizeof(outfile));
          strcpy(config.outfile,outfile);
         

          config.WORLD_SIZE=world_size;
          config.world_rank=world_rank;
      
	/* Get matrix size header */
          MPI_File_open(MPI_COMM_WORLD,A_file,MPI_MODE_RDONLY, MPI_INFO_NULL,&config.A_file);
          MPI_File_open(MPI_COMM_WORLD,B_file,MPI_MODE_RDONLY, MPI_INFO_NULL,&config.B_file);
          MPI_File_open(MPI_COMM_WORLD,outfile,MPI_MODE_WRONLY|MPI_MODE_CREATE, MPI_INFO_NULL,&config.C_file);
          if(config.world_rank==0)
          {
          MPI_File_read(config.A_file,config.A_dims,2,MPI_INT,MPI_STATUS_IGNORE);
          MPI_File_read(config.B_file,config.B_dims,2,MPI_INT,MPI_STATUS_IGNORE);
          }
  

	/* Broadcast global matrix sizes */
           MPI_Bcast(config.A_dims,2,MPI_INT,0,MPI_COMM_WORLD);
           MPI_Bcast(config.B_dims,2,MPI_INT,0,MPI_COMM_WORLD);
      
      
	/* Set dim of tiles relative to the number of processes as NxN where N=sqrt(world_size) */
          config.dim[0]=sqrt(config.WORLD_SIZE);
          config.dim[1]=sqrt(config.WORLD_SIZE);
           
	/* Verify dim of A and B matches for matul and both are square*/
     //number of columns in first matrix should be equal to the number of rows in second matrix          
           if(config.A_dims[1]!=config.B_dims[0]||config.A_dims[0]!=config.A_dims[1]||config.B_dims[0]!=config.B_dims[1])
           exit(1);
           int wrap_around[2];
           config.C_dims[0]=config.A_dims[0];
           config.C_dims[1]=config.B_dims[1];
           wrap_around[0]=0;
           wrap_around[1]=1;

	/* Create Cart communicator for NxN processes */
          MPI_Cart_create(MPI_COMM_WORLD,2,config.dim,wrap_around,1,&config.grid_comm);
          MPI_Comm_rank(config.grid_comm,&(config.grid_rank));
   /* Sub div cart communicator to N row communicator */
          int free_coords[2];
          free_coords[0]=0;free_coords[1]=1;
          MPI_Cart_sub(config.grid_comm,free_coords,&config.row_comm);
          MPI_Comm_rank(config.row_comm,&(config.row_rank));   
	/* Sub div cart communicator to N col communicator */

         free_coords[0]=1;free_coords[1]=0;
         MPI_Cart_sub(config.grid_comm,free_coords,&config.col_comm);
         MPI_Comm_rank(config.col_comm,&(config.col_rank));
	/* Setup sizes of full matrices */
         config.matrix_size=  config.A_dims[0]*config.A_dims[0];
	/* Setup sizes of local matrix tiles */
         config.local_dims[0]=config.A_dims[0]/config.dim[0];
         config.local_dims[1]=config.A_dims[0]/config.dim[0];
         config.local_size=config.local_dims[0]*config.local_dims[1];
         
  
        MPI_Cart_coords(config.grid_comm,config.grid_rank,2,config.coords); 
        config.my_row=config.coords[0];
	config.my_col=config.coords[1];
        
	/* Create subarray datatype for local matrix tile */
        int starts[2]={config.my_row*(config.local_dims[0]),config.my_col*(config.local_dims[0])};
        MPI_Type_create_subarray(2,config.A_dims,config.local_dims,starts,MPI_ORDER_C, MPI_DOUBLE, &config.block); 
        MPI_Type_commit(&config.block);
	/* Create data array to load actual block matrix data */

        config.A=(double*)malloc(config.local_dims[0] *config.local_dims[0]*sizeof(double));
        config.B=(double*)malloc(config.local_dims[0] *config.local_dims[0]*sizeof(double));
        config.C=(double*)malloc(config.local_dims[0] *config.local_dims[0]*sizeof(double));
        int i; 
        for(i=0;i<config.local_size;i++)
        config.C[i]=0.0;
	/* Set fileview of process to respective matrix block */
        MPI_File_set_view(config.A_file,sizeof(int)*2,MPI_DOUBLE,config.block, "native",MPI_INFO_NULL);
        MPI_File_set_view(config.B_file,sizeof(int)*2,MPI_DOUBLE,config.block, "native",MPI_INFO_NULL);  //check for block reuse
	/* Collective read blocks from files */
        MPI_File_read_all(config.A_file,config.A,config.local_size,MPI_DOUBLE,MPI_STATUS_IGNORE);
        MPI_File_read_all(config.B_file,config.B,config.local_size,MPI_DOUBLE,MPI_STATUS_IGNORE);
	/* Close data source files */ 
        MPI_File_close(&config.A_file);
        MPI_File_close(&config.B_file);
}

void cleanup_matmul()
{
	/* Rank zero writes header specifying dim of result matrix C */
          if(config.world_rank==0)
          MPI_File_write(config.C_file,config.C_dims,2, MPI_INT, MPI_STATUS_IGNORE);
         
	/* Set fileview of process to respective matrix block with header offset */
          MPI_File_set_view(config.C_file,sizeof(int)*2,MPI_DOUBLE,config.block, "native",MPI_INFO_NULL);

	/* Collective write and close file */
          MPI_File_write_all(config.C_file,config.C,config.local_size, MPI_DOUBLE, MPI_STATUS_IGNORE);

	/* Cleanup */
          MPI_File_close(&config.C_file);
          free(config.A);
          free(config.B);
          free(config.C); 
}

void compute_fox()
{
        config.A_tmp=(double*)malloc(config.local_dims[0] *config.local_dims[0]*sizeof(double));
	/* Compute source and target for verticle shift of B blocks */
        int source = (config.my_row+1) % config.dim[0]; /* pick the emmediately lower element */
	int dest= (config.my_row+config.dim[0]-1) % config.dim[0];    /* move current element to immediately upper row */ 
        int i;int root = 0;
        for(i=0;i<config.local_size;i++)
        config.C[i]=0.0;
	for ( i = 0; i < config.dim[0]; i++) 
        {
		root=(config.my_row+i)%config.dim[0];
		if(root==config.my_col)
		{
			MPI_Bcast(config.A,config.local_dims[0]*config.local_dims[1],MPI_DOUBLE,root,config.row_comm);
		   	multiply_matrix(config.A);
                 }
		else
		{
			
                        MPI_Bcast(config.A_tmp,config.local_dims[0]*config.local_dims[1],MPI_DOUBLE,root,config.row_comm);
   	    	        multiply_matrix(config.A_tmp);
		}
		/* Diag + i broadcast block A horizontally and use A_tmp to preserve own local A */
                 
		/* Shfting block B upwards and receive from process below */
		MPI_Sendrecv_replace(config.B,config.local_dims[0]*config.local_dims[1],MPI_DOUBLE,dest,0,source,0,config.col_comm,MPI_STATUS_IGNORE);
	}
       free(config.A_tmp);
}


void multiply_matrix(double *Amatrix)
{

	int i, j, k;
    	for (i = 0; i <config.local_dims[0]; i++)
        for (k = 0; k <config.local_dims[0]; k++)
        for (j = 0; j <config.local_dims[0]; j++)
        config.C[i*config.local_dims[0]+j] += Amatrix[i*config.local_dims[0]+k]*config.B[k*config.local_dims[0]+j];

}
int main(int argc, char *argv[])
{
        int opt;
        int world_rank,world_size;
        int repeat = 1;
        void (*matmul)() = &compute_fox;

        double avg_runtime = 0.0, prev_avg_runtime = 0.0, stddev_runtime = 0.0;
        double start_time, end_time;

        MPI_Init(&argc, &argv);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Comm_size(MPI_COMM_WORLD, &world_size);
        while ((opt = getopt(argc, argv, "cfr:")) != -1) {
                switch (opt) {
                        case 'f':
                                if (world_rank == 0) fprintf(stderr, "Using Fox's algorithm\n");
                                matmul = &compute_fox;
                                break;
                        case 'r':
                                repeat = atoi(optarg);
                                break;
                        default:
                                if (world_rank == 0) print_usage(argv[0]);
                                MPI_Finalize();
                                exit(1);
                }
        }

        if (argv[optind] == NULL || argv[optind + 1] == NULL || argv[optind + 2] == NULL) {
                if (world_rank == 0) print_usage(argv[0]);
                MPI_Finalize();
                exit(1);
        }
        init_matmul(argv[optind], argv[optind + 1], argv[optind + 2],world_size,world_rank);
        MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
        MPI_Barrier(MPI_COMM_WORLD);
        matmul();
        MPI_Barrier(MPI_COMM_WORLD);
        cleanup_matmul();
        MPI_Finalize();
        return 0;
}

void print_usage(char *program)
{
        fprintf(stderr, "Usage: %s [Matrix A] [Matrix B] [Output Matrix C]\n", program);
}


