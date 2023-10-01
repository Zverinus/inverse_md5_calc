#include <iostream>
#include <thread>
#include <fstream>
#include <algorithm>
#include <cinttypes>
#include <windows.h>
// libmd5 https://github.com/gofer/libmd5
#include "md5.hpp"


struct md5_calc_unit {
    std::string calculated_hash;
    std::string permutation;
    bool finished = false;
    size_t counter = 1;
    bool terminate = false;

    md5_calc_unit(std::string calculated_hash, std::string permutation, bool finished, size_t counter):
    calculated_hash(calculated_hash), permutation(permutation), finished(finished), counter(counter), terminate(false) {};
};


void comp_permutation(md5_calc_unit* unit) {
    size_t local_counter = 0;
    while (true) {
        if (unit->terminate) {
            return;
        }
        if (local_counter != unit->counter) { // Calculating hash if got new permutation
            auto hash = MD5::hash(unit->permutation);
            unit->calculated_hash = MD5::to_string(hash);
            unit->finished = true;
            local_counter = (local_counter + 1) % (UINT64_MAX - 1);
        }
    }
}


void start_calculation(const std::string& hash, std::string& permutation) {
    size_t free_threads = std::thread::hardware_concurrency(); // Choosing threads count
    if (free_threads == 0 || free_threads == 1)
        free_threads = 1;
    else
        --free_threads;
    std::vector<std::thread> threads;
    std::vector<md5_calc_unit*> units;
    for (size_t i = 0; i < free_threads; ++i) { // Creating threads and connected units
        auto* unit = new md5_calc_unit("", permutation, false, 1);
        units.push_back(unit);
        threads.emplace_back(comp_permutation, unit);
        threads[i].detach();
    }
    size_t i = 0;
    do {
        while (!units[i]->finished)
            i = (i + 1) % free_threads;
        if (units[i]->calculated_hash == hash) { // Comparing hashes
            std::cout << "INVERSE MD5 FOUND:\n\n";
            std::cout << units[i]->permutation << "\n\n";
            std::ofstream temp_file("inverse_md5_calc_temp.txt"); // Writing last permutation to temp file
            if (temp_file.is_open())
                temp_file.close();
            for (int ind = 0; ind < free_threads; ++ind) { // Terminating threads
                units[i]->terminate = true;
            }
            std::cout << "EXECUTION FINISHED\n";
            return;
        } else { // Sending new permutation to free thread
            units[i]->finished = false;
            units[i]->permutation = permutation;
            units[i]->counter = (units[i]->counter + 1) % (UINT64_MAX - 1);
        }
        if (GetKeyState('S') & 0x8000) { // Handling S press
            std::ofstream temp_file("inverse_md5_calc_temp.txt");
            temp_file << hash << '\n' << permutation;
            temp_file.close();
            std::cout << "EXECUTION PAUSED\nTO RESUME RUN:\ninverse_md5_calc.exe resume\n";
            for (i = 0; i < free_threads; ++i) {
                units[i]->terminate = true;
            }
            return;
        }
    } while (std::next_permutation(permutation.begin(), permutation.end()));

    std::cout << "FAILED TO INVERSE MD5\n";
    std::ofstream temp_file("inverse_md5_calc_temp.txt");
    if (temp_file.is_open())
        temp_file.close();
    for (int ind = 0; ind < free_threads; ++ind) {
        units[i]->terminate = true;
    }
}


int main(int argc, char* argv[]) {
    if (argc == 1) {
        std::cout << "NO ARGUMENTS PASSED\n";
        std::cout << "TO START EXECUTION RUN:\ninverse_md5_calc.exe <hash> <path_to_config_file>\n";
        std::cout << "TO RESUME EXECUTION RUN:\ninverse_md5_calc.exe resume\n";
    } else if (argc == 2 && std::string(argv[1]) == "resume") {
        std::cout << "resuming...\n";
        std::ifstream temp_r("inverse_md5_calc_temp.txt");
        if (!temp_r.is_open()) {
            std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
        } else {
            std::string hash, permutation;
            if (!std::getline(temp_r, hash)) {
                std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
            } else if (!std::getline(temp_r, permutation)) {
                std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
            } else {
                temp_r.close();
                start_calculation(hash, permutation);
            }
        }
    } else if (argc == 3) {
        std::string hash(argv[1]), permutation, config_path(argv[2]);
        std::ifstream config_file(config_path);
        if (!config_file.is_open()) {
            std::cout << "CONFIG FILE PATH IS INVALID\n";
        } else {
            std::getline(config_file, permutation);
            std::sort(permutation.begin(), permutation.end());
            std::cout << "EXECUTION STARTED\n";
            std::cout << "PRESS S TO PAUSE EXECUTION\n";
            start_calculation(hash, permutation);
        }
    } else {
        std::cout << "INVALID PARAMETERS\n";
    }
    return 0;
}