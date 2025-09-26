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

## Cross-qss testing

### How to use it
This can only be done using the `benny-quantinuum-old` branch of the QuteFuzz repository and following the steps below: (Note that due to this feature is being deprecated for a re-write that uses similar syntax and better formatting.)
- After setting up Docker and building the project in `README.md`, launch QuteFuzz using `./fuzzer`
- Type in either `pytket_cq program` or `guppy_cq program` and press `Enter`
    - This will set your base grammar to be either pytket or guppy
- Enter Cross-QSS mode by entering `cross-qss` and  pressing `Enter`
    - A message should tell you that you’re currently in cross-qss mode
- Enter either `pytket_cq program` or `guppy_cq program`, whichever one being the different language to be tested against and press `Enter`
- Enter the number of tests to be carried out as an integer e.g. 10 and then press `Enter`
- After the circuits have been generated, simply type run_tests and press `Enter`
- The results of the testing will be printed in a file called `results.txt` in the same outputs folder as where the random circuits are generated. Manual test case reduction is necessary which is outlined above.
- If another language needs to be tested, simply disable the `cross-qss` testing mode by entering the same `cross-qss` command again and entering a new grammar with a new entry point

### How it works

The cross-qss testing feature works by generating DAGs, which is an object with specific information on the qubit properties, operations done on them nd their definition (singular or register, internal or external). This information is needed to minimally construct a valid quantum program in any qss and therefore kept in the DAG data structure. This data is then passed into the AST generator in accordance to a pre-defined grammar that is stripped down to only include the essentials to ensure compatibility. This includes removing the notion of internal qubits in Guppy, since there are no equivalents in Pytket. The technical details of how this is carried out is detailed below:

The entry point of teh `cross_qss` testing starts from the call to `generate_cross_qss()` located in `src/generator.cpp`. This function generates random programs in a base language defined before entering the `cross-qss` testing mode, and then uses the generated DAG stored in the `genome` object to pass into the `ast_to_program` function for traversal and re-creation of semantically equivalent programs in the desired languages.

Upon calling the `ast_to_program` function having passed in a DAG wrapped in a `genome` object, the `build()` function is called, allowing an AST to be built as per normal until critical nodes are encountered and need to be generated. This all happens in the `src/ast/ast.cpp` file, where the giant switch statement handles the various nodes that can be created and have necessary information added to the global context to facilitate special generation rules.

The most important switch cases that `cross-qss` mode relies on, are the `new_gate()` calls all the way at the bottom of the switch cases, as well as the fundamental areas essential to what constitutes a quantum circuit:
- `case Common::qubit_defs_external:`, `case Common::qubit_defs_internal:`, `case Common::qubit_defs_external_owned:`
    - These cases handle the declatation/definition of qubits in a particular language, like adding qubits to a circuit in Pytket or defining a `qubit()` variable in Guppy
- `case Common::block`
    - This is a node that encompasses everything a circuit/subcircuit contains, and therefore the exact same name and number of qubit resource it contains should be copied over from the DAG
- `case Common::gate_name`
    - This is a greatly important case since different gates names can take on different capitalization across qss, like how everything is lowercase in Guppy but not so in Pytket
- `case Common::gate_op_kind`
    - This is another important part since the format of how qubits and arguments are passed into quantum gates differ between qsses, so any tweaks here and the associated function in `src/ast/context.cpp`, `make_gate_name()` will have an effect on this.
- `case Common::qubit_op`
    - This node is the parent of the above two, and therefore need to carry out important DAG traversal to give the context critical information.

All the cases above calls their corresponding counterpart functions in the `context.cpp` file. A good rule of thumb is that any functions with reference to `Common::cross-qss` probably has something to do with cross-qss testing.

