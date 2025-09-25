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
| [Issue 623 &#x1F41E;](https://github.com/CQCL/pytket-quantinuum/issues/623) |  |
| [Issue 2004  &#x1F41E;](https://github.com/CQCL/tket/issues/2004) | 
| | |

### Guppy:

| Compiler Bugs | Other Bugs |
|---------------|------------|
| [Issue 1122 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1122)  | [Issue 1101 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1101)|
| [Issue 1224 &#x1F41E;](https://github.com/CQCL/guppylang/issues/1224) | |
| [Issue 1079 &#x1F41E;](https://github.com/CQCL/tket2/issues/1079) | |
| | |

## Manual testing procedures

After the desired tests have been executed, all the results will be stored in text form in `outputs/result.txt`. The easiest way to find errors is to search for the term: "Interesting circuit found:", and then find all the interesting circuit outputs.

A circuit is determined to be interesting if one of the following occurs:
- K-S test value falls below 0.05
- K-S test value falls below 0.2 for 2 or more times
- Program throws an exception
- Program times out (only guppy ks_diff_test)

These testing methods are all contained within `diff_testing/lib.py`, where the testing methods are categorized by a unique class for each frontend.

Upon looking through the results text file, the corresponding interesting circuits will have been saved into an alternative directory in `outputs/interesting_circuits`. These files can then be manually ran again using `python outputs/interesting_circuits/circuit__.py` with `__` being the circuit number of interest after looking at the results text file. These test cases can then be manually tweaked by removing gates/subcircuits until the bug can be reproduced with the minimal number of gates. 
