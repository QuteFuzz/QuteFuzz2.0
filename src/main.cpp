#include <iostream>
#include <string>
#include <vector>
#include "../include/ast.h"
#include "../include/run.h"

int main(int argc, char* argv[]){
    
    fs::path file("../examples/testbed.bnf");
    // testing the lexer
    //Lexer lexer(file.string());
    //lexer.print_tokens();

    // testing grammar builder
    //Grammar grammar(file);
    //grammar.print_tokens();
    //grammar.build_grammar();
    //grammar.print_grammar();

    // Set default values for command line arguments
    bool debug_mode = false;
    int num_circuits = 1;
    std::string language = "qiskit"; // default language

    // Parse arguments
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == "-d") {
            debug_mode = true;
        } else if(arg == "-n" && i+1 < argc) {
            num_circuits = std::stoi(argv[++i]);
        } else if(arg == "-c" && i+1 < argc) {
            language = "cirq";
        } else if(arg == "-q") {
            language = "qiskit";
        } else if(arg == "-p") {
            language = "pytket";
        }
    }
    
    //run entire application
    Run run("../examples");
    if(debug_mode) {
        run.loop();
    } else {
        run.generate_circs(num_circuits, language);
    }

    return 0;
}
