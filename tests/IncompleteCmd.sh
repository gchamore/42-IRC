#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS \r\n" >&3
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK \r\n" >&3
echo -ne "NICK anto\r\n" >&3
sleep 1

echo -ne "USER  \r\n" >&3
echo -ne "USER  anferre\r\n" >&3
sleep 1

echo -ne "JOIN \r\n" >&3
echo -ne "JOIN #test\r\n" >&3
sleep 1

echo -ne "PRIVMSG \r\n" >&3
echo -ne "PRIVMSG #test \r\n" >&3
echo -ne "PRIVMSG #test :hello\r\n" >&3
sleep 1

echo -ne "MODE  \r\n" >&3
echo -ne "MODE #test \r\n" >&3
echo -ne "MODE #test +i\r\n" >&3

echo -ne "NOTEXIST  \r\n" >&3
sleep 4
echo -ne "QUIT \r\n" >&3
sleep 1

exec 3<&-  # Close input stream
exec 3>&-  # Close output stream