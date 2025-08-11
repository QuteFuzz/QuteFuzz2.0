#ifndef GENERATOR_H
#define GENERATOR_H

#include <grammar.h>
#include <ast.h>

struct Scored_genome {
    Dag::Dag genome;
    int dag_score;
};

struct Generator {

    public:

        Generator(Grammar& _grammar): 
            grammar(std::make_shared<Grammar>(_grammar)),
            builder(std::make_shared<Ast>()),
            population(population_size)
        {}

        void setup_builder(const std::string entry_name);

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

        std::pair<Scored_genome&, Scored_genome&> pick_parents();

        void ast_to_program(fs::path output_dir, int num_programs);

        void run_genetic();

    private:
        std::shared_ptr<Grammar> grammar;
        std::shared_ptr<Ast> builder;

        size_t population_size = 100;
        size_t n_epochs = 100;
        float elitism = 0.2;

        std::vector<Scored_genome> population;

};


#endif

