sudo tc qdisc add dev lo root netem loss 20% #add packet loss

nc -q 1 localhost 6667
NICK lossuser
USER lossuser 0 * :Packet Loss Test
JOIN #test
PRIVMSG #test :Hello, anyone here?
