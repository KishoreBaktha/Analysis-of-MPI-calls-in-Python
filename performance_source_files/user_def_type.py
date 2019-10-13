from __future__ import print_function
from  mpi4py import MPI
import numpy as np
import sys
comm = MPI.COMM_WORLD
size = comm.Get_size()
rank = comm.Get_rank()
length=int(sys.argv[1])
assert size > 1

def definetype(field_names, field_dtypes):
    num = 2
    dtypes = list(zip(field_names, field_dtypes))
    a = np.zeros(num, dtype=dtypes)

    struct_size = a.nbytes // num
    offsets = [ a.dtype.fields[field][1] for field in field_names ]

    mpitype_dict = {np.int32:MPI.INT, np.float64:MPI.DOUBLE}  
    field_mpitypes = [mpitype_dict[dtype] for dtype in field_dtypes]

    structtype = MPI.Datatype.Create_struct([1]*len(field_names), offsets, field_mpitypes)
    structtype = structtype.Create_resized(0, struct_size)
    structtype.Commit()
    return structtype

struct_field_names = ['int', 'dbl']
struct_field_types = [np.int32, np.float64]
mytype = definetype(struct_field_names, struct_field_types)
data = np.zeros(1, dtype=(list(zip(struct_field_names, struct_field_types))))

if rank == 0 :
  for j in range(1,size) :
    comm.Recv([data, mytype], source=j, tag=0)
    print(data)
else :
   data[0]['int'] = length
   data[0]['dbl'] = rank * 2.45*length
   comm.Send([data, mytype], dest=0, tag=0)
