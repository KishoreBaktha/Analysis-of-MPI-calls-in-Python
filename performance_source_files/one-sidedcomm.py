from mpi4py import MPI 
from array import array 
import sys
comm = MPI.COMM_WORLD 
size=int(sys.argv[1])
if comm.rank == 0: 
    memory = array('i',range(size)) 
    disp_unit = memory.itemsize 
else: 
    # other process do not expose memory 
    memory = None 
    disp_unit = 1 

win = MPI.Win.Create(memory, disp_unit, comm=comm) 

# all processes get  integers from process zero 
buf = array('i',[0]*size) 
win.Fence() 
win.Get(buf, 0) 
win.Fence() 
#print("[%d] %r" % (comm.rank, buf)) 
