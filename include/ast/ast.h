#ifndef AST_H
#define AST_H

#include <optional>
#include <algorithm>
#include <term.h>
#include <grammar.h>
#include <node.h>
#include <context.h>

class Ast{
    public:
        Ast(){}

        ~Ast() = default;

        inline void set_entry(const std::shared_ptr<Rule> _entry){
            entry = _entry;
        }

        inline bool entry_set(){return entry != nullptr;}

        void write_branch(std::shared_ptr<Node> parent, const Term& term);

        std::shared_ptr<Node> get_node_from_term(const std::shared_ptr<Node> parent, const Term& term);

        inline void set_ast_counter(const int& counter){context.set_ast_counter(counter);}

        Result<Node> build();

        inline Dag::Dag& get_dag(){ return dag; }

        inline void render_dag(const fs::path& current_circuit_dir){dag.render_dag(current_circuit_dir);}

        inline int get_dag_score(){return dag.score();}

    protected:

        std::shared_ptr<Rule> entry = nullptr;
        std::shared_ptr<Node> dummy = std::make_shared<Node>("");
        
        Context::Context context;
        Dag::Dag dag;
};

#endif
