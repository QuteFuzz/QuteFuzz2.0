#include "../include/write.h"

Result<Branch, std::string> Write::pick_branch(std::shared_ptr<Rule> rule){
    float choice = random_float();
    float cummulative = 0.0;
    Result<Branch, std::string> result;

    std::vector<Branch> branches = rule->get_coll();

    if(branches.empty()){
        result.set_ok(Branch());
        return result;
    }
    
    if ((depth_limit <= 0) && rule->get_recursive_flag()){
        for(const Branch& b : branches){    
            if(!b.get_recursive_flag()){
                result.set_ok(b);
                return result;
            }
        }
    }

    for(const Branch& b : branches){
        cummulative += b.get_prob();

        if(choice <= cummulative){
            result.set_ok(b);

            if(b.get_recursive_flag()) depth_limit -= 1;

            return result;
        }
    }

    result.set_error("[ERROR] Cannot pick branch for rule " + rule->get_name() + " !" + "\nChoose prob " + std::to_string(choice));
    return result;
}

void Write::write_branch(std::ofstream& stream, const Result<Branch, std::string>& maybe_branch){

    if(maybe_branch.is_ok()){
        Branch branch = maybe_branch.get_ok();

        std::vector<Node> nodes = branch.get_coll();

        for(const Node& node : branch.get_coll()){
            if (node.is_syntax()){
                std::string syn = node.get_syntax();

                if(syn == "\\n"){
                    stream << "\n";
                } else {
                    stream << node.get_syntax();
                }


            } else {
                std::shared_ptr<Rule> next_rule = node.get_rule();

                write_branch(stream, pick_branch(next_rule)); // randomly pick a branch from next rule to continue from        
            }
        }

    } else {
        std::cout << maybe_branch.get_error() << std::endl;
    }
}

void Write::emit(){

    fs::path output_path = grammar.get_path();
    output_path.replace_extension(".txt");

    std::ofstream stream(output_path.string());

    std::shared_ptr<Rule> entry = grammar.get_rule_pointer(entry_point);

    write_branch(stream, pick_branch(entry)); // pick branch randomly to be written from entry point
}
