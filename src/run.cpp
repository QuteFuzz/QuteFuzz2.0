#include <run.h>
#include <ast.h>


Run::Run(const std::string& _grammars_dir) : grammars_dir(_grammars_dir) {
    // build all grammars
    try{

        if(fs::exists(grammars_dir) && fs::is_directory(grammars_dir)){
            Grammar commons_grammar;

            /*
                find tokens grammar and parse that first
            */
            for(auto& file : fs::directory_iterator(grammars_dir)){

                if(file.is_regular_file() && (file.path().stem() == Common::TOKENS_GRAMMAR_NAME)){
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

                if(file.is_regular_file() && (file.path().extension() == ".bnf") && (file.path().stem() != Common::TOKENS_GRAMMAR_NAME)){

                    Grammar grammar(file);
                    grammar += commons_grammar;
                    grammar.build_grammar();

                    std::string name = grammar.get_name();
                    std::cout << "Built " << name << std::endl;
                    generators[name] = std::make_shared<Generator>(grammar);
                    
                }

            }

            /* 
                prepare directories
            */
            output_dir = grammars_dir.parent_path() / Common::OUTPUTS_FOLDER_NAME;
            
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
        current_generator = generators[grammar_name];
        current_generator->setup_builder(entry_name);

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

void Run::help(){
    std::cout << "-> Type enter to write to a file" << std::endl;
    std::cout << "-> \"grammar_name grammar_entry\" : command to set grammar " << std::endl;
    std::cout << "  These are the known grammar rules: " << std::endl;

    for(const auto& generator : generators){
        std::cout << generator.second << std::endl;
    }
}

void Run::run_tests(){
    int current = 0;
    std::string results_path = (output_dir / "results.txt").string();
    std::ofstream results_file(results_path);

    for(auto& entry : fs::directory_iterator(output_dir)){

        // check for directories to avoid running the results.txt file and interesting_circuits
        if(entry.is_directory() && entry.path().filename() != "interesting_circuits"){

            current++;

            results_file << "Running test: " << entry.path().filename() << std::endl;
            
            fs::path program_path = entry.path() / ("circuit.py");
            std::string command = "python3 " + program_path.string() + (Common::plot ? " --plot" : "") + " 2>&1";
            
            results_file << pipe_from_command(command) << std::endl;

            print_progress_bar(current);                       
        }              
    }

    results_file.close();

    INFO("Test results written to " + results_path);
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

        } else if(current_generator != nullptr){

            if(current_command == "print"){
                current_generator->print_grammar();
            
            } else if (current_command == "print_tokens"){
                current_generator->print_tokens();
            
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

            } else if (current_command == "run_genetic"){
                remove_all_in_dir(output_dir);
                current_generator->run_genetic();

            } else if ((n_programs = safe_stoi(current_command))){
                remove_all_in_dir(output_dir);
                current_generator->ast_to_program(output_dir, n_programs.value_or(0));
            }

        } else {
            std::cout << current_command << " = " << hash_rule_name(current_command) << "ULL," << std::endl;
        
        }
    }
}




