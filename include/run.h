#ifndef RUN_H
#define RUN_H

#include "write.h"
#include "grammar.h"
#include <sstream>

class Run{

    public:
        Run(const std::string& _grammars_dir);

        bool is_grammar(const std::string& name){
            return grammars.find(name) != grammars.end();
        }

        void help(){
            std::cout << "-> Type enter to write to a file" << std::endl;
            std::cout << "-> \"grammar_name grammar_entry\" : command to set grammar " << std::endl;
            std::cout << "  These are the known grammar rules: " << std::endl;

            for(const auto& g : grammars){
                std::cout << "  . " << g.first << ": ";
                g.second.print_rules();
                std::cout << std::endl;
            }
        }

        void set_grammar();

        void tokenise(const std::string& command);

        void loop();

    private:
        fs::path grammars_dir;
        std::unordered_map<std::string, Grammar> grammars;
        Grammar* current_grammar = NULL;
        Write write;

        std::vector<std::string> tokens;

        bool run = true;

};

#endif

