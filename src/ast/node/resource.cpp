#include <resource.h>
#include <iostream>
#include <sstream>
#include <string>

std::string Resource::Resource::resolved_name() const {

    if(is_register_def()){
        return get_name()->get_string() + "[" + get_index()->get_string() + "]";
    } else {
        return get_name()->get_string();
    }

}

