# QuteFuzz

A fuzzer for quantum compilers. 

## How it works
1. Provide a grammar. Grammar must define particular rules to guide the AST generation.
2. Qutefuzz generates a well-formed AST from the grammar
3. Qutefuzz prints the circuit from the AST. 

## Grammar parser
- Write grammar in BNF syntax. 
- As a convention, I use lower case for rules, and uppercase for syntax tokens. This isn't needed for functionality. 
- `tokens.bnf` should be used for any tokens, but you may define new tokens in the file for the specific grammar being defined. `tokens.bnf` gets parsed first, and its grammar is added to the definitions of all other grammars.
- Gates, imports, compiler calls, float literals, qubit register names, qubit register sizes, qubits can also be written in the grammar but not defined. These are replaced with actual values while parsing the AST. See [`pytket.bnf`](examples/pytket.bnf).
- Wildcards are expanded out, with a maximum set to 50. So `expr+` becomes `expr | expr expr | .... | expr ... expr (50 times)` in memory. 

### Limitations
The grammar parser can handle most of the BNF syntax, up to simple groupings with / without wildcards like `(term (expr)+ hello)*`. However, things like `term ("+" | "-") expr` are tokenised, but not parsed correctly. 

## AST builder
Constraints are added depending on certain rules to limit which branches can be chosen from the grammar. 

## QIG (Qubit Interaction Graph)
When adding any 2 qubit gate, QuteFuzz will try all possible pairs of qubits it can use in the given circuit, score each pair, and choose that with the highest score. The score is a heuristic chosen to promote generation of circuits that stress test the compiler.

### Hueristics (work in progress)
- Diameter: The longest shortest path in the graph. 
- Average Shortest Path: The average of all shortest paths in the graph. 

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

Output is a circuit, as well as the QIG for the main circuit in DOT language. This can be rendered using `dot`. For instance:
```
dot -Tpng ../outputs/output1.dot -o ../outputs/output1.png
```

## Ideas

- [AFL](https://github.com/google/AFL): Feedback to the fuzzer on which circuits are good circuits for testing
- Automatic test reduction
- How to generate loops that terminate?