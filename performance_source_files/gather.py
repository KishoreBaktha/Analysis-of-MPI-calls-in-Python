from mpi4py import MPI
import random as r
import math as m
comm = MPI.COMM_WORLD
world_rank = comm.Get_rank()
size = comm.Get_size()
count=0.0
value = [1]*(size-1)
localpi=0.0
x=0.0
y=0.0
z=0.0
i=0
flip=500000
for i in range(0,flip) :
     x = r.random()**2
     y = r.random()**2
    # Increment if inside unit circle.
     z =  m.sqrt(x + y)
     if z<=1.0 :
       count+=1
value= comm.gather(count,root=0)
if world_rank ==0 :
  for j in range(0,size) :
       localpi+=value[j]
  answer=(localpi/(flip*size))*4
  print("answer is %lf " % answer)
