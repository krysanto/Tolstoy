#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include "process_file.cpp"

int main()
{
    auto lines = readFileLines("war_and_peace.txt");

    if (!lines.has_value())
    {
        std::cerr << "File could not be opened or read." << std::endl;
        return 1;
    }

    auto tolstoy = processLinesToChapters(lines.value());

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

    return 0;
}
