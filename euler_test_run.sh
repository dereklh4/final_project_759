#!/bin/sh
#SBATCH --partition=slurm_shortgpu
#SBATCH --time=0-00:20:00 # run time in days-hh:mm:ss
#SBATCH --nodes=1
#SBATCH --cpus-per-task=20
#SBATCH --error=/srv/home/dlhancock/final_project_759/sbatch.err
#SBATCH --ntasks=1
#SBATCH --output=/srv/home/dlhancock/final_project_759/sbatch.out
## #SBATCH --gres=gpu:1

module load python/2.7.13
module load java
python test.py 14
