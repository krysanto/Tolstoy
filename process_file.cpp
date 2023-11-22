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

using Chapter = std::vector<std::vector<std::string>>; // inner vector -> tokenized line
using Tolstoy = std::vector<Chapter>;

auto isNewChapter = [](const std::string &line)
{
    return line.rfind("CHAPTER", 0) == 0;
};

auto tokenize = [](const std::string &str) -> std::vector<std::string>
{
    std::istringstream iss(str);
    std::vector<std::string> tokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

    return tokens;
};

auto processLinesToChapters = [](const std::vector<std::string> &lines) -> Tolstoy
{
    return std::accumulate(lines.begin(), lines.end(), Tolstoy(), [&](Tolstoy acc, const std::string &line) -> Tolstoy
                           {
        if (isNewChapter(line)) {
            Chapter newChapter;
            newChapter.push_back(tokenize(line)); // Start new chapter with tokenized line
            acc.push_back(newChapter);
        } else if (!acc.empty()) {
            acc.back().push_back(tokenize(line)); // Tokenize line and add to current chapter
        }
        return acc; });
};

auto readFileLines = [](const std::string &fileName) -> std::optional<std::vector<std::string>>
{
    std::ifstream file(fileName);
    if (!file.is_open())
    {
        return std::nullopt;
    }

    std::vector<std::string> lines;
    std::copy(std::istream_iterator<std::string>(file), std::istream_iterator<std::string>(), std::back_inserter(lines));

    auto end_marker = std::find_if(lines.begin(), lines.end(), [](const std::string &line)
                                   { return line.find("END") != std::string::npos; });

    if (end_marker != lines.end())
    {
        lines.erase(end_marker, lines.end());
    }

    file.close();
    return lines;
};
