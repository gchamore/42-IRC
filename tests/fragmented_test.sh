#!/bin/bash

exec 3<>/dev/tcp/localhost/8080
echo -n "Hello, " >&3
sleep 1
echo "world!" >&3
