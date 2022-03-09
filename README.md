# cli

This is a command Line Tool Template Project. It includes Stratify Labs API libraries for basic tasks:

- FsAPI: Filesytem routines
- InetAPI: Internet including secure sockets
- JsonAPI: JSON file reading/writing

To build:

```
git clone https://github.com/StratifyLabs/cli
cmake -P bootstrap.cmake
source profile.sh
cd cmake_link
cmake .. -GNinja
ninja install
```

To Run:

```
cli
cli --json
```

To Test:

The project includes some simple example tests for the `cli` program as well as
unit tests for SDK libraries.

```bash
cmake .. -DAPI_IS_TEST=ON -DJSON_API_IS_TEST=ON
ninja test
```

To have the tests show the output, use:

```bash
ctest -VV
```