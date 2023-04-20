#include <iostream>
#include <thread>
#include <fstream>
// libmd5 https://github.com/gofer/libmd5
#include "md5.hpp"


void start_calculation(const std::string& hash, const std::string& permutation, const std::string& config) {

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
            std::string hash, permutation, config;
            if (!std::getline(temp_r, hash)) {
                std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
            } else if (!std::getline(temp_r, permutation)) {
                std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
            } else if (!std::getline(temp_r, config)) {
                std::cout << "TEMP FILE IS INVALID\nTRY TO START NEW EXECUTION\n";
            } else {
                temp_r.close();
                start_calculation(hash, permutation, config);
            }
        }
    } else if (argc == 3) {
        std::string hash(argv[1]), permutation(argv[1]), config(argv[2]);
        std::sort(permutation.begin(), permutation.end());
        start_calculation(hash, permutation, config);

        /*std::string hash_value = "e8129842ceb05ca859946a38ff29acce";

        std::string config = "asdf32fsa";
        auto hash = MD5::hash(config);
        std::cout << MD5::to_string(hash) << '\n';
        std::sort(config.begin(), config.end());

        size_t i = 0;
        do {
            auto hash = MD5::hash(config);
            auto string_hash = MD5::to_string(hash);
            if (string_hash == hash_value) {
                std::cout << i << ' ' << config;
                break;
            }
            ++i;
        } while (std::next_permutation(config.begin(), config.end()));*/

    } else {
        std::cout << "INVALID PARAMETERS\n";
    }
    return 0;
}
