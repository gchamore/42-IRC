#!/bin/bash

# Cleanup function
cleanup() {
    echo "Cleaning up..."
    tc qdisc del dev lo root netem 2>/dev/null
    exit "${1:-0}"
}

# Set trap for cleanup on script interruption
trap cleanup SIGINT SIGTERM

# Ensure script is run with sudo
if [ "$EUID" -ne 0 ]; then 
    echo "Please run with sudo"
    exit 1
fi

# Check if IRC server is running
nc -z localhost 6667 || {
    echo "Error: IRC server not running on port 6667"
    exit 1
}

# Clear any existing rules
tc qdisc del dev lo root netem 2>/dev/null

# Add 300ms latency
echo "Adding 300ms latency..."
tc qdisc add dev lo root netem delay 300ms || {
    echo "Failed to add latency"
    cleanup 1
}

# Send IRC commands with delays and proper line endings
echo "Sending IRC commands..."
{
    printf "PASS password123\r\n"
    sleep 1
    printf "NICK laguser\r\n"
    sleep 1
    printf "USER laguser 0 * :Lag Test\r\n"
    sleep 2
    printf "JOIN #lagchannel\r\n"
    sleep 2
    printf "PRIVMSG #lagchannel :Testing under high latency\r\n"
    sleep 2
    printf "QUIT :Test complete\r\n"
} | nc -w 10 -v localhost 6667

# Cleanup and exit
cleanup