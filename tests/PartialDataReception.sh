#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS password123\r\n" >&3
sleep 1
echo -ne "NICK anto\r\n" >&3
sleep 1

echo -ne "NICK " >&3
sleep 1
echo -ne "testnick\r\n" >&3	

sleep 1
echo -ne "USER testuser\r\n" >&3
sleep 1

sleep 4
echo -ne "QUIT\r\n" >&3

exec 3<&-  # Close input stream
exec 3>&-  # Close output stream
