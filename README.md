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

With an AST, we have the potential to do some cool techniques we couldn't have done before. 

There's 2 nice things about this AST builder, the constriant system and the dependency system. 

- We can add constraints on particular nodes such that the corresponding rule in the grammar picks branches with certain [characteristics](include/constraints.h#L8). 
- There's a set of constraints defined, which can be flagged to activate them. When the constraint is satisfied, its flag is reset (constraint deactivated), unless it is a "global" constraint, which means that it must always be specified regardless of context. For instance, a constraint on the gateset that gates should come from is always the same regardless of context, while a constraint on the number of qubits the rule `qubit_list` must choose is dependent on the gate that has been picked.
- The dependency system allows us to define node dependencies that must be satisfied while the AST is being built. There's only support for one set of node dependencies in the AST, which is defined by having one completer and a set of initialisers. Completers are the nodes which give information which is used by initiators to build their parts of the AST. 
- There's dependencies defined in the builder between `qreg_defs`, `subroutines`(initiators) and `gate_application`(completer), such that the number of qubits, and subroutine size generated in the AST is a function of the number of gate applications.

These are the [constraints](include/constraints.h) defined, 
```C++
#define ON_RULES_CONSTRAINT(node_hash, type, n) (Constraint(node_hash, type, n))
#define N_QUBIT_CONSTRAINT(n) (ON_RULES_CONSTRAINT(Common::qubit_list, NUM_RULES_EQUALS, n))

/* 
        .........
*/

std::vector<Constraint> constraints = {
        N_QUBIT_CONSTRAINT(1),
        N_QUBIT_CONSTRAINT(2),
        N_QUBIT_CONSTRAINT(3),
        Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::float_literal, Common::qubit_list}),
        Constraint(Common::gate_application_kind, BRANCH_EQUALS, {Common::qubit_list}),
        Constraint(BRANCH_IS_NON_RECURSIVE),
        Constraint(Common::gate_name, BRANCH_IN, {Common::h, Common::x}, true),
};
```

which are activated as required by the context. The last one is global (true argument), so is "always activated". This is what allows us to set a gateset. 

## Ideas

- [AFL](https://github.com/google/AFL): Feedback to the fuzzer on which circuits are good circuits for testing
- Automatic test reduction
- How to generate loops that terminate?

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
