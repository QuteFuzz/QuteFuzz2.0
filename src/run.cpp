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
        // prepare plots directory
        plots_dir = grammars_dir.parent_path() / PLOTS_FOLDER_NAME;
        
        if(!fs::exists(output_dir)){
            fs::create_directory(output_dir);
        } else {
            remove_all_in_dir(output_dir);
        }

        if(!fs::exists(plots_dir)){
            fs::create_directory(plots_dir);
        } else {
            remove_all_in_dir(plots_dir);
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

void Run::loop(){

    std::string current_command;
    std::optional<int> n;
    bool plot = false;

    while(run){
        std::cout << "> ";

        std::getline(std::cin, current_command);
        tokenise(current_command);

        if(current_command == "quit"){
            run = false;

        } else if (current_command == "h"){
            help();
            
        } else if ((current_command == "print") && (current_spec != nullptr)){
            current_spec->grammar->print_grammar();
        
        } else if ((current_command == "print_tokens") && (current_spec != nullptr)){
            current_spec->grammar->print_tokens();

        } else if ((current_command == "plot") && (current_spec != nullptr)) {
            plot = !plot;
            std::cout << "Plot mode is now " << (plot ? "enabled" : "disabled") << std::endl;
        
        } else if ((current_command == "run_tests") && (current_spec != nullptr)){
            std::set<fs::path> py_files;
            for(auto& entry : fs::directory_iterator(output_dir)){
                if(entry.is_regular_file() && (entry.path().extension() == ".py")){
                    py_files.insert(entry.path());
                }
            }
            // Run all python files in the outputs directory
            for(const auto& path : py_files){
                std::cout << "Running test: " << path.filename() << std::endl;
                std::string command = "python3 " + path.string() + (plot ? " --plot" : "");
                int result = system(command.c_str());
                if(result != 0){
                    std::cout << "Test failed with exit code: " << result << std::endl;
                }
            }
        } else if (tokens.size() == 2){
            set_grammar();

        } else if ((current_spec != nullptr) && (n = safe_stoi(current_command)) && (n.has_value())){ 
            current_spec->builder->num_circuits = 0;

            // Clear the outputs and plots directory first before generating new outputs
            remove_all_in_dir(output_dir);
            remove_all_in_dir(plots_dir);

            for(int i = 0; i < n.value(); i++){
                fs::path output_path = output_dir / ("output" + std::to_string(i+1) + current_spec->extension);
                current_spec->builder->ast_to_program(output_path);
            }  

        } else {
            std::cout << current_command << " = " << hash_rule_name(current_command) << "ULL," << std::endl;
        
        }
    }
}


