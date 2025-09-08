#ifndef GENERATOR_H
#define GENERATOR_H

#include <grammar.h>
#include <ast.h>
#include <genome.h>
#include <dag.h>

struct Generator {

    public:

        Generator(Grammar& _grammar): 
            grammar(std::make_shared<Grammar>(_grammar)),
            builder(std::make_shared<Ast>()),
            cross_qss_builder(std::make_shared<Ast>())
        {}

        void setup_builder(const std::string entry_name, bool is_cross_qss = false);

        friend std::ostream& operator<<(std::ostream& stream, Generator generator){
            stream << "  . " << generator.grammar->get_name() << ": ";
            generator.grammar->print_rules();

            return stream;
        }

        void print_grammar(){
            grammar->print_grammar();
        }

        void print_tokens(){
            grammar->print_tokens();
        }

        Dag::Dag crossover(const Dag::Dag& dag1, const Dag::Dag& dag2);

        std::pair<Genome&, Genome&> pick_parents();

        std::vector<Common::Rule_hash> get_available_gate_hashes();

        Node_constraint get_swarm_testing_gateset();

        std::shared_ptr<Grammar> get_grammar(){
            return grammar;
        }

        void set_cross_qss_grammar(std::shared_ptr<Grammar> _cross_qss_grammar){
            cross_qss_grammar = _cross_qss_grammar;
        }

        void ast_to_program(fs::path output_dir, int build_counter, std::optional<Genome> genome, bool is_cross_qss = false);

        void generate_random_programs(fs::path output_dir, int n_programs);

        void generate_cross_qss(fs::path output_dir, int n_programs);

        void run_genetic(fs::path output_dir, int population_size);


    private:
        std::shared_ptr<Grammar> grammar;
        std::shared_ptr<Grammar> cross_qss_grammar;
        std::shared_ptr<Ast> builder;
        std::shared_ptr<Ast> cross_qss_builder;

        int n_epochs = 100;
        float elitism = 0.2;

        std::vector<Genome> population;

};


#endif

