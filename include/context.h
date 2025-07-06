#ifndef CONTEXT_H
#define CONTEXT_H

#include "variables.h"
#include "node.h"
#include "graph.h"

namespace Context {

    enum Level {
        L_QUBIT = 1,
        L_GATE_APPLICATION = 2,
        L_BLOCK = 4,
        L_PROGRAM = 8,
    };

    struct Context {

        public:
            Context(){}

            void set_circuits_dir(fs::path dir){
                circuit_dir = dir;
            }

            void reset(){
                level = L_PROGRAM;

                variables.clear();

                qreg_to_use = DEFAULT_QREG;
                qubit_to_use = DEFAULT_QUBIT;
                subroutines_node = nullptr;
                subroutine_counter = 0;
                circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;

                qig = nullptr;
                best_entanglement = std::nullopt;

                num_external_qubits = Common::MIN_QUBITS;
            }

            void gate_application_reset(){
                best_entanglement = std::nullopt;
                get_current_variables()->qubit_flag_reset();
            }

            void set_variables_ptr(){
                if(variables.find(circuit_name) == variables.end()){
                    std::shared_ptr<Variables> ptr = std::make_shared<Variables>(circuit_name);
                    variables[circuit_name] = ptr;
                }
            }

            std::shared_ptr<Variables> get_current_variables() const {
                return variables.at(circuit_name);
            }

            bool can_apply_subroutines(){
                std::shared_ptr<Variables> current_variables = get_current_variables();
                
                for(const auto& pair : variables){
                    if((pair.first != Common::TOP_LEVEL_CIRCUIT_NAME) && (pair.first != circuit_name) && (current_variables->num_qubits() >= pair.second->num_qubits())){
                        return true;
                    }
                }

                return false;
            }

            std::shared_ptr<Variables> get_random_block(){            
                std::shared_ptr<Variables> current_variables = get_current_variables();

                std::unordered_map<std::string, std::shared_ptr<Variables>>::iterator it = variables.begin();
                std::advance(it, random_int(variables.size() - 1));
                std::shared_ptr<Variables> ret = it->second;

                while(ret->owned_by(Common::TOP_LEVEL_CIRCUIT_NAME) || ret->owned_by(circuit_name) || (ret->num_qubits() > current_variables->num_qubits())){                
                    it = variables.begin();
                    std::advance(it, random_int(variables.size() - 1));
                    ret = it->second;
                }

                return ret;
            }

            int get_max_defined_qubits(){
                int res = Common::MIN_QUBITS;

                for(const auto& pair : variables){
                    res = std::max(res, (int)pair.second->num_qubits());
                }

                return res;
            }

            int setup_qregs(){
                return get_current_variables()->setup_qregs(num_external_qubits);
            }

            int setup_qubits(){
                return get_current_variables()->setup_qubits(num_external_qubits);
            }

            void set_qreg(){
                qreg_to_use = get_current_variables()->get_next_qreg();
            }

            void set_qreg_name(std::shared_ptr<Node> node){
                node->add_child(std::make_shared<Node>(qreg_to_use->get_name()));
            }

            void set_qreg_size(std::shared_ptr<Node> node){
                node->add_child(std::make_shared<Node>(qreg_to_use->get_size_as_string()));
            }

            bool set_qubit(bool singular = false){
                qubit_to_use = singular ? 
                    get_current_variables()->get_random_singular_qubit() :
                    get_current_variables()->get_random_qubit(best_entanglement);

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

            int setup_circuit_block(){
                int num_statements_in_body;
                Qreg::count = 0;

                bool under_subroutines_node = (subroutines_node != nullptr) && (subroutines_node->build_state() == NB_READY);

                if(under_subroutines_node){
                    circuit_name = "sub"+std::to_string(subroutine_counter++);
                    num_statements_in_body = std::min(5, WILDCARD_MAX);
                    num_external_qubits = random_int(Common::MAX_QUBITS, Common::MIN_QUBITS);

                } else {
                    circuit_name = Common::TOP_LEVEL_CIRCUIT_NAME;
                    num_statements_in_body = std::min(5, WILDCARD_MAX);
                    num_external_qubits = random_int(Common::MAX_QUBITS, get_max_defined_qubits());

                    subroutine_counter = 0;
                }

                set_variables_ptr();

                return num_statements_in_body;
            }

            void maybe_render_qig(){
                if(qig != nullptr){
                    fs::path img_path = circuit_dir / (circuit_name + ".png");
                    qig->render_graph(img_path, get_current_variables());
                }
            }

            void set_qig(){
                qig = std::make_unique<Graph>(get_current_variables()->total_num_qubits());
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
                stream << "               VARIABLES                    " << std::endl;
                stream << "============================================" << std::endl;

                for(const auto& pair : context.variables){
                    stream << *pair.second << std::endl;
                }

                stream << "============================================" << std::endl;
                stream << "                QREG_TO_USE                 " << std::endl;
                stream << "============================================" << std::endl;

                stream << *context.qreg_to_use << std::endl;

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

            Level level = L_PROGRAM;

            std::unordered_map<std::string, std::shared_ptr<Variables>> variables;

            std::shared_ptr<Qreg> qreg_to_use = DEFAULT_QREG;
            std::shared_ptr<Qubit> qubit_to_use = DEFAULT_QUBIT;
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
