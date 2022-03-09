# doxy2md

doxy2md is a command line tool for generating markdown files from doxygen XML output.

## Building

To build:

```
git clone https://github.com/StratifyLabs/doxy2md
cmake -P bootstrap.cmake
cd cmake_link
cmake .. -GNinja
ninja install
```

## Running

```
doxy2md ...
```
