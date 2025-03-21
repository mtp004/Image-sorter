cas# Image Sorter

A simple command-line utility to organize and sort image files from your directories. Currently, the config file contains only image content, but you can actually sort any kind of file as long as that file extensiion is supported in your native system.

## Overview

Image Sorter scans specified directories for image files, moves them to a dedicated "images" folder, and optionally sends the organized folder to the trash.

## Installation

1. Clone the repository:
   ```
   git clone https://github.com/yourusername/image-sorter.git
   ```

2. Compile the program:
   ```
   make
   ```

## Configuration

Before running the program, create a `config.txt` file in the project directory with the following format:

```
filetypes=.jpg .jpeg .png .gif .webp
trashPath=/Users/yourusername/.Trash
```

- First line: List of file extensions to identify as images
- Second line: Path to your system's trash directory

## Usage

1. Run the executable by running make run or clicking on the executable itself:
   ```
   ./image-sorter
   ```

2. Enter the HOME subdirectories you want to scan (separated by spaces):
   ```
   Enter HOME subdirectories separated by space | Press [ENTER] to exit program:
   Desktop Downloads Documents
   ```

3. For each directory, the program will:
   - Create an "images" folder
   - Move all image files to that folder
   - Ask if you want to move the "images" folder to the trash

4. Press ENTER with no input to exit the program

## Example

```
$ ./image-sorter
Program starting...
Loaded file types:
.jpg
.png
.gif
Trash bin path: /Users/yourusername/.Trash

Enter HOME subdirectories separated by space | Press [ENTER] to exit program:
Downloads

Downloads path: /Users/yourusername/Downloads
Created folder: /Users/yourusername/Downloads/images
  Found image: vacation.jpg
    Moved -> /Users/yourusername/Downloads/images/vacation.jpg
  Found image: screenshot.png
    Moved -> /Users/yourusername/Downloads/images/screenshot.png
Move folder to Trash? (y/n): y
Success: /Users/yourusername/Downloads/images ==> Trash
SUCCESS
```

## License

[MIT License](LICENSE)