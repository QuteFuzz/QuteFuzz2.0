# QuteFuzz

<p align="center" width="100%">
    <img width="25%" src="etc/qutefuzz.png">

A tool for fuzzing quantum compilers.

See wiki for more details.

[![Unitary Foundation](https://img.shields.io/badge/Supported%20By-UNITARY%20FOUNDATION-brightgreen.svg?style=for-the-badge)](https://unitary.foundation)

## Running

### Build with:

You need to install docker and use our dockerfile to set up the environment. After [installing docker](https://docs.docker.com/engine/install/), follow these steps.

1. Set up environment from dockerfile

```
docker build -t qutefuzz-env .
```

2. Run docker container
```
docker run -it --rm -v "$PWD":/qutefuzz qutefuzz-env
```

This will drop you into a shell within the environment where all dependencies are installed

3. Compile project

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Use `cmake -DCMAKE_BUILD_TYPE=Debug ..` for debug symbols and other logging info

4. Run with `./fuzzer`.

See [wiki](https://github.com/QuteFuzz/QuteFuzz2.0/wiki/Interacting-with-the-tool) for help on how to intertact with the tool

## Bugs found with the help of QuteFuzz 2.0

### Pytket:

| Compiler Bugs | Other Bugs |
|---------------|------------|
| [Issue 623 &#x1F41E;](https://github.com/CQCL/pytket-quantinuum/issues/623) | |
| [Issue 2004  &#x1F41E;](https://github.com/CQCL/tket/issues/2004) | 
| | |

### Guppy:

| Compiler Bugs | Other Bugs |
|---------------|------------|
| [Issue 1122 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1122)  | [Issue 1101 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1101)|
| [Issue 1224 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1224) | |
| [Issue 1079 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1079) | |
| | |



