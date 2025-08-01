#ifndef DAG_H
#define DAG_H

#include <gate.h>

namespace Dag {

    class Edge {

        public:
            Edge(){}

            Edge(size_t _src_port, size_t _dest_port, std::shared_ptr<Gate> _op): 
                src_port(_src_port),
                dest_port(_dest_port),
                op(_op)
            {}

            size_t get_dest_port(){
                return dest_port;
            }

            std::string get_op_name() const {
                return op->get_string() + "_" + op->get_id_as_str();
            }

            friend std::ostream& operator<<(std::ostream& stream, const Edge& edge) {
                stream << " -> " << edge.get_op_name() << "[label=\"" << std::to_string(edge.src_port) << ", " << std::to_string(edge.dest_port) << "\"";
                return stream;
            }

        private:
            size_t src_port;
            size_t dest_port;
            std::shared_ptr<Gate> op;
    };

};




#endif
