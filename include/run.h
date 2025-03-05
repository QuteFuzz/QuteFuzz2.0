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

        void set_grammar(const std::string& command){

            std::stringstream ss(command);
            std::string token, grammar = "math", entry = "expr";

            while(std::getline(ss, token, ' ')){
                if(is_grammar(token)) grammar = token;
            }

        }

        int get_input(){
            std::cin >> current_command;
        
            return 1;
        }

        void run();

    private:
        fs::path grammars_dir;
        std::unordered_map<std::string, Grammar> grammars;
        Write write;

        std::string current_command;

};

#endif

