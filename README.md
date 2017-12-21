# 759 Final Project - Derek Hancock

This project has the server/client code necessary to play two minimax agents against each other. There are 3 agents available:
1. client_random_player - makes moves randomly throughout the game
2. client_baseline_minimax - uses minimax to make moves but doesn't use parallelization
3. client_openmp_minimax - Uses OpenMP and minimax to determine its moves  

note that client_minimax_player contains the code for baseline minimax as well as openmp minimax

## Running the Code
In order to run the code, you need to start up 3 processes -- one for the server, and one for each of the clients. To simplify things, I have a bash script that will start the processes up for you. The default bash script starts a 1 minute game server on 2 free ports, starts a client_baseline_minimax player 1 (black) with a max depth of 6, and starts a client_openmp_minimax player 2 (white) with a max depth of 6.

On euler, there are two main ways to run the code. Both scripts automaticaly load the necessary modules, compile the code, and then runs it. Option 1 is probably the easiest:
1. Use an interactive session.  
  - Run "srun -p slurm_shortgpu -c 20 --pty -u bash -i" to start an interactive session
  - Run "bash local_run.sh"
  
2. Use sbatch
  - Modify euler_run.sh to send the output and error to the desired location
  - run "sbatch euler_run.sh" to submit the job
  - Observe the results in the output file
    
You can observe the output as the code is running and once it finishes. It shows the current state of the board, the time left for each player, the number of nodes expanded per millisecond for the openmp agent, how many pieces each player has on the board, and other information. 

Note that black is player 1 and white is player 2. At each spot on the board it shows a 1 for player 1, 2 for player 2, or 0 if it is empty. 

If you want to see the output information for the other player as well, you can go into the bash or sbatch script and comment out the ">/dev/null 2>&1" on the player 1 line. Note that both players will be outputting results if you do this, so the outputs may overlap some.
