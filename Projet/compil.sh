#!/bin/bash

input_directory="code"
output_directory="exe"

if [ ! -d "$output_directory" ]; then
    mkdir "$output_directory"
    echo "Created directory: $output_directory"
fi

for file in "$input_directory"/*.cpp; do
    base_name=$(basename "$file" .cpp)
    g++ -o "$output_directory/$base_name" "$file"
    
    echo "Compiled $file to $output_directory/$base_name"
done
