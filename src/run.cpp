#include <run.h>
#include <ast.h>


void Program_Spec::setup_builder(const std::string entry_name){
    if(grammar->is_rule(entry_name)){
        builder->set_entry(grammar->get_rule_pointer(entry_name));
    } else {
        std::cout << "Rule " << entry_name << " is not defined for grammar " << grammar->get_name() << std::endl;  
    }
}

void Program_Spec::ast_to_program(fs::path output_dir, int num_programs){
    builder->ast_to_program(output_dir, extension, num_programs);
}

Run::Run(const std::string& _grammars_dir) : grammars_dir(_grammars_dir) {
    // build all grammars
    try{

        if(fs::exists(grammars_dir) && fs::is_directory(grammars_dir)){
            Grammar commons_grammar;

            /*
                find tokens grammar and parse that first
            */
            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().stem() == TOKENS_GRAMMAR_NAME)){
                    Grammar grammar(file);
                    grammar.build_grammar();

                    commons_grammar = grammar;
                    commons_grammar.mark_as_commons_grammar();

                    break;
                }
            }

            /*
                parse all other grammars, appending the tokens grammar to each one
            */
            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().extension() == ".bnf") && (file.path().stem() != TOKENS_GRAMMAR_NAME)){

                    Grammar grammar(file);
                    grammar += commons_grammar;
                    grammar.build_grammar();

                    std::string name = grammar.get_name();
                    std::cout << "Built " << name << std::endl;
                    specs[name] = std::make_shared<Program_Spec>(grammar);
                    
                }

            }

            /* 
                prepare directories
            */
            output_dir = grammars_dir.parent_path() / OUTPUTS_FOLDER_NAME;
            
            if(!fs::exists(output_dir)){
                fs::create_directory(output_dir);
            } else {
                remove_all_in_dir(output_dir);
            }

        }

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

void Run::remove_all_in_dir(const fs::path& dir){
    if(fs::exists(dir) && fs::is_directory(dir)){
        for(const auto& entry : fs::directory_iterator(dir)){
            fs::remove_all(entry.path());
        }
    }
}

// Prints a progress bar to the terminal
void Run::print_progress_bar(unsigned int current) {
    const int bar_width = 40;
    unsigned int n = n_programs.value_or(0);

    float progress = (n > 0) ? float(current) / n : 0.0f;
    int pos = static_cast<int>(bar_width * progress);

    std::cout << "[";

    for (int i = 0; i < bar_width; ++i) {
        if (i < pos) std::cout << "#";
        else std::cout << "-";
    }

    std::cout << "] " << std::setw(3) << int(progress * 100.0) << "%  (" << current << "/" << n << ")\r";
    std::cout.flush();

    if (current == n) std::cout << std::endl;
}

void Run::loop(){

    std::string current_command;

    while(true){
        std::cout << "> ";

        std::getline(std::cin, current_command);
        tokenise(current_command);

        if(tokens.size() == 2){
            set_grammar();

        } else if(current_command == "h"){
            help();

        } else if (current_command == "quit"){
            break;

        } else if(current_spec != nullptr){

            if(current_command == "print"){
                current_spec->print_grammar();
            
            } else if (current_command == "print_tokens"){
                current_spec->print_tokens();
            
            } else if (current_command == "plot"){
                Common::plot = !Common::plot;
                INFO("Plot mode is now " + (Common::plot ? "enabled" : "disabled"));
            
            } else if (current_command == "verbose"){
                Common::verbose = !Common::verbose;
                INFO("Verbose mode is now " + (Common::verbose ? "enabled" : "disabled"));

            } else if (current_command == "render_dags"){
                Common::render_dags = !Common::render_dags;
                INFO("DAG render " + (Common::render_dags ? "enabled" : "disabled"));

            } else if (current_command == "run_tests"){
                run_tests();

            } else if ((n_programs = safe_stoi(current_command))){
                remove_all_in_dir(output_dir);
                current_spec->ast_to_program(output_dir, n_programs.value_or(0));
            }

        } else {
            std::cout << current_command << " = " << hash_rule_name(current_command) << "ULL," << std::endl;
        
        }
    }
}




