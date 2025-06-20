# QuteFuzz

A grammar to AST to program fuzzer. 

## Premise

- The program receives a path to grammar in BNF form then outputs text based on that grammar, see `examples` folder.
- Each branch will be assigned equal probability of being picked.
- The idea is that we have a grammar representation on top of which we can build other things. Here, the grammar is parsed to output text directly, but Dr Wickerson had spoken about making our circuit generation more "graph based". Perhaps we could write for each QSS: 1. `QSS Grammar -> QSS AST`, then we can play with the AST to edit the circuit in any way we want before doing 2. `QSS AST -> QSS Program`. 
- The first step will probably be the hardest part, because I think this is where we do type checking and other checks to make sure the program is well formed

## Grammar parser

### How to use
- Write grammar in BNF syntax. 
- As a convention, I use lower case for rules, and uppercase for syntax tokens. This isn't needed for functionality. 
- `tokens.bnf` should be used for any tokens. This gets parsed first, and its grammar is added to the definitions of all other grammars.
- Gates, imports, compiler calls, float literals, qubit register names, qubit register sizes, qubits can also be written in the grammar but not defined. These are replaced with actual values while parsing the AST. See [`pytket.bnf`](examples/pytket.bnf).
- Wildcards are expanded out, with a maximum set to 50. So `expr+` becomes `expr | expr expr | .... | expr ... expr (50 times)` in memory. 

### Limitations

The grammar parser can handle most of the BNF syntax, up to simple groupings with / without wildcards like `(term (expr)+ hello)*`. However, things like `term ("+" | "-") expr` are tokenised, but not parsed correctly. 

## AST builder

These are the [constraints](include/constraints.h) defined, which are activated as required by the context.

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

Grammars are written in `examples` folder. 

## Ideas

- [AFL](https://github.com/google/AFL): Feedback to the fuzzer on which circuits are good circuits for testing
- Automatic test reduction
- How to generate loops that terminate?