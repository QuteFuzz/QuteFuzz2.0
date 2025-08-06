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

#define WILDCARD_MAX 5

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

std::optional<int> safe_stoi(const std::string& str);

std::vector<std::vector<int>> n_choose_r(const int n, const int r);

int vector_sum(std::vector<int> in);

int vector_max(std::vector<int> in);

void pipe_to_command(std::string command, std::string write);

std::string pipe_from_command(std::string command);

std::string escape(const std::string& str);

std::string random_hex_colour();

namespace Common {
    constexpr char TOP_LEVEL_CIRCUIT_NAME[] = "main_circuit";
    constexpr int MIN_N_QUBITS_IN_ENTANGLEMENT = 2;
    constexpr int MIN_QUBITS = 3;
    constexpr int MIN_BITS = 1;
    constexpr int MAX_QUBITS = std::max(MIN_QUBITS + 1, (int)(0.5 * WILDCARD_MAX));
    constexpr int MAX_BITS = std::max(MIN_BITS + 1, (int)(0.5 * WILDCARD_MAX));
    constexpr int MAX_SUBROUTINES = (int)(0.5 * WILDCARD_MAX);
    constexpr int COMPOUND_STMT_DEPTH = 2;

    extern bool plot;
    extern bool verbose;
    extern bool render_dags;
    
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
        project_z = 2608459580584910443ULL,
        measure_and_reset = 15416528854723921477ULL,
        Measure = 13586824709325088415ULL,

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
        qubit_def_name = 9637820166840754028ULL,
        bit_def_name = 10693244643305520522ULL,
        qreg_size = 11502232252882731618ULL,
        creg_size = 4440641367620343308ULL,
        qubit = 9613145697666387222ULL,
        bit = 23676991090714736ULL,
        qubit_op = 7363837753828900628ULL,
        gate_op = 17845938762380861480ULL,
        subroutine_op = 4275855153236832595ULL,
        measurement_op = 5371156234969671235ULL,
        gate_op_kind = 14929235841933060947ULL,
        gate_name = 4107851538286704628ULL,
        qubit_list = 18380990572907722739ULL,
        bit_list = 3372974018932553881ULL,
        qubit_def_list = 12288968177510665943ULL, 
        qubit_def_size = 16541258826307231238ULL,      
    
        qubit_defs_internal = 15763474746718250229ULL,
        qubit_def_internal = 8367506410210901254ULL,
        register_qubit_def_internal = 7889892016210498366ULL,
        singular_qubit_def_internal = 14695716164594440254ULL,
    
        qubit_defs_external = 12875408231208951867ULL,
        qubit_defs_external_owned = 2338026120644312637ULL,
        qubit_def_external = 9066873179617854836ULL,
        register_qubit_def_external = 17118825122301876444ULL,
        singular_qubit_def_external = 5477905196976266716ULL,

        bit_defs_external = 9605680741777250781ULL,
        bit_def_external = 13726870050363098110ULL,
        bit_def_internal = 5486644757629614132ULL,
        bit_defs_internal = 17430815952964625635ULL,
        singular_bit_def_external = 4316476105238344134ULL,
        register_bit_def_external = 8288554911174669510ULL,
        singular_bit_def_internal = 11391126108199619852ULL,
        register_bit_def_internal = 15363204914135945228ULL,

        singular_qubit = 358282167276964286ULL,
        register_qubit = 1224902788537856702ULL,
        singular_bit = 9032933660288388984ULL,
        register_bit = 17133561988447867000ULL,

        qubit_name = 8757953683662904688ULL,
        bit_name = 6055046893651473182ULL,
        qubit_index = 6830264791288854081ULL,
        bit_index = 8231177906794104931ULL,
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
        comptime_block = 13310967149289622197ULL,
        comptime_block_def = 5962160394468879029ULL,
        non_comptime_block = 15717574089437842153ULL,
        arg_singular_qubit = 18441410415153523ULL,
        arg_register_qubits = 3829129026513754988ULL,
        
        simple_stmt = 15680233693926857886ULL,
        simple_stmts = 7071648921283339959ULL,
        compound_stmt = 4421131487532525991ULL,
        compound_stmts = 17100464358870324028ULL,
        
        indent = 8881161079555216635ULL,
        dedent = 2224769550356995471ULL,

        if_stmt = 5300980200188049869ULL,
        arguments = 16442092644301152671ULL,
        arg = 16669728881526232807ULL,
        else_stmt = 2582766405432659795ULL,
        elif_stmt = 9453565397799917274ULL,
        disjunction = 13554539731759707019ULL,
        conjunction = 15291463859833294983ULL,
        inversion = 6572250267686125186ULL,
        expression = 17035010316065342411ULL,
        compare_op_bitwise_or_pair = 11179282617234990551ULL,
        name = 14176396743819860870ULL,
        number = 9237349086447201248ULL,
    };
}

#endif

