from mpi4py import MPI
import sys
comm = MPI.COMM_WORLD
rank = comm.Get_rank()
size=int(sys.argv[1])
data = [0] * size
if rank == 0:
   for i in range (0,size) :
       data[i]=i;
else:
    data = None
data = comm.bcast(data, root=0)
print(" the 1000th element in data is %d " %data[999])
