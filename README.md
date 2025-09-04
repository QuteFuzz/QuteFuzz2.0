# QuteFuzz

<p align="center" width="100%">
    <img width="25%" src="etc/qutefuzz.png">

A tool for fuzzing quantum compilers.

See wiki for more details.

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

### Commands

- `h` : help
- `quit`: quit program
- `print`: print grammar if any has been set
- `print_tokens`: print tokenisation of grammar if any has been set
- `<grammar> <entry_point>`: set grammar. For example, `pytket program` sets the `pytket` grammar an builds AST with `program` entry point
- `<n>`: generate n programs from the set grammar
- `plot`: Show probability distribution of results
- `verbose`: Verbose mode shows which python commands are being run while testing the cirucits
- `run_tests`: Run differential testing on the circuits

Grammars are written in `examples` folder.
Outputs are in the `outputs` folder. 

## Bugs found with the help of QuteFuzz 2.0

### Pytket:

| Compiler Bugs | Other Bugs |
|---------------|------------|
| [Issue 623 &#x1F41E;](https://github.com/CQCL/pytket-quantinuum/issues/623) | [Issue 374 &#x1F41E;](https://github.com/CQCL/pytket-qiskit/issues/374) |
| [Issue 2004  &#x1F41E;](https://github.com/CQCL/tket/issues/2004) | 
| | |

### Guppy:

| Compiler Bugs | Other Bugs |
|---------------|------------|
| [Issue 1122 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1122)  | [Issue 1101 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1101)|
| [Issue 1224 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1224) | |
| [Issue 1079 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1079) | |
| | |

## Ideas

- [AFL](https://github.com/google/AFL): Feedback to the fuzzer on which circuits are good circuits for testing
- Automatic test reduction
- How to generate loops that terminate?

