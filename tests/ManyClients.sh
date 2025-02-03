#!/bin/bash

# Number of clients to simulate
CLIENTS=100

# Run the test
for i in $(seq 1 $CLIENTS); do
    (
        echo -ne "PASS password123 \r\nNICK user$i\r\nUSER user$i 0 * :User $i\r\nJOIN #stress\r\n" | nc localhost 6667
    ) &
done

# Wait for all background processes to finish
wait

echo "Stress test with $CLIENTS clients completed!"