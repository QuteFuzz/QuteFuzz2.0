#ifndef CONTEXT_H
#define CONTEXT_H

#include "block.h"
#include "node.h"
#include "../qig/graph.h"

namespace Context {

    enum Level {
        QUBIT = 1,
        GATE_APPLICATION = 2,
        BLOCK = 4,
        PROGRAM = 8,
    };

    struct Context {

        public:
            Context(){}

            void set_circuits_dir(fs::path dir){
                circuit_dir = dir;
            }

            void reset(){
                level = PROGRAM;

                blocks.clear();

                qig = nullptr;
                best_entanglement = std::nullopt;
                num_external_qubits = Common::MIN_QUBITS;
            }

            void gate_application_reset(){
                best_entanglement = std::nullopt;
                get_current_block()->qubit_flag_reset();
            }

            void set_block_ptr(){
                if(blocks.find(circuit_name) == blocks.end()){
                    std::shared_ptr<Block> ptr = std::make_shared<Block>(circuit_name);
                    blocks[circuit_name] = ptr;
                }
            }

            std::shared_ptr<Block> get_current_block() const {
                return blocks.at(circuit_name);
            }

            bool can_apply_subroutines(){
                std::shared_ptr<Block> current_block = get_current_block();
                
                for(const auto& pair : blocks){
                    if((pair.first != Common::TOP_LEVEL_CIRCUIT_NAME) && (pair.first != circuit_name) && (current_block->num_qubits() >= pair.second->num_qubits())){
                        return true;
                    }
                }

                return false;
            }

            std::shared_ptr<Block> get_random_block(){            
                std::shared_ptr<Block> current_block = get_current_block();

                std::unordered_map<std::string, std::shared_ptr<Block>>::iterator it = blocks.begin();
                std::advance(it, random_int(blocks.size() - 1));
                std::shared_ptr<Block> ret = it->second;

                #ifdef DEBUG
                INFO("Getting random block");
                #endif

                while(ret->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || ret->owned_by(circuit_name) || (ret->num_qubits() > current_block->num_qubits())){                
                    it = blocks.begin();
                    std::advance(it, random_int(blocks.size() - 1));
                    ret = it->second;
                }

                return ret;
            }

            int get_max_defined_qubits(){
                int res = Common::MIN_QUBITS;

                for(const auto& pair : blocks){
                    res = std::max(res, (int)pair.second->num_qubits());
                }

                return res;
            }

            int setup_qubit_defs(){
                return get_current_block()->setup_qubit_defs(num_external_qubits);
            }

            Qubit_def::Type set_qubit_def(){
                qubit_def_to_write = get_current_block()->get_next_qubit_def();
                return qubit_def_to_write->get_type(); 
            }

            void set_qreg_name(std::shared_ptr<Node> node){
                if(qubit_def_to_write->get_type() == Qubit_def::REGISTER){
                    node->add_child(std::make_shared<Node>(qubit_def_to_write->get_register_def()->get_name()));
                
                } else if (qubit_def_to_write->get_type() == Qubit_def::SINGULAR){
                    node->add_child(std::make_shared<Node>(qubit_def_to_write->get_singular_def()->get_name()));

                }
            }

            void set_qreg_size(std::shared_ptr<Node> node){
                if(qubit_def_to_write->get_type() == Qubit_def::REGISTER){
                    node->add_child(std::make_shared<Node>(qubit_def_to_write->get_register_def()->get_size_as_string()));
                }
            }

            bool set_qubit(){
                qubit_to_use = get_current_block()->get_random_qubit(best_entanglement);
                return qubit_to_use->is_from_register();
            }

            void set_qubit_name(std::shared_ptr<Node> node){
                node->add_child(std::make_shared<Node>(qubit_to_use->get_name()));
            }

            void set_qubit_index(std::shared_ptr<Node> node){
                node->add_child(std::make_shared<Node>(qubit_to_use->get_index_as_string()));
            }

            void set_circuit_name(std::shared_ptr<Node> node){
                node->add_child(std::make_shared<Node>(circuit_name));
            }

            bool under_subroutines_node(){
                return (subroutines_node != nullptr) && (subroutines_node->build_state() == NB_READY);
            }

            int setup_block(){
                int num_statements_in_body;

                Qubit_def::Qreg::count = 0;
                Qubit_def::Qubit::count = 0;
            
                if(under_subroutines_node()){
                    circuit_name = "sub"+std::to_string(subroutine_counter++);
                    num_statements_in_body = std::min(5, WILDCARD_MAX);
                    num_external_qubits = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);

                } else {
                    circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;
                    num_statements_in_body = std::min(10, WILDCARD_MAX);
                    num_external_qubits = random_int(Common::MAX_QUBITS, get_max_defined_qubits());

                    subroutine_counter = 0;
                }

                set_block_ptr();

                return num_statements_in_body;
            }

            void maybe_render_qig(){
                if(qig != nullptr){
                    fs::path img_path = circuit_dir / (circuit_name + ".png");
                    qig->render_graph(img_path, get_current_block());
                }
            }

            void set_qig(){
                qig = std::make_unique<Graph>(get_current_block()->total_num_qubits());
            }

            void set_subroutines_node(std::shared_ptr<Node> node){
                subroutines_node = node;
            }

            void set_best_entanglement(int num_qubits){
                best_entanglement = 
                    std::make_optional<std::vector<int>>(qig->get_best_entanglement(num_qubits));
            }

            friend std::ostream& operator<<(std::ostream& stream, const Context& context){

                stream << "Current circuit: " << context.circuit_name << std::endl;

                stream << "============================================" << std::endl;
                stream << "               BLOCKS                    " << std::endl;
                stream << "============================================" << std::endl;

                for(const auto& pair : context.blocks){
                    stream << *pair.second << std::endl;
                }

                stream << "============================================" << std::endl;
                stream << "                QUBIT_DEF                   " << std::endl;
                stream << "============================================" << std::endl;

                stream << *context.qubit_def_to_write << std::endl;

                stream << "============================================" << std::endl;
                stream << "                QUBIT_TO_USE                " << std::endl;
                stream << "============================================" << std::endl;

                stream << *context.qubit_to_use << std::endl;

                if(context.subroutines_node != nullptr){

                    stream << "============================================" << std::endl;
                    stream << "                 SUBROUTINES                " << std::endl;
                    stream << "============================================" << std::endl;

                    stream << "At: " << context.subroutines_node << std::endl;
                    stream << "Counter: " << context.subroutine_counter << std::endl;
                }

                if(context.best_entanglement.has_value()){

                    stream << "============================================" << std::endl;
                    stream << "              BEST_ENTANGLEMENT             " << std::endl;
                    stream << "============================================" << std::endl;

                    for(size_t i = 0; i < context.best_entanglement.value().size(); i++){
                        stream << context.best_entanglement.value()[i] << " ";
                    }

                    stream << std::endl;
                }

                return stream;
            }

        private:

            Level level = PROGRAM;

            std::unordered_map<std::string, std::shared_ptr<Block>> blocks;

            std::shared_ptr<Qubit_def::Qubit_def> qubit_def_to_write;
            std::shared_ptr<Qubit_def::Qubit> qubit_to_use = Qubit_def::DEFAULT_QUBIT;
            
            std::shared_ptr<Node> subroutines_node = nullptr;
            int subroutine_counter = 0;
            std::string circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;

            std::unique_ptr<Graph> qig = nullptr;
            std::optional<std::vector<int>> best_entanglement = std::nullopt;

            int num_external_qubits = Common::MIN_QUBITS;

            fs::path circuit_dir;

    };

}

#endif
