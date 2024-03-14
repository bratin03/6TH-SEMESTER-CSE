#!/bin/bash

# Function to extract the number of leaf nodes from tree.txt
get_num_leaf_nodes() {
    # Read the total number of nodes from the first line of tree.txt
    local num_nodes=$(head -n 1 tree.txt)
    # Initialize an array to track if a node is a parent (1) or leaf (0)
    local is_parent=()
    for ((i=0; i<$num_nodes; i++)); do
        is_parent[$i]=0
    done

    # Read each line after the first one in tree.txt using process substitution
    while read -r child parent; do
        # Convert parent to an integer
        temp=$((parent))
        # Mark parent node in the array
        is_parent[$temp]=1
    done < <(tail -n +2 tree.txt)

    # Count the number of leaf nodes (nodes marked as not parent)
    local num_leaf_nodes=0
    for ((i=0; i<$num_nodes; i++)); do
        if [[ ${is_parent[$i]} -eq 0 ]]; then
            ((num_leaf_nodes++))
        fi
    done

    # Print the number of leaf nodes
    echo $num_leaf_nodes
}

for ((i=1; i<=100; i++)); do
    echo "Run $i:"
    make newrun
    # Get the number of leaf nodes from tree.txt
    num_leaf_nodes=$(get_num_leaf_nodes)

    # Generate random numbers for leaf nodes
    random_inputs=""
    for ((j=0; j<$num_leaf_nodes; j++)); do
        random_input=$((RANDOM % 100))  # Adjust the range as needed
        random_inputs+="$random_input "
    done

    echo "$random_inputs"
    # Execute newrun with random inputs

    echo "------------------------------------------------------"
done
