#!/bin/bash

exec 3<>/dev/tcp/localhost/6667

echo -ne "USER testuser\r\n" >&3
sleep 1
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "USER testuser\r\n" >&3
sleep 1
echo -ne "NICK  testnick\r\n" >&3
sleep 1
echo -ne "USER \r\n" >&3
sleep 1

echo -ne "USER @testuser\r\n" >&3
sleep 1

echo -ne "USER testuser\r\n" >&3
sleep 1

echo -ne "USER testuser\r\n" >&3
sleep 1

echo -ne "QUIT \r\n" >&3
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream