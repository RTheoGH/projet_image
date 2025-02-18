#!/bin/bash

# Directory containing the .pgm and .ppm files
input_directory="in"

# Loop through each .pgm and .ppm file in the directory
for image_file in "$input_directory"/*.{pgm,ppm}; do
  # Check if there are no .pgm or .ppm files
  if [ ! -e "$image_file" ]; then
    echo "No .pgm or .ppm files found in the directory."
    break
  fi
  
  # Get the base name of the file (without extension)
  base_name=$(basename "$image_file")
  extension="${base_name##*.}"       # Extract the file extension
  base_name="${base_name%.*}"        # Remove the extension

  # Convert to .png using the convert command
  convert "$image_file" "out/png/$base_name.png"
  
  echo "Converted $image_file to $input_directory/$base_name.png"
done
