# project

For running python codes on beskow(supercomputer)- <br />
1)Login to beskow <br />
 2)Allocate required nodes <br />
 Type following commands- <br />
 3)module load anaconda/py27/5.3 <br />
 4)source activate mpi4py <br />
 5)export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$CONDA_PREFIX/lib <br />
 To run code- <br />
 aprun -n <number_of_processors>  python <name_of_python_file>.py <arguments> <br />

For running map- <br />
1)module load allinea-forge/18.1.1 <br />
2)export ALLINEA_SAMPLER_INTERVAL=1 <br />
3)map --profile aprun -n <number_of_processors>  python <name_of_python_file>.py <arguments> <br />

Performance_source folder consists of all the code files for evaluation of basic MPI calls- <br />
For running - broadcast.py,one-sidedcomm.py,sr_comparision.py,sr_normal.py,sr_numpy.py,user_def_type.py <br /> 
aprun -n <number_of_processors> python <name_of_python_file>.py <size_argument> <br />
For other programs- <br />
aprun -n <number_of_processors> python <name_of_python_file>.py <br />

Fox folder consists of source files for fox algorithm- <br />
For generating matrice elements- <br />
1)cc generate.c <br />
2)aprun ./a.out 576 576 576 A.bin B.bin C.answer 944 <br />
For running - <br />
aprun -n <number_of_processors> python <name_of_the_file>.py A.bin B.bin C2.answer <br />
For verifying- <br />
1)cc verify.c <br />
2)aprun ./a.out C2.answer C.answer <br />

Map folder consists of all the map files. They can be viewed using the ARM map tool. <br />

Results Folder consists of all the table data .

