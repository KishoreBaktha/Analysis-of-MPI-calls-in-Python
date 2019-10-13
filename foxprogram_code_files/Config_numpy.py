import numpy
from mpi4py import MPI
from array import array
class Config:
    A_file = None
    B_file = None
    C_file = None
    grid_comm = None
    row_comm = None
    col_comm = None
    dim = []
    coords = []
    A_dims = array('i',[0]*2)
    B_dims = array('i',[0]*2)
    C_dims = array('i',[0]*2)
    matrix_size = 0
    local_dims = []
    local_size = 0
    my_row = 0
    my_col = 0
    world_rank=None
    WORLD_SIZE=None
    grid_rank=None
    A=None
    A_tmp=None
    B=None
    C=None




    def compute_fox(self):
        self.A_tmp = numpy.empty(shape=(int(self.local_dims[0]),int(self.local_dims[0])))
        self.A_tmp.fill(0.0)
        source = (self.my_row + 1) % int(self.dim[0])
        dest = (self.my_row + int(self.dim[0]) - 1) % int(self.dim[0])
        for i in range(0,int(self.dim[0])):
            root = int((self.my_row + i) % int(self.dim[0]))
            if int(root) == int(self.my_col):
                self.row_comm.Bcast(self.A, root)
                self.multiply_matrix(self.A)

            else:
                self.row_comm.Bcast(self.A_tmp, root)
                self.multiply_matrix(self.A_tmp)

            self.col_comm.Sendrecv_replace(self.B,dest,0,source,0,status=None)


    def multiply_matrix(self, Amatrix):
          self.C+=numpy.matmul(Amatrix,self.B)
