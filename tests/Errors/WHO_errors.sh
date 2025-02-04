#!/bin/bash

exec 3<>/dev/tcp/localhost/6667
echo -ne "PASS test\r\n" >&3
sleep 1
echo -ne "NICK operator\r\n" >&3
sleep 1

echo -ne "USER  operator\r\n" >&3
sleep 1

echo -ne "WHO \r\n" >&3
sleep 1

echo -ne "WHO #nonexistentchannel\r\n" >&3
sleep 1

echo -ne "JOIN #test\r\n" >&3
sleep 1

echo -ne "WHO #test\r\n" >&3
sleep 1

echo -ne "WHO #test operator\r\n" >&3
sleep 1

echo -ne "QUIT \r\n" >&3
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream
