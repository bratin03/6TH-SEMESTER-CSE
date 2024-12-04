#!/bin/bash

# Loop to execute client.out in 10 different terminals
for i in {1..10}; do
    gnome-terminal -- ./client.out &
done

