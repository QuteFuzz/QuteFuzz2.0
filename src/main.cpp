#include <iostream>
#include "../include/write.h"
#include "../include/run.h"

int main(int argc, char** argv){

    Write write(fs::path("../examples/postal.bnf"), "postal-address");
    write.emit();

    return 0;
}
