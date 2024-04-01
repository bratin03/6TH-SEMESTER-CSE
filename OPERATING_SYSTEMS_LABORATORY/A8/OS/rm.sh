#!/bin/bash

# Get a list of all semaphores and shared memory message queues
ipcs_output=$(ipcs -s)

# Loop through each line of the ipcs output
while IFS= read -r line; do
    # Check if the line contains "666" (indicating permissions)
    if [[ $line =~ .*666.* ]]; then
        # Get the ID of the resource
        resource_id=$(echo "$line" | awk '{print $2}')
        # Remove the resource
        ipcrm -s $resource_id

    fi
done <<< "$ipcs_output"

# Get a list of all shared memory segments
ipcs_output=$(ipcs -m)

# Loop through each line of the ipcs output
while IFS= read -r line; do
    # Check if the line contains "666" (indicating permissions)
    if [[ $line =~ .*666.* ]]; then
        # Get the ID of the resource
        resource_id=$(echo "$line" | awk '{print $2}')
        # Remove the resource
        ipcrm -m $resource_id
    fi
done <<< "$ipcs_output"

# Get a list of all message queues
ipcs_output=$(ipcs -q)

# Loop through each line of the ipcs output
while IFS= read -r line; do
    # Check if the line contains "666" (indicating permissions)
    if [[ $line =~ .*666.* ]]; then
        # Get the ID of the resource
        resource_id=$(echo "$line" | awk '{print $2}')
        # Remove the resource
        ipcrm -q $resource_id
    fi
done <<< "$ipcs_output"

echo "Cleanup complete."
