#include "../include/run.h"

Run::Run(const std::string& _grammars_dir) : grammars_dir(_grammars_dir) {
    // build all grammars
    try{

        if(fs::exists(grammars_dir) && fs::is_directory(grammars_dir)){
            for(auto& entry : fs::directory_iterator(grammars_dir)){

                if(entry.is_regular_file() && (entry.path().extension() == ".bnf")){
                    Grammar grammar(entry);
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

void Run::run(){
    write.set_grammar(grammars["postal"], "postal-address");
    write.emit();

}


