from mpi4py import MPI
comm = MPI.COMM_WORLD
import sys
# encoding=utf8
rank = comm.Get_rank()
size = comm.Get_size()
length=int(sys.argv[1])
import numpy as np
if rank ==0:
  requests = [ MPI . REQUEST_NULL for i in range (0 , size )]
  d = np.zeros(length, dtype = np.uint32)
  # Request data from a set of processes
  for i in range (1 , size ):
     requests [ i] = comm . Irecv ([ d ,length , MPI . INT], source =i , tag =7)
  status = [ MPI . Status () for i in range (0 , size )]
  # Wait for all the messages
  MPI . Request . Waitall ( requests , status )
  print (" Received data" )
else :
  data = np.arange(length)
  request = comm . Isend ([ data , length , MPI . INT ], 0, tag=7 )
  request . Wait ()
