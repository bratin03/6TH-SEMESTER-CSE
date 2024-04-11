#!/bin/bash

# Define the range of IP addresses
start_ip="10.5.16.145"
end_ip="10.5.16.146"

# Define the file to copy
file_to_copy="./.cse"

# Define the username to use for SSH login
username="user"

# Define timeout for SSH connection attempt
timeout_duration=3

# Prompt for the password
password="user12"

# Loop through the IP addresses
for ((i=$(echo $start_ip | cut -d '.' -f 4); i<= $(echo $end_ip | cut -d '.' -f 4); i++)); do
    ip=$(echo $start_ip | cut -d '.' -f 1-3).$i
    echo "Copying file to $ip..."
    timeout $timeout_duration sshpass -p $password scp -rp $file_to_copy $username@$ip:/home/$username/Public
    if [ $? -eq 0 ]; then
        echo "File copied successfully to $ip"
    else
        echo "Failed to copy file to $ip"
    fi
done
