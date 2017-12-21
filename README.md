# 759 Final Project - Derek Hancock

This project has the server/client code necessary to play two minimax agents against each other. There are 3 agents available:
1. client_random_player - makes moves randomly throughout the game
2. client_baseline_minimax - uses minimax to make moves but doesn't use parallelization
3. client_openmp_minimax - Uses OpenMP and minimax to determine its moves
note that client_minimax_player contains the code for baseline minimax as well as openmp minimax

## Running the Code
In order to run the code, you need to start up 3 processes -- one for the server, and one for each of the clients. To simplify things, I have a bash script that will start the processes up for you. The default bash script starts a 1 minute game server on 2 free ports, starts a client_baseline_minimax player 1 with a max depth of 6, and starts a client_openmp_minimax player 2 with a max depth of 6.

On euler, there are two main ways to run the code:
1. Use an interactive session.  
  - Run "srun -p slurm_shortgpu -c 20 --pty -u bash -i" to start an interactive session
  - Run "module load java"
  - Run "make"
  - Run "bash local_run.sh"
  
2. Use sbatch
  - Modify euler_run.sh to send the output and error to the correct location
  - run "sbatch euler_run.sh" to submit the job
  - Observe the results in the output file
    
