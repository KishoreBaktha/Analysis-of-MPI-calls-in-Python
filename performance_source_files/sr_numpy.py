from mpi4py import MPI
import sys
from numpy import \
    arange as np_arange, \
    zeros as np_zeros, \
    uint32 as np_uint32
comm = MPI . COMM_WORLD
myrank = comm . Get_rank ()
nproc = comm . Get_size ()
size=int(sys.argv[1])
partial_sum = np_arange(size)
if ( myrank != 0 ):
   comm . Send ([ partial_sum , size , MPI . INT], dest =0 , tag =7 )
else :
   tmp_sum = np_zeros(size , dtype = np_uint32)
   for i in range (1 , nproc):
      comm . Recv ([ tmp_sum ,size , MPI . INT], source =i , tag =7)
   print("received data");



