#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK operator\r\n" >&3
sleep 1

echo -ne "USER  operator\r\n" >&3
sleep 1

echo -ne "JOIN #test \r\n" >&3
sleep 1
echo -ne "MODE\r\n" >&3
sleep 1
echo -ne "MODE #nonexistentchannel\r\n" >&3
sleep 1
echo -ne "MODE #test \r\n" >&3
sleep 1
echo -ne "MODE #test +k \r\n" >&3
sleep 1
echo -ne "MODE #test +k secret \r\n" >&3
sleep 1
echo -ne "MODE #test +i \r\n" >&3
sleep 1
echo -ne "MODE #test +t\r\n" >&3
sleep 1

exec 4<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&4
echo -ne "NICK regular\r\n" >&4
echo -ne "USER regular\r\n" >&4

echo -ne "JOIN #test \r\n" >&4
sleep 1

echo -ne "INVITE regular #test \r\n" >&3
sleep 1

echo -ne "JOIN #test \r\n" >&4
sleep 1

echo -ne "JOIN #test secret \r\n" >&4
sleep 1

echo -ne "TOPIC #test new topic \r\n" >&4
sleep 1

echo -ne "MODE #test +o regular \r\n" >&3

echo -ne "TOPIC #test new topic \r\n" >&4
sleep 1

echo -ne "MODE #test\r\n" >&4
sleep 1

echo -ne "KICK #test regular \r\n" >&3
sleep 1

echo -ne "KICK #test operator \r\n" >&3

echo -ne "PART #test \r\n" >&3
sleep 1

echo -ne "QUIT ciao\r\n" >&3

echo -ne "QUIT goodbye\r\n" >&4

exec 3<&-
exec 3>&-
exec 4<&-
exec 4>&-