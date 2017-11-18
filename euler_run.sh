#!/bin/sh
#SBATCH --partition=slurm_shortgpu
#SBATCH --time=0-00:03:00 # run time in days-hh:mm:ss
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --error=/srv/home/dlhancock/final_project_759/sbatch.err
#SBATCH --ntasks=1
#SBATCH --output=/srv/home/dlhancock/final_project_759/sbatch.out
## #SBATCH --gres=gpu:1

function random_unused_port {
    local port=$(shuf -i 2000-65000 -n 1)
    netstat -lat | grep $port > /dev/null
    if [[ $? == 1 ]] ; then
        export RANDOM_PORT=$port
    else
        random_unused_port
    fi
}

random_unused_port
port1=$RANDOM_PORT
random_unused_port
port2=$RANDOM_PORT
echo $port1
echo $port2

module load java
java ReversiServer $port1 $port2 1 &
sleep 2
./client_random_player.exe localhost $port1 1 &
sleep 2
./client_random_player.exe localhost $port2 2 >/dev/null 2>&1;
