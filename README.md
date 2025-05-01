# QuteFuzz 2.0

An idea for where QuteFuzz 2.0 might head. 

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

A constraint system ensures that the AST is well formed. 

- The AST builder follows the grammar, picking a branch at random as long as it satisfies a set of constraints. The default AST which just follows the grammar blindly has only one constraint which prevents infinite recursion. See [`src/ast.cpp`](src/ast.cpp#L16-#L21)

- The pytket AST has other interesting constraints. Constraints are added depending on which node in the AST is being built. For instance, here's a set of constraints for single qubit gates:

```C++
case Common::h: case Common::x: case Common::y: case Common::z:
        constraints.add_constraint(Constraints::Constraint(gate_application, Constraints::BRANCH_SIZE_EQUALS, 1)); 
        constraints.add_constraint(Constraints::Constraint(qubit_list, Constraints::BRANCH_SIZE_EQUALS, 1)); 
        break;
```

Where `gate_application` and `qubit_list` are rules defined in the grammar.

## Running

Build with:

```sh
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make
```

Use `cmake -DCMAKE_BUILD_TYPE=Debug ..` for debug symbols and other logging info. 

Run with `./fuzzer`, and type `h` for help.

An AST for [`testbed.bnf`](examples/testbed.bnf) can be produced by typing in the commands `testbed <entry_point>`.

Example result for `testbed expr`:
```
AST: 
[] children: 1 depth: 0
->[term] children: 1 depth: 1
        ->[factor] children: 1 depth: 2
                ->[integer] children: 1 depth: 3
                        ->["4"] children: 0 depth: 4
```

Example result for `testbed factor`:
```
AST: 
[] children: 3 depth: 0
->["("] children: 0 depth: 1
->[expr] children: 1 depth: 1
        ->[term] children: 1 depth: 2
                ->[factor] children: 1 depth: 3
                        ->[integer] children: 1 depth: 4
                                ->["4"] children: 0 depth: 5
->[")"] children: 0 depth: 1
```

An AST for [`pytket.bnf`](examples/pytket.bnf) can be produced in the same way. Since an AST builder is defined for it, a program will be written to `outputs/output.py`.  
