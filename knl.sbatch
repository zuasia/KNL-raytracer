#!/bin/bash
#SBATCH -J ray_xeon           # job name
#SBATCH -o %j.report       # output and error file name (%j expands to jobID)
#SBATCH -n 1              # total number of mpi tasks requested
#SBATCH -p normal     # queue (partition) -- normal, development, etc.
#SBATCH -t 01:00:00        # run time (hh:mm:ss) - 1 hours
#SBATCH -A Fa16EE382N-20

#./ray -r 1 geometry_file/scenes/polymesh/dragon1.ray test.bmp
#./ray -r 1 geometry_file/scenes/polymesh/simple.ray test.bmp
./ray -r 1 geometry_file/scenes/polymesh/trimesh1.ray test.bmp
