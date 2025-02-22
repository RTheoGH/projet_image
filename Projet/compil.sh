#!/bin/bash

input_directory="code"
for file in "$input_directory"/*.cpp; do
    base_name=$(basename "$file")
    extension="${base_name##*.}"       # Extract the file extension
    base_name="${base_name%.*}"        # Remove the extension

    g++ -o "exe/$base_name" "$file"
    
    echo "Compiled $file to exe/$base_name"
done
