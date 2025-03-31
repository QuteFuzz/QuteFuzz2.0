#include "../include/pytket.h"


void Pytket::Pytket::write(fs::path& path) {
    Result<Node, std::string> maybe_ast_root = build(); 

    if(maybe_ast_root.is_ok()){
        Node ast_root = maybe_ast_root.get_ok();

        std::ofstream stream(path.string());

        write_imports(stream) << std::endl;

        write(stream, ast_root);

        std::cout << ast_root << std::endl;
        std::cout << "Written to " << path.string() << std::endl;

    } else {
        std::cout << "[ERROR] " << maybe_ast_root.get_error() << std::endl;
    }
};