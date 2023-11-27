#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "process_file.cpp"

auto printChapterTheme = [](const int chapterNumber) 
{
    return [chapterNumber](const std::string& theme) -> void
    {
        std::cout << "Chapter " << chapterNumber << ": " << theme << std::endl;
    };
};

int main()
{
    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    std::cout << "Elapsed time for processing War and Peace: " << elapsed.count() << " s\n";

    return 0;
}
