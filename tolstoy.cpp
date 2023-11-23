#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "process_file.cpp"

auto filterWords = [](const Chapter currentChapter)
{
    return [&currentChapter]()
    {

    };
};


int main()
{
    std::optional<std::vector<std::string>> lines = readFileLines("war_and_peace.txt");
    std::optional<std::vector<std::string>> peace = readFileLines("peace_terms.txt");
    std::optional<std::vector<std::string>> war = readFileLines("war_terms.txt");


    if (!lines.has_value() || !peace.has_value() || !war.has_value())
    {
        std::cerr << "One or more files could not be opened or read." << std::endl;
        return 1;
    }

    Tolstoy tolstoy = processLinesToChapters(lines.value());
    Terms peace_map = processTerms(peace.value())(false);
    Terms war_map = processTerms(war.value())(true);
/*
    for (const auto &chapter : tolstoy)
    {
        for (const auto &line : chapter)
        {
            for (const auto &word : line)
            {
                std::cout << word << ' ';
            }
            std::cout << std::endl;
        }
    }
*/
    for (auto &[k, v] : peace_map){
        std::cout << k << "   " << v << std::endl;
    }

    for (auto &[k, v] : war_map){
        std::cout << k << "   " << v << std::endl;
    }
    return 0;
}

