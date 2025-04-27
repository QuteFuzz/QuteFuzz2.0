#ifndef RUN_H
#define RUN_H

#include "grammar.h"
#include "ast.h"
#include "pytket.h"
#include "qiskit.h"
#include <sstream>


const std::string OUTPUTS_FOLDER_NAME = "outputs";

typedef struct {
    std::shared_ptr<Grammar> grammar;
    std::shared_ptr<Ast> builder;
    std::string extension;

    void setup_builder(const std::string entry_name){
        if(grammar->is_rule(entry_name)){
            builder->set_entry(grammar->get_rule_pointer(entry_name));
        } else {
            std::cout << "Rule " << entry_name << " is not defined for grammar " << grammar->get_name() << std::endl;  
        }
    }

} Program_Spec;

class Run{

    public:
        Run(const std::string& _grammars_dir);

        bool is_grammar(const std::string& name){
            return specs.find(name) != specs.end();
        }

        void help(){
            std::cout << "-> Type enter to write to a file" << std::endl;
            std::cout << "-> \"grammar_name grammar_entry\" : command to set grammar " << std::endl;
            std::cout << "  These are the known grammar rules: " << std::endl;

            for(const auto& spec : specs){
                std::cout << "  . " << spec.first << ": ";
                spec.second->grammar->print_rules();
                std::cout << std::endl;
            }
        }

        void set_grammar();

        void tokenise(const std::string& command);

        void loop();

    private:
        fs::path grammars_dir;
        std::unordered_map<std::string, std::shared_ptr<Program_Spec>> specs;
        std::shared_ptr<Program_Spec> current_spec = nullptr;

        std::vector<std::string> tokens;

        bool run = true;

        fs::path output_dir;
};

#endif

