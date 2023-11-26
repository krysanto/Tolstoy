#!/bin/sh

# Check if Makefile exists in the current directory
if [ -f "Makefile" ]; then
    echo "Makefile found. Running make..."
    make
else
    echo "No Makefile found in the current directory."
fi
