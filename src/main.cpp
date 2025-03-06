#include <iostream>
#include "../include/write.h"
#include "../include/run.h"

int main(int argc, char** argv){
    fs::path file("../examples/openqasm.bnf");

    // testing the lexer
    //Lexer lexer(file.string());
    //lexer.print_tokens();

    // testing grammar builder
    //Grammar grammar(file);
    //grammar.build_grammar();
    //grammar.print_grammar();

    // testing the writer
    //Write write(file, "qreg_declaration");
    //write.emit();

    // run entire application
    Run run("../examples");
    run.loop();

    return 0;
}
