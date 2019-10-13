from mpi4py import MPI
from numpy import \
arange as np_arange, \
zeros as np_zeros
comm = MPI.COMM_WORLD
myrank = comm.Get_rank()
nproc = comm.Get_size()
if myrank == 0:
  fulldata = np_arange(3*nproc, dtype='i')
  print("I'm {0} fulldata is: {1}".format(myrank,fulldata))
else:
  fulldata = None
count = 3
mydata = np_zeros(count, dtype='i')
comm.Scatter([fulldata, count, MPI.INT],[mydata, count, MPI.INT],root=0)
print("After Scatter, I'm {0} and mydata is: {1}".format(myrank,mydata))
