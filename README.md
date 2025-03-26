# QuteFuzz 2.0

An idea for where QuteFuzz 2.0 might head. 

## Premise

- The program receives a path to grammar in BNF form then outputs text based on that grammar, see `examples` folder.
- Each branch will be assigned equal probability of being picked.
- The idea is that we have a grammar representation on top of which we can build other things. Here, the grammar is parsed to output text directly, but Dr Wickerson had spoken about making our circuit generation more "graph based". Perhaps we could write for each QSS: 1. `QSS Grammar -> QSS AST`, then we can play with the AST to edit the circuit in any way we want before doing 2. `QSS AST -> QSS Program`. 
- The first step will probably be the hardest part, because I think this is where we do type checking and other checks to make sure the program is well formed

## Grammar parser limitations

The grammar parser can handle most of the BNF syntax, up to simple groupings with / without wildcards like `(term (expr)+ hello)*`. However, things like `term ("+" | "-") expr` and `[1-9]` are tokenised, but not parsed correctly. 

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