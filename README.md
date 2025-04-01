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
- The tokens below are known as "common" syntax tokens, and therefore do not need to be defined in the grammar. 
Use the names below (case-agnostic), and the corresponding token will be written to the program:
```
lparen, "("
rparen, ")"
comma, ","
space, " "
dot, "."
single_quote, "\'"
double_pipe, "||"
double_quote, "\""
double_ampersand, "&&"
equals, " = "
```
Gates have not been added as those could have different APIs depending on the front-end.

### Limitations

The grammar parser can handle most of the BNF syntax, up to simple groupings with / without wildcards like `(term (expr)+ hello)*`. However, things like `term ("+" | "-") expr` are tokenised, but not parsed correctly. 

These are small bits that can be added later.

## Running

Build with:

```sh
mkdir build
cd build
cmake ..
make
```

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
