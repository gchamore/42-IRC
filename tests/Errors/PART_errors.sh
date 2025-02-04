#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK operator\r\n" >&3
sleep 1
echo -ne "USER  operator\r\n" >&3
sleep 1

exec 4<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&4
sleep 1
echo -ne "NICK another\r\n" >&4
sleep 1
echo -ne "USER  another\r\n" >&4
sleep 1

echo -ne "PART \r\n" >&3
sleep 1

echo -ne "PART #nonexistentchannel\r\n" >&3
sleep 1

echo -ne "JOIN #test\r\n" >&4
sleep 1

#not on the same channel
echo -ne "PART #test\r\n" >&3
sleep 1

echo -ne "PART #test\r\n" >&4
sleep 1


echo -ne "QUIT \r\n" >&3
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream

echo -ne "QUIT \r\n" >&4
exec 4<&-  # Close input stream
exec 4>&-  # Close output stream