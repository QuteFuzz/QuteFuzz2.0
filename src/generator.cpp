#include <generator.h>

void Generator::setup_builder(const std::string entry_name, bool is_cross_qss){
    if(grammar->is_rule(entry_name)){
        if (is_cross_qss) {
            cross_qss_builder->set_entry(cross_qss_grammar->get_rule_pointer(entry_name));
        } else {
            builder->set_entry(grammar->get_rule_pointer(entry_name));
        }

    } else if(builder->entry_set() || cross_qss_builder->entry_set()){
        WARNING("Rule " + entry_name + " is not defined for grammar " + grammar->get_name() + " or " + cross_qss_grammar->get_name() + ". Will use previous entry instead");

    } else {
        ERROR("Rule " + entry_name + " is not defined for grammar " + grammar->get_name());
    }
}

void Generator::ast_to_program(fs::path output_dir, int build_counter, std::optional<Genome> genome, bool is_cross_qss){

    fs::path current_circuit_dir =  output_dir / ("circuit" + std::to_string(build_counter));
    fs::create_directory(current_circuit_dir);

    if (is_cross_qss) {
        cross_qss_builder->set_ast_counter(build_counter);
    } else {
        builder->set_ast_counter(build_counter);
    }

    std::optional<Node_constraint> gateset;
    if (Common::swarm_testing) {
        gateset = get_swarm_testing_gateset();
    } else {
        gateset = std::nullopt;
    }

    Result<Node> maybe_ast_root = (is_cross_qss ? cross_qss_builder : builder)->build(genome, gateset);

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();

        std::string circuit_name = is_cross_qss ? "circuit_cross_qss" : "circuit";
        fs::path program_path = current_circuit_dir / (circuit_name + ".py");
        std::ofstream stream(program_path.string());

        // render dag (main block)
        if (Common::render_dags) {
            (is_cross_qss ? cross_qss_builder : builder)->render_dag(current_circuit_dir);
            (is_cross_qss ? cross_qss_builder : builder)->render_subroutine_dags(current_circuit_dir);
        }

        int dag_score;

        if(genome.has_value()){
            dag_score = genome.value().dag_score;
        } else {
            dag_score = (is_cross_qss ? cross_qss_builder : builder)->genome().dag_score;
        }

        INFO("Dag score: " + std::to_string(dag_score));

        // write program
        stream << ast_root << std::endl;
        INFO("Program written to " + YELLOW(program_path.string()));
        
    } else {
        ERROR(maybe_ast_root.get_error());
    }
}

/// @brief It's possible that 2 parents that were picked before will be picked again, that's fine
/// @return 
std::pair<Genome&, Genome&> Generator::pick_parents(){

    std::vector<size_t> indices(population.size());
    std::iota(indices.begin(), indices.end(), 0);

    auto weighted_pick = [&](){
        std::vector<double> weights;

        for(size_t& i : indices){
            weights.push_back(static_cast<double>(population[i].dag_score));
        }

        std::discrete_distribution<> dist(weights.begin(), weights.end());
        return indices[dist(seed())];
    };

    size_t first, second;
    
    first = weighted_pick();

    indices.erase(std::remove(indices.begin(), indices.end(), first), indices.end());

    second = weighted_pick();
    
    return { population[first], population[second] };
}

/// @brief Get all available gate names from grammar rule "gate_name", always exclude "subroutine"
/// @return
std::vector<Common::Rule_hash> Generator::get_available_gate_hashes(){
    std::vector<Common::Rule_hash> gate_name_hashes;

    for (Branch& b : grammar->get_rule_pointer("gate_name")->get_branches()) {
        std::vector<Term> terms = b.get_terms();
        for (Term& t : terms) {
            if (t.get_string() != "subroutine") {
                gate_name_hashes.push_back(Common::Rule_hash(t.get_hash()));
            }
        }
    }

    /*
        I've kept this separate from get_swarm_testing_gateset in case we want to use it for something else later
    */

    return gate_name_hashes;
}

Node_constraint Generator::get_swarm_testing_gateset(){
    std::vector<Common::Rule_hash> gate_name_hashes = get_available_gate_hashes();

    size_t n_gates = std::min((size_t)Common::SWARM_TESTING_GATESET_SIZE, gate_name_hashes.size()-1);
    std::vector<Common::Rule_hash> selected_hashes(n_gates);
    
    #ifdef DEBUG
    if (n_gates == gate_name_hashes.size()) {
        WARNING("Requested swarm testing gateset size is larger than or equal to available gates");
    }
    #endif

    // Sample directly from gate_name_hashes into selected_hashes
    std::sample(gate_name_hashes.begin(), gate_name_hashes.end(), 
                selected_hashes.begin(), n_gates, seed());

    /*
        Most number of gates that Common::SWARM_TESTING_GATESET_SIZE can be is the total number of gates - 1 (excluding subroutine)
        This is to ensure that there is always at least one gate not in the gateset
    */

    std::vector<unsigned int> selected_occurances(n_gates, 0);
    return Node_constraint(selected_hashes, selected_occurances);
}

