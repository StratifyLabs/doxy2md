# doxy2md

doxy2md is a command line tool for generating markdown files from doxygen XML output. It is written in C++ for C++ documentation.

## Download

This project is in a pre-release state. There are no binaries available to download.

## Building

To build, you need to have `cmake`, `git`, and a `clang`/`mingw`/`gcc` compiler installed. The commands below will use `ninja`, but you could also use another generator like `make`.

```bash
git clone https://github.com/StratifyLabs/doxy2md
cd doxy2md
cmake -P bootstrap.cmake
source profile.sh
cd cmake_link
cmake .. -GNinja
ninja install
```

By default, `doxy2md` is copied to the SDK folder that is created in the bootstrap process in a subfolder of the `doxy2md` repository. You can use the following to
see the full path of the tool.

```bash
which doxy2md
```

You can completely remove `doxy2md` from your computer by deleting the repository.

## Running

Before you can run `doxy2md`, you need to generate the `xml` output using `doxygen`. Then you specify the `source`, `destination`, and optionally the `configuration` file.

```bash
doxy2md --source=<path to xml folder> --destination=<path to markdown folder> --configuration=<path to json configuration file>
```

To get the default configuration use:

Show in `yaml` format:

```bash
doxy2md --showConfiguration
```

Show in `json` format:

```bash
doxy2md --showConfiguration --json
```

Save to the local folder as `config.json`

```bash
doxy2md --showConfiguration --json > config.json
```

## Contributing

MIT licensed (or similarly permissive) contributions are welcome. I built `doxy2md` because everything else I tried to use to convert `doxygen` output to `markdown` had shortcomings (especially when it came to `C++`). It is working for my purposes, but is designed to be highly flexible using the `configuration` input. 

It is written in `C++` so that `C++` developers can make changes optimized for documenting `C` and/or `C++` code.

## Example Output

The [documentation for the Stratify Labs API framework](https://docs.stratifylabs.dev/docs/application/Introduction) is generated using `doxy2md` and then published using [docusaurus](https://docusaurus.io/).