#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK anto\r\n" >&3
sleep 1

echo -n "NICK " >&3
sleep 1
echo -ne "testnick\r\n" >&3	

sleep 4

exec 3<&-  # Close input stream
exec 3>&-  # Close output stream
