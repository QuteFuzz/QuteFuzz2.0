#ifndef RUN_H
#define RUN_H

#include <grammar.h>
#include <sstream>
#include <set>
#include <iomanip>

#include <ast.h>

const std::string OUTPUTS_FOLDER_NAME = "outputs";

struct Program_Spec {

    public:

        Program_Spec(Grammar& _grammar): 
            grammar(std::make_shared<Grammar>(_grammar)),        
            extension(".py")
        {
            builder = std::make_shared<Ast>();   
        }

        void setup_builder(const std::string entry_name);

        friend std::ostream& operator<<(std::ostream& stream, Program_Spec spec){
            stream << "  . " << spec.grammar->get_name() << ": ";
            spec.grammar->print_rules();

            return stream;
        }

        void print_grammar(){
            grammar->print_grammar();
        }

        void print_tokens(){
            grammar->print_tokens();
        }

        void ast_to_program(fs::path output_dir, int num_programs);

    private:
        std::shared_ptr<Grammar> grammar;
        std::shared_ptr<Ast> builder;
        std::string extension;

};

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
                std::cout << spec.second << std::endl;
            }
        }

        void set_grammar();

        void tokenise(const std::string& command);

        void remove_all_in_dir(const fs::path& dir);

        void print_progress_bar(int current, int n);

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

