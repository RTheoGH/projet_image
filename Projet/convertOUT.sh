#!/bin/bash

input_directory="out"
output_directory="png"

if [ ! -d "$input_directory/$output_directory" ]; then
    mkdir "$input_directory/$output_directory"
    echo "Created directory: $output_directory"
fi

for image_file in "$input_directory"/*/*.{pgm,ppm}; do
  if [ ! -e "$image_file" ]; then
    echo "No .pgm or .ppm files found in the directory."
    break
  fi
  
  base_name=$(basename "$image_file")
  extension="${base_name##*.}"
  base_name="${base_name%.*}"

  convert "$image_file" "$input_directory/$output_directory/$base_name.png"
  
  echo "Converted $image_file to $input_directory/$base_name.png"
done
