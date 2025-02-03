#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS \r\n" >&3
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK \r\n" >&3
echo -ne "NICK anto\r\n" >&3
sleep 1

echo -n "USER  \r\n" >&3
echo -n "USER  anferre\r\n" >&3
sleep 1

echo -n "JOIN \r\n" >&3
echo -n "JOIN #test\r\n" >&3
sleep 1

echo -n "PRIVMSG \r\n" >&3
echo -n "PRIVMSG #test :hello\r\n" >&3
sleep 1

echo -n "MODE  \r\n" >&3
echo -n "MODE #test \r\n" >&3
echo -n "MODE #test +i\r\n" >&3
sleep 1

sleep 4

exec 3<&-  # Close input stream
exec 3>&-  # Close output stream