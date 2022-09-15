# pixel-analyzer
Finds the most common pixel in an image and returns the closest accepted plaintext name and its hexadecimal representation.

# Setup
For now, there will only be instructions for systems which use the apt package manager, but more may be added in the future.
1. Be sure to update first: `apt update && apt upgrade -y`.
2. Install ImageMagick by running the command `apt install imagemagick`.
3. Clone this project and enter the directory (`cd pixel-analyzer`).
4. Initialize submodules via `git submodule update --init --recursive`.

# Compilation
1. I recommend g++ for this project. It can be installed via `apt install g++`.
2. Run the following command with all the associated config flags.
```sh
g++ pixel-analyzer.cpp `Magick++-config --cxxflags --cppflags --ldflags --libs`
```

# Usage
Any image format supported by ImageMagick is accepted.
```sh
$ ./a.out filename.jpg
$ ./a.out filename2.png
$ ./a.out filename3.tiff
...
```
