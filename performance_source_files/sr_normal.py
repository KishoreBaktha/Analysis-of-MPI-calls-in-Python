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
partial_sum= [0] * size
for i in range(0,size) :
  partial_sum[i]= i
if ( myrank != 0 ):
   comm.send(partial_sum,dest=0,tag =7)
else :
   for i in range (1 , nproc ):
     tmp_sum=comm.recv(source=i,tag=7)
   print("received data")
