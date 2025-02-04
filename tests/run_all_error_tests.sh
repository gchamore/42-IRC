#!/bin/bash

# Define the directory where the test scripts are located
TEST_DIR="/home/gchamore/42Paris/projects_on_duty/irc/tests/Errors"

# List of test scripts
TEST_SCRIPTS=(
    "PASS_errors.sh"
    "NICK_errors.sh"
    "USER_errors.sh"
    "JOIN_errors.sh"
    "WHO_errors.sh"
    "PRIVMSG_errors.sh"
    "PART_errors.sh"
    "QUIT_errors.sh"
    "MODE_errors.sh"
)

# Run each test script
for script in "${TEST_SCRIPTS[@]}"; do
    echo "Running $script..."
    bash "$TEST_DIR/$script"
    echo "$script completed."
    echo "-----------------------------"
done

echo "All tests completed."
