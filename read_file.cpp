#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include <fstream>
#include <sstream>
#include <optional>
#include <algorithm>

using Chapter = std::vector<std::string>;
using Tolstoy = std::vector<Chapter>;


auto fileStream = [](const std::string& fileName) -> std::optional<Tolstoy>
{
    std::ifstream file(fileName);

    if (!file.is_open())
    {
        return std::nullopt;
    }

    std::vector<std::string> words((std::istream_iterator<std::string>(file)), std::istream_iterator<std::string>());
    file.close();

    Tolstoy chapters;
    Chapter currentChapter;
    std::string currentLine;
    bool lastChapterEnded = false;

    std::for_each(words.begin(), words.end(), [&](const std::string& word) 
    {
        if (word == "***" && currentLine.find("END OF THE PROJECT GUTENBERG EBOOK") != std::string::npos) 
        {
            lastChapterEnded = true;

            if (!currentChapter.empty()) 
            {
                chapters.push_back(currentChapter);
                currentChapter.clear();
            }

            return;
        }
        
        if (lastChapterEnded) 
        {
            return;
        }

        if (word.rfind("CHAPTER", 0) == 0) 
        {
            if (!currentChapter.empty()) 
            {
                chapters.push_back(currentChapter);
                currentChapter.clear();
            }
        }

        currentLine += word + " ";

        if (word.back() == '.') 
        {
            currentChapter.push_back(currentLine);
            currentLine.clear();
        }
    });

    return std::make_optional(chapters);
};