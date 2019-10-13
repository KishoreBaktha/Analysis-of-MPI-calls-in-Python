from mpi4py import MPI
from Config_numpy import Config
import math
import sys
from array import array
import struct
from decimal import Decimal
import numpy
if len(sys.argv)<2:
    print("specify: matrixA matrixB matrixC")
    sys.exit(0)
comm = MPI.COMM_WORLD
myrank = comm.Get_rank()
nproc = comm.Get_size()



config=Config()
amode = MPI.MODE_WRONLY|MPI.MODE_CREATE
config.A_file=MPI.File.Open(comm,sys.argv[1])
config.B_file=MPI.File.Open(comm,sys.argv[2])
config.C_file=MPI.File.Open(comm,sys.argv[3],amode)
config.world_rank=myrank
config.WORLD_SIZE=nproc


if config.world_rank == 0:
    config.A_file.Read(config.A_dims)
    config.B_file.Read(config.B_dims)	
config.A_dims=comm.bcast(config.A_dims, root=0)
config.B_dims=comm.bcast(config.B_dims, root=0)

config.dim = [math.sqrt(config.WORLD_SIZE),math.sqrt(config.WORLD_SIZE)]
config.C_dims[0]=int(config.A_dims[0])
config.C_dims[1]=int(config.B_dims[1])
wrap_around=[0,1]

config.grid_comm=comm.Create_cart(config.dim,periods=wrap_around)
config.grid_rank=config.grid_comm.Get_rank()
free_coords=[0,1]
config.row_comm=config.grid_comm.Sub(free_coords)
config.row_rank=config.row_comm.Get_rank()
free_coords=[1,0]
config.col_comm=config.grid_comm.Sub(free_coords)
config.col_rank=config.col_comm.Get_rank()

config.matrix_size= config.A_dims[0]*config.A_dims[0]
config.local_dims=[int(config.A_dims[0]/config.dim[0]),int(config.A_dims[0]/config.dim[0])]

config.local_size=int(config.local_dims[0]*config.local_dims[1])

config.coords=config.grid_comm.Get_coords(config.grid_rank)

config.my_row=config.coords[0]
config.my_col=config.coords[1]

starts = [config.my_row*int(config.local_dims[0]),config.my_col*int(config.local_dims[0])]

config.block=MPI.DOUBLE.Create_subarray(config.A_dims,config.local_dims, starts, order=MPI.ORDER_C)
config.block.Commit()
config.A_file.Set_view(disp= MPI.INT.Get_size()*2,filetype=config.block)
config.B_file.Set_view(disp= MPI.INT.Get_size()*2,filetype=config.block)
config.A = numpy.empty(shape=(int(config.local_dims[0]),int(config.local_dims[0])))
config.A.fill(0.0)
config.B = numpy.empty(shape=(int(config.local_dims[0]),int(config.local_dims[0])))
config.B.fill(0.0)
config.C = numpy.empty(shape=(int(config.local_dims[0]),int(config.local_dims[0])))
config.C.fill(0.0)

config.A_file.Read_all(config.A)
config.B_file.Read_all(config.B)


config.A_file.Close()
config.B_file.Close()
config.compute_fox()
comm.Barrier()
if config.world_rank==0 :
      config.C_file.Write(config.C_dims)
config.C_file.Set_view(disp= MPI.INT.Get_size()*2,filetype=config.block)
config.C_file.Write_all(config.C)      
config.C_file.Close()

