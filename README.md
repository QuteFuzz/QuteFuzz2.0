# QuteFuzz

<p align="center" width="100%">
    <img width="25%" src="images/qutefuzz.png">

A front-end agnostic tool for fuzzing quantum compilers. 

See wiki for more details.

## Running

### Build with:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Use `cmake -DCMAKE_BUILD_TYPE=Debug ..` for debug symbols and other logging info, run with `./fuzzer`.

### Commands:
- `h` : help
- `quit`: quit program
- `print`: print grammar if any has been set
- `print_tokens`: print tokenisation of grammar if any has been set
- `<grammar> <entry_point>`: set grammar. For example, `pytket program` sets the `pytket` grammar an builds AST with `program` entry point
- `<n>`: generate n programs from the set grammar
- `plot`: Show probability distribution of results
- `verbose`: Verbose mode shows which python commands are being run while testing the cirucits

Grammars are written in `examples` folder. 

## Ideas

- [AFL](https://github.com/google/AFL): Feedback to the fuzzer on which circuits are good circuits for testing
- Automatic test reduction
- How to generate loops that terminate?

## Todo
- [x] Extend QIG to work for any number of qubits
- [ ] Make the AST modifiable to allow polymorphic testing
- [ ] Add support for loops
- [ ] Add support for other front-ends 