#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <queue>
#include <stack>
#include <memory>
#include <string>
#include <iostream>
#include <variant>
#include <regex>
#include <random>
#include <filesystem>
#include <unordered_map>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <optional>
#include <array>
#include <iomanip>

#define WILDCARD_MAX 20

#define UNUSED(x) (void)(x)

// location annotation
#define ANNOT(x) (std::string("at ") + __FILE__ + "," + std::to_string(__LINE__) + ": " + (x))

// logging
#define ERROR(x) std::cerr << (std::string("[ERROR] ") + ANNOT(x)) << std::endl
#define WARNING(x) std::cout << (std::string("[WARNING] ") + ANNOT(x)) << std::endl
#define INFO(x) std::cout << (std::string("[INFO] ") + x) << std::endl


#define TOKENS_GRAMMAR_NAME "tokens"

using U64 = uint64_t;

namespace fs = std::filesystem;

U64 hash_rule_name(std::string rule_name);

void lower(std::string& str);

int random_int(int max, int min = 0);

float random_float(float max, float min = 0.0);

int get_amount(int from, int resmin, int resmax);

std::optional<int> safe_stoi(const std::string& str);

std::vector<std::vector<int>> n_choose_r(const int n, const int r);

int vector_sum(std::vector<int> in);

int vector_max(std::vector<int> in);

void pipe_to_command(std::string command, std::string write);

std::string pipe_from_command(std::string command);

std::string escape(const std::string& str);

namespace Common {
    constexpr char TOP_LEVEL_CIRCUIT_NAME[] = "main_circuit";
    constexpr int MIN_N_QUBITS_IN_ENTANGLEMENT = 2;
    constexpr int MIN_QUBITS = 3;
    constexpr int MAX_QUBITS = std::max(MIN_QUBITS + 1, (int)(0.5 * WILDCARD_MAX));
    constexpr int MAX_SUBROUTINES = (int)(0.5 * WILDCARD_MAX);

    extern bool plot;
    extern bool verbose; 
    
    enum Rule_hash : U64 {
        // SINGLE QUBIT GATES
        h = 12638197096160295895ULL,
        x = 12638214688346347271ULL,
        y = 12638213588834719060ULL,
        z = 12638216887369603693ULL,
        rz = 638471042196278621ULL,
        rx = 638468843173022199ULL,
        ry = 638467743661393988ULL,
        u1 = 631765120777144307ULL,
        s = 12638206991764949794ULL,
        sdg = 9383135583349391919ULL,
        t = 12638201494206808739ULL,
        tdg = 6262443406867765286ULL,
        phasedxpowgate = 11832025932794728373ULL,

        // TWO QUBIT GATES
        cx = 622136697450792830ULL,
        cy = 622137796962421041ULL,
        cz = 622134498427536408ULL,
        ccx = 17716034042247149281ULL,
        u2 = 631766220288772518ULL,
        cnot = 352634892887392207ULL,
        ch = 622154289636844206ULL,
        crz = 17699985570524998350ULL,
    
        // THREE QUBIT GATES    
        u3 = 631767319800400729ULL,
        cswap = 9091793091986225957ULL,
        toffoli = 15991066036535617246ULL,

        // OTHER GATES
        u = 12638200394695180528ULL,

        // RULE NAMES
        block = 1505885265403634530ULL,
        circuit_name = 5389401364268778602ULL,
        float_list = 12337679196430086138ULL,
        float_literal = 6014115549703600901ULL,
        main_circuit_name = 15359974437464362266ULL,

        body = 14793735007222968981ULL,
        qreg_name = 10722739978486866664ULL,
        qreg_size = 11502232252882731618ULL,
        qubit = 9613145697666387222ULL,
        qubit_op = 7363837753828900628ULL,
        gate_op = 17845938762380861480ULL,
        subroutine_op = 4275855153236832595ULL,
        gate_op_kind = 14929235841933060947ULL,
        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,        
    
        qubit_defs_internal = 15763474746718250229ULL,
        qubit_def_internal = 8367506410210901254ULL,
        register_qubit_def_internal = 7889892016210498366ULL,
        singular_qubit_def_internal = 14695716164594440254ULL,
    
        qubit_defs_external = 12875408231208951867ULL,
        qubit_def_external = 9066873179617854836ULL,
        register_qubit_def_external = 17118825122301876444ULL,
        singular_qubit_def_external = 5477905196976266716ULL,
        
        singular_qubit = 358282167276964286ULL,
        register_qubit = 1224902788537856702ULL,

        qubit_name = 8757953683662904688ULL,
        qubit_index = 6830264791288854081ULL,
        subroutines = 3276487754481867520ULL,
        subroutine = 7419198593375467891ULL,
        circuit_id = 12523072865437646660ULL,

        // Guppy rules
        main_block = 9115425723233342258ULL,
        main_block_def = 6816634947724795910ULL,
        block_args = 8098915444984821122ULL,
        indented_body = 11869103690287558935ULL,
        type = 12075340201627130925ULL,
        parameter_name = 12726161396389809054ULL,
        discard_internal_qubits = 2953622336913522322ULL,
        discard_internal_qubit = 1018654204566407765ULL,
        discard_single_qubit = 11775807085076373088ULL,
        discard_qreg = 13047647025597388553ULL,
        decorators = 741076755586776343ULL,
        
        
        simple_stmt = 15680233693926857886ULL,
        simple_stmts = 7071648921283339959ULL,
        compound_stmt = 4421131487532525991ULL,
        compound_stmts = 17100464358870324028ULL,
 
        measure = 13586824709325088415ULL,
        measure_array = 5009991113106631741ULL,
        reset = 1086335023529244512ULL,
        project_z = 2608459580584910443ULL,
        
        indent = 8881161079555216635ULL,
        dedent = 2224769550356995471ULL,
    };
}

#endif

