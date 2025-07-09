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
        set_possible_qubit_combinations();

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
void Run::print_progress_bar(int current, int n) {
    const int bar_width = 40;
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
    std::optional<int> n;

    while(run){
        std::cout << "> ";

        std::getline(std::cin, current_command);
        tokenise(current_command);

        if(current_command == "quit"){
            run = false;

        } else if (current_command == "h"){
            help();
            
        } else if ((current_command == "print") && (current_spec != nullptr)){
            current_spec->print_grammar();
        
        } else if ((current_command == "print_tokens") && (current_spec != nullptr)){
            current_spec->print_tokens();

        } else if ((current_command == "plot") && (current_spec != nullptr)) {
            Common::plot = !Common::plot;
            std::cout << "Plot mode is now " << (Common::plot ? "enabled" : "disabled") << std::endl;
        
        } else if ((current_command == "verbose") && (current_spec != nullptr)){
            Common::verbose = !Common::verbose;
            std::cout << "Vetbose mode is now " << (Common::verbose ? "enabled" : "disabled") << std::endl;

        } else if ((current_command == "run_tests") && (current_spec != nullptr)){
            // Initialize progress bar variables and results file
            int current = 0;
            int total = n.value();
            std::ofstream results_file((output_dir / "results.txt").string());

            for(auto& entry : fs::directory_iterator(output_dir)){

                // check for directories to avoid running the results.txt file
                if(entry.is_directory()){

                    current++;

                    results_file << "Running test: " << entry.path().filename() << std::endl;
                    
                    fs::path program_path = entry.path() / ("circuit.py");
                    std::string command = "python3 " + program_path.string() + (Common::plot ? " --plot" : "");
                    
                    results_file << pipe_from_command(command);

                    print_progress_bar(current, total);                       
                }              
            }

            results_file.close();

            std::cout << std::endl;
            INFO("Test results written to results.txt");

        } else if (tokens.size() == 2){
            set_grammar();

        } else if ((current_spec != nullptr) && (n = safe_stoi(current_command)) && (n.has_value())){ 
            // Clear the outputs and plots directory first before generating new outputs
            remove_all_in_dir(output_dir);
            current_spec->ast_to_program(output_dir, n.value());

        } else {
            std::cout << current_command << " = " << hash_rule_name(current_command) << "ULL," << std::endl;
        
        }
    }
}




