from mpi4py import MPI
import random as r
import math as m
from array import array
comm = MPI.COMM_WORLD
world_rank = comm.Get_rank()
size = comm.Get_size()
count=array('d',[0.0]*1)
value = [1]*(size-1)
localpi=array('d',[0.0]*1)
x=0.0
y=0.0
z=0.0
i=0
flip=5000
pending=[]
for i in range(0,flip) :
     x = r.random()**2
     y = r.random()**2
    # Increment if inside unit circle.
     z =  m.sqrt(x + y)
     if z<=1.0 :
       count[0]+=1
comm.Ireduce(count,localpi,op=MPI.SUM,root =0)
comm.Barrier()
if world_rank ==0 :
   answer=(localpi[0]/(flip*size))*4
   print("answer is %lf " % (answer))
