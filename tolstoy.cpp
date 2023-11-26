#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "process_file.cpp"

using Chapter = std::vector<std::vector<std::string>>; // inner vector -> tokenized line
using Tolstoy = std::vector<Chapter>;
using Terms = std::map<std::string, bool>;
using WordCount = std::map<std::string, size_t>;

auto printChapterTheme = [](int chapterNumber) 
{
    return [chapterNumber](const std::string& theme) -> void
    {
        std::cout << "Chapter " << chapterNumber << ": " << theme << std::endl;
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

    int chapterNumber = 1;
    std::for_each(tolstoy.begin(), tolstoy.end(), [&chapterNumber, &war_map, &peace_map](const auto& chapter) 
    {
        double warDensity = calculateTermDensity(chapter)(war_map);
        double peaceDensity = calculateTermDensity(chapter)(peace_map);

        std::string chapterTheme = (warDensity > peaceDensity) ? "war-related" : "peace-related";

        printChapterTheme(chapterNumber)(chapterTheme);

        ++chapterNumber;
    });

    return 0;
}
