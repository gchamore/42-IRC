sudo tc qdisc add dev lo root netem delay 300ms #add latency
nc -q 1 localhost 6667
NICK laguser
USER laguser 0 * :Lag Test
JOIN #lagchannel
PRIVMSG #lagchannel :Testing under high latency
sudo tc qdisc del dev lo root netem #remove latency
