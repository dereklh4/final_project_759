#function random_unused_port {
#    local port=$(shuf -i 2000-65000 -n 1)
#    netstat -lat | grep $port > /dev/null
#    if [[ $? == 1 ]] ; then
#        export RANDOM_PORT=$port
#    else
#        random_unused_port
#    fi
#}

#random_unused_port
port1=3333
#random_unused_port
port2=3334
echo $port1
echo $port2

#module load java
java ReversiServer $port1 $port2 1 &
sleep 1
./main.exe client_baseline_minimax localhost $port1 1 7 >/dev/null 2>&1 &
sleep 1
./main.exe client_openmp_minimax localhost $port2 2 7;
