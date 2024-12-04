#include <iostream>
#include <fstream>
#include <string>

int main() {
    std::ifstream input("input.txt");
    std::ifstream output("output.txt");

    if (!input.is_open() || !output.is_open()) {
        std::cerr << "Error opening files!" << std::endl;
        return 1;
    }

    std::string input_line, output_line;
    int line_number = 0;
    bool mismatch_found = false;

    while (std::getline(input, input_line) && std::getline(output, output_line)) {
        line_number++;

        if (input_line != output_line) {
            mismatch_found = true;
            std::cout << "Mismatch found at line " << line_number << std::endl;
            std::cout << "Input:  " << input_line << std::endl;
            std::cout << "Output: " << output_line << std::endl;

            // Print 100 characters after the first mismatch
            std::string::size_type pos = input_line.find_first_not_of(output_line);
            if (pos != std::string::npos) {
                std::cout << "100 characters after mismatch:" << std::endl;
                std::cout << input_line.substr(pos, 100) << std::endl;
            }
            break;
        }
    }

    if (!mismatch_found) {
        std::cout << "Files are identical." << std::endl;
    }

    input.close();
    output.close();

    return 0;
}
