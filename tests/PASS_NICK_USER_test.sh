#!/bin/bash
exec 3<>/dev/tcp/localhost/6667
echo "PASS test" >&3
sleep 1
echo "NICK anto" >&3
sleep 1
echo "USER" >&3
sleep 1
exec 3<&-  # Close input stream
exec 3>&-  # Close output stream