#!/bin/bash

exec 3<>/dev/tcp/localhost/6667

echo -ne "NICK validnick\r\n" >&3
sleep 1

echo -ne "PASS test\r\n" >&3
sleep 1

echo -ne "NICK   			\r\n" >&3
sleep 1

echo -ne "NICK 123invalidnick\r\n" >&3
sleep 1

echo -ne "NICK validnick\r\n" >&3
sleep 1

echo -ne "USER validuser\r\n" >&3
sleep 1


exec 4<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&4
sleep 1
echo -ne "NICK validnick\r\n" >&4
sleep 1
echo -ne "USER validuser\r\n" >&4
sleep 1

echo -ne "NICK invalid!nick\r\n" >&3
sleep 1

echo -ne "QUIT \r\n" >&3
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream

echo -ne "QUIT \r\n" >&4
exec 4<&-  # Close input stream
exec 4>&-  # Close output stream