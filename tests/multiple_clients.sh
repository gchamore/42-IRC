#!/bin/bash

for i in {1..10}; do
    echo "Hello from client $i" | nc localhost 6667 &
done