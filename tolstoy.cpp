#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "read_file.cpp"

int main (){
    Tolstoy tolstoy = read_file("war_and_peace.txt");

    for (auto chapter : tolstoy){
        for (auto line : chapter){
            std::cout << line << std::endl;
        }
    }

    return 0;
}