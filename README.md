# doxy2md

doxy2md is a command line tool for generating markdown files from doxygen XML output.

## Download

This project is in a pre-release state. So there are no binaries available to download.

## Building

To build, you need to have `cmake`, `git`, and a `clang`/`mingw`/`gcc` compiler installed. The commands below will use `ninja` but that is not required.



```bash
git clone https://github.com/StratifyLabs/doxy2md
cd doxy2md
cmake -P bootstrap.cmake
source profile.sh
cd cmake_link
cmake .. -GNinja
ninja install
```

By default, `doxy2md` is copied to the SDK folder that is created in the bootstrap process. You can use the following to see the full path of the tool.

```bash
which doxy2md
```

## Running

```
doxy2md ...
```
