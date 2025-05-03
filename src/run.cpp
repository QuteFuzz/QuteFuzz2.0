#include "../include/run.h"

Run::Run(const std::string& _grammars_dir) : grammars_dir(_grammars_dir) {
    // build all grammars
    try{

        if(fs::exists(grammars_dir) && fs::is_directory(grammars_dir)){
            Grammar commons_grammar;

            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().stem() == TOKENS_GRAMMAR_NAME)){
                    Grammar grammar(file);
                    grammar.build_grammar();

                    commons_grammar = grammar;
                    commons_grammar.mark_as_commons_grammar();

                    break;
                }
            }

            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().extension() == ".bnf") && (file.path().stem() != TOKENS_GRAMMAR_NAME)){

                    Grammar grammar(file);
                    grammar += commons_grammar;
                    grammar.build_grammar();

                    std::string name = grammar.get_name();
                    std::cout << "Built " << name << std::endl;
             
                    Program_Spec spec;
                    
                    spec.grammar = std::make_shared<Grammar>(grammar);
                    
                    if(name == "pytket"){
                        spec.builder = std::make_shared<Pytket>();
                        spec.extension = ".py";

                    } else if(name == "qiskit"){
                        spec.builder = std::make_shared<Qiskit>();
                        spec.extension = ".py";

                    } else if(name == "cirq"){
                        spec.builder = std::make_shared<Cirq>();
                        spec.extension = ".py";
                        
                    } else {
                        spec.builder = std::make_shared<Ast>();
                        spec.extension = ".txt";
                    }

                    specs[name] = std::make_shared<Program_Spec>(spec);
                    
                }

            }

        }

        // prepare outputs directory
        output_dir = grammars_dir.parent_path() / OUTPUTS_FOLDER_NAME;
        
        if(!fs::exists(output_dir)){
            fs::create_directory(output_dir);
        } else {

            for(auto& entry : fs::directory_iterator(output_dir)){
                fs::remove(entry.path());
            }
        }

        std::cout << grammars_dir.parent_path() / "outputs" << std::endl;

    } catch (const fs::filesystem_error& error) {
        std::cout << error.what() << std::endl;
    }

}

void Run::set_grammar(){

    std::string grammar_name = tokens[0], entry_name = tokens[1];

    if(is_grammar(grammar_name)){
        current_spec = specs[grammar_name];
        current_spec->setup_builder(entry_name);

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

        } else if ((current_command == "") && (current_spec != nullptr)){
            fs::path output_path = output_dir / ("output" + current_spec->extension);

            current_spec->builder->ast_to_program(output_path);

        } else if (current_command == "h"){
            help();
            
        } else if ((current_command == "print") && (current_spec != nullptr)){
            current_spec->grammar->print_grammar();
        
        } else if ((current_command == "print_tokens") && (current_spec != nullptr)){
            current_spec->grammar->print_tokens();
        
        } else {
            tokenise(current_command);

            if(tokens.size() == 2){
                set_grammar();
            } else {
                std::cout << current_command << " = " << hash_rule_name(current_command) << "ULL," << std::endl;

            }
        }
    }
}


