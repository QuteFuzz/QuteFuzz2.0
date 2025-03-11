#ifndef WRITE_H
#define WRITE_H

#include "../include/grammar.h"

class Write{

    public:
        Write() : gen(rd()), float_dist(0.0, 1.0) {}

        Write(const fs::path& filename, const std::string& _entry_point) : grammar(filename), entry_point(_entry_point), gen(rd()), float_dist(0.0, 1.0) {
            grammar.build_grammar();
            grammar.print_grammar();
        }

        inline float random_float(){
            return float_dist(gen);
        }

        void set_grammar(const Grammar& _grammar, const std::string& _entry_point){
            grammar = _grammar;
            entry_point = _entry_point;
        }

        Result<Branch, std::string> pick_branch(std::shared_ptr<Rule> rule);

        void write_branch(std::ofstream& stream, const Result<Branch, std::string>& maybe_branch);

        void emit();

    private:
        Grammar grammar;

        std::string entry_point;
        int depth_limit = 10;

        std::random_device rd;
        std::mt19937 gen;
        std::uniform_real_distribution<float> float_dist;
};

#endif
