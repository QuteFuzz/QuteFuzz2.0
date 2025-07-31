#ifndef QUBIT_H
#define QUBIT_H

#include <gate.h>
#include <dag.h>
#include <register_qubit.h>
#include <singular_qubit.h>

namespace Qubit {

    enum Type {
        REGISTER_EXTERNAL,
        REGISTER_INTERNAL,
        SINGULAR_EXTERNAL,
        SINGULAR_INTERNAL
    };

    class Qubit : public Node {
        public:

            /// @brief Dummy qubit
            Qubit() :
                Node("qubit", hash_rule_name("qubit")),
                value(Singular_qubit()),
                type(SINGULAR_EXTERNAL)
            {}

            Qubit(Register_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? REGISTER_EXTERNAL : REGISTER_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::register_qubit, 1);
            }

            Qubit(Singular_qubit qubit, bool external) :
                Node("qubit", hash_rule_name("qubit")),
                value(qubit),
                type(external ? SINGULAR_EXTERNAL : SINGULAR_INTERNAL)
            {
                constraint = std::make_optional<Node_constraint>(Common::singular_qubit, 1);
            }

            Type get_type(){
                return type;
            }

            void reset(){
                std::visit([](auto&& val){
                    val.reset();
                }, value);
            }

            bool is_used(){
                return std::visit([](auto&& val) -> bool {
                    return val.is_used();
                }, value);
            }

            void set_used(){
                std::visit([](auto&& val){
                    val.set_used();
                }, value);
            }

            inline std::shared_ptr<Variable> get_name() const {
                return std::visit([](auto&& val) -> std::shared_ptr<Variable> {
                    return val.get_name();
                }, value);
            }

            inline bool is_external() const {
                return ((type == REGISTER_EXTERNAL) || (type == SINGULAR_EXTERNAL));
            }

            inline bool is_register_def() const {
                return ((type == REGISTER_EXTERNAL) || (type == REGISTER_INTERNAL));
            }

            inline std::shared_ptr<Integer> get_index() const {
                if(is_register_def()){
                    return std::get<Register_qubit>(value).get_index();
                }

                ERROR("Singular qubits do not have indices!");

                return std::make_shared<Integer>();
            }

            inline std::string resolved_name() const {

                if(is_register_def()){
                    return get_name()->get_string() + "_" + get_index()->get_string();
                } else {
                    return get_name()->get_string();
                }

            }
            
            void extend_flow_path(const std::shared_ptr<Gate> gate, size_t current_port){
            
                Dag::Edge edge;
                size_t path_length = flow_path.size();

                if(path_length == 0){
                    // qubit being operated on for the first time
                    edge = Dag::Edge(0, current_port, gate);

                } else {
                    edge = Dag::Edge(flow_path.back().get_dest_port(), current_port, gate);
                }
                
                flow_path.push_back(edge);
            }

            void extend_dot_string(std::ostringstream& ss) const {
                size_t flow_path_length = flow_path.size();
                std::string input_node = "Input_" + resolved_name();
                std::string output_node = "Output_" + resolved_name();  

                ss << input_node << "[color=\"" << flow_path_colour << "\"];" << std::endl;
                ss << output_node << "[color=\"" << flow_path_colour << "\"];" << std::endl;

                if(flow_path_length){

                    ss << input_node;

                    for(size_t i = 0; i < flow_path_length; i++){
                        if(i != 0) {
                            ss << flow_path[i - 1].get_op_name();
                        }
                        
                        ss << flow_path[i] << ", color=\"" << flow_path_colour << "\"];" << std::endl;
                    }

                    Dag::Edge last_path = flow_path[flow_path_length-1]; 

                    ss << last_path.get_op_name() << "-> " << output_node << 
                    "[label=\"" << std::to_string(last_path.get_dest_port()) << ",0\", color=\"" << flow_path_colour << "\"];" << std::endl;

                } else {
                    ss << input_node << "-> " << output_node << ";" << std::endl;

                }
            }
            
        private:
            std::variant<Register_qubit, Singular_qubit> value;
            Type type;

            std::vector<Dag::Edge> flow_path;
            std::string flow_path_colour = random_hex_colour();
    };

}

#endif
