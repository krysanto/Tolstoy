#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "read_file.cpp"

int main (){
    std::optional<Tolstoy> tolstoy = fileStream("war_and_peace.txt");

    if (tolstoy.has_value()) {
        for (const auto& chapter : tolstoy.value()) {
            for (const auto& line : chapter) {
                std::cout << line << std::endl;
            }
        }
    } else {
        std::cerr << "File Stream Error..." << std::endl;
    }

    return 0;
}