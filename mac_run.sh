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
sleep 2
./client_random_player.exe localhost $port1 1 &
sleep 2
./client_random_player.exe localhost $port2 2 >/dev/null 2>&1;
