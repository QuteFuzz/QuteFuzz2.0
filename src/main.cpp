#include <iostream>
#include <string>
#include <vector>
#include "../include/ast.h"
#include "../include/run.h"

int main(){
    
    fs::path file("../examples/testbed.bnf");

    // testing the lexer
    // Lexer lexer(file.string());
    // lexer.print_tokens();

    // testing grammar builder
    // Grammar grammar(file);
    // grammar.print_tokens();
    // grammar.build_grammar();
    // grammar.print_grammar();

    //run entire application
    Run run("../examples");
    run.loop();

    return 0;
}
