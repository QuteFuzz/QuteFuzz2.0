# GramParse

An idea for where QuteFuzz 2.0 might head. 

## Premise

- The program receives a path to grammar in BNF form then outputs text based on that grammar, see `examples` folder.
- Each branch can be manually assigned a probability of being picked using `[prob]` next to it, or you can write `equal_prob` at the top of the bnf file to make all branches in each rule equally probable
- The idea is that we have a grammar representation on top of which we can build other things. Here, the grammar is parsed to output text directly, but Dr Wickerson had spoken about making our circuit generation more "graph based". Perhaps we could write for each QSS `QSS Grammar -> QSS AST`, then we can play with the AST to edit the circuit in any way we want before doing `QSS AST -> QSS Program`. 

## Running

Build with:

```sh
mkdir build
cd build
cmake ..
make
```

Run with `./fuzzer`, and type `h` for help.

## Ideas for the future
- [ ] QSS grammar definition
- [ ] Writing relevant parsers
- [ ] Thinking about what cool techniques we could use if we have an AST of the circuit
