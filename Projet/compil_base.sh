#!/bin/bash

input_directory="code"
output_directory="exe"

if [ ! -d "$output_directory" ]; then
    mkdir "$output_directory"
    echo "Created directory: $output_directory"
fi

g++ -o "$output_directory/mosaique_laplacien" "$input_directory/mosaique_laplacien.cpp" "$input_directory/ImageBase.cpp"

echo "Compilation terminée."