Dag::Dag Generator::crossover(const Dag::Dag& dag1, const Dag::Dag& dag2){
    Dag::Dag child;

    child.make_dag(dag1.get_qubits(), dag1.get_bits(), "child");

    /*
        I just thought I'd name it something. A nameless child is a sad child. 
        On a serious note, I can make the default name empty if needed
    */

    UNUSED(dag2);

    return child;
}

/// @brief Use genetic algorithm to maximize DAG score, producing final set of circuits that maximise this score
/// @param output_dir 
/// @param population_size 
void Generator::run_genetic(fs::path output_dir, int population_size){

    if(!population_size) return;

    /*
        Fill initial DAG population
    */
    population.clear();

    for(int i = 0; i < population_size; i++){

        std::optional<Node_constraint> gateset;

        if (Common::swarm_testing) {
            gateset = get_swarm_testing_gateset();
        } else {
            gateset = std::nullopt;
        }
        Result<Node> maybe_root = builder->build(std::nullopt, gateset);

        if(maybe_root.is_ok()){
            population.push_back(builder->genome());
        }
    }

    INFO(YELLOW("Initial set of " + std::to_string(population_size) + " dag(s) generated"));

    /*
        Run genetic algorithm
    */
    for(int epoch = 0; epoch < n_epochs; epoch++){

        INFO("Epoch " + std::to_string(epoch));

        // sort population by descending order of dag score
        std::sort(population.begin(), population.end(), [](Genome& a, Genome& b) {
            return a.dag_score > b.dag_score;
        });

        /*
            Prepare for new epoch
        */
        std::vector<Genome> new_pop;

        for(int j = 0; j < population_size; j++){

            // top performers go to next epoch as is, already in population in correct order due to sort 
            if(j > elitism * population_size){
                std::pair<Genome&, Genome&> parents = pick_parents();

                Genome child{.dag = std::move(crossover(parents.first.dag, parents.second.dag)), .subroutine_dags = {}, .dag_score = 0};
                child.dag_score = child.dag.score();

                new_pop.push_back(child);
            } else {
                new_pop.push_back(population[j]);
            }
        }

        population = std::move(new_pop);
    }

    INFO(YELLOW("Run genetic algorithm for " + std::to_string(n_epochs) + " epochs"));

    /*
        Generate programs from final DAGs
    */
    for(int build_counter = 0; build_counter < (int)population.size(); build_counter++){
        ast_to_program(output_dir, build_counter, std::make_optional<Genome>(population[build_counter]));
    }

    INFO(YELLOW("Generated " + std::to_string(population_size) + " program(s)"));

}

void Generator::generate_cross_qss(fs::path output_dir, int n_programs) {
    
    /*
    Reset population
    */
    population.clear();

    /*
        Fill initial DAG population
    */
    for(int i = 0; i < n_programs; i++){
        
        std::optional<Node_constraint> gateset;
        if (Common::swarm_testing) {
            gateset = get_swarm_testing_gateset();
        } else {
            gateset = std::nullopt;
        }
        Result<Node> maybe_root = builder->build(std::nullopt, gateset);

        if(maybe_root.is_ok()){
            population.push_back(builder->genome());
        }
    }

    INFO(YELLOW("Generated " + std::to_string(population.size()) + " program(s)"));

    /*
        Generate semantically equivalent programs in base language first
    */
    for(int build_counter = 0; build_counter < (int)population.size(); build_counter++){
        ast_to_program(output_dir, build_counter, std::make_optional<Genome>(population[build_counter]));
    }

    INFO("Generating cross-QSS programs...");
    /*
        Set builder to new grammar using same entry point and generate programs
    */
   for(int build_counter = 0; build_counter < (int)population.size(); build_counter++){
        ast_to_program(output_dir, build_counter, std::make_optional<Genome>(population[build_counter]), true);
    }

}

void Generator::generate_random_programs(fs::path output_dir, int n_programs){
    for(int build_counter = 0; build_counter < n_programs; build_counter++){
        ast_to_program(output_dir, build_counter, std::nullopt);
    }
}