#include "../include/run.h"

Run::Run(const std::string& _grammars_dir) : grammars_dir(_grammars_dir) {
    // build all grammars
    try{

        if(fs::exists(grammars_dir) && fs::is_directory(grammars_dir)){
            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().extension() == ".bnf")){
                    Grammar grammar(file);
                    grammar.build_grammar();

                    std::cout << "Building " << grammar.get_name() << std::endl;
                    
                    grammars[grammar.get_name()] = grammar;
                }
            }
        }

    } catch (const fs::filesystem_error& error) {
        std::cout << error.what() << std::endl;
    }

}

void Run::set_grammar(){

    std::string grammar_name = tokens[0], entry_name = tokens[1];

    if(is_grammar(grammar_name)){
        current_grammar = &grammars[grammar_name];
        // write.set_grammar(grammars[grammar_name], entry_name);
    } else {
        std::cout << grammar_name << " is not a known grammar!" << std::endl;
    }
}

void Run::tokenise(const std::string& command){

    std::stringstream ss(command);
    std::string token;

    tokens.clear();

    while(std::getline(ss, token, ' ')){
        tokens.push_back(token);
    }
}

void Run::loop(){

    std::string current_command;

    while(run){
        std::cout << "> ";

        std::getline(std::cin, current_command);

        if(current_command == "quit"){
            run = false;
        } else if (current_command == ""){
            // write.emit();
        } else if (current_command == "h"){
            help();
        } else if ((current_command == "print") && (current_grammar != NULL)){
            current_grammar->print_grammar();
        } else if ((current_command == "print_tokens") && (current_grammar != NULL)){
            current_grammar->print_tokens();
        } else {
            tokenise(current_command);

            if(tokens.size() == 2){
                set_grammar();
            }
        }
    }
}


