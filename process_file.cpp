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
#include <map>
#include <ranges>
#include <chrono>

using Chapter = std::vector<std::vector<std::string>>; // inner vector -> tokenized line
using Tolstoy = std::vector<Chapter>;
using Terms = std::map<std::string, bool>;
using WordCount = std::map<std::string, size_t>;

auto isNewChapter = [](const std::string &line)
{
    return line.rfind("CHAPTER", 0) == 0;
};

auto tokenize = [](const std::string &str) -> std::vector<std::string> 
{
    std::istringstream iss(str);
    std::vector<std::string> rawTokens{std::istream_iterator<std::string>{iss}, std::istream_iterator<std::string>{}};

    // Remove punctuation
    std::transform(rawTokens.begin(), rawTokens.end(), rawTokens.begin(), [](const std::string &token) 
    {
        std::string cleanToken;
        std::copy_if(token.begin(), token.end(), std::back_inserter(cleanToken), [](char c) { return std::isalpha(c) || c == '\''; });
        return cleanToken;
    });

    // Remove empty tokens
    auto it = std::remove_if(rawTokens.begin(), rawTokens.end(), [](const std::string &token) { return token.empty(); });
    rawTokens.erase(it, rawTokens.end());

    return rawTokens;
};

auto processLinesToChapters = [](const std::vector<std::string> &lines) -> Tolstoy
{
    return std::accumulate(lines.begin(), lines.end(), Tolstoy(), [&](Tolstoy acc, const std::string &line) -> Tolstoy
    {
        if (isNewChapter(line))
        {
            Chapter newChapter;
            newChapter.push_back(tokenize(line)); // Start new chapter with tokenized line
            acc.push_back(newChapter);
        }
        else if (!acc.empty())
        {
            acc.back().push_back(tokenize(line)); // Tokenize line and add to current chapter
        }

        return acc;
    });
};

auto processTerms = [](const std::vector<std::string> inputTerms)
{
    return [&inputTerms](const bool isWarTerm) -> Terms
    {
        return std::accumulate(inputTerms.begin(), inputTerms.end(), Terms(), [&] (Terms acc, const std::string inputTerm) -> Terms
        {
            acc.insert({inputTerm, isWarTerm});

            return acc;
        });
    };
};

auto flattenChapterToWords = [](const Chapter& chapter) -> std::vector<std::string>
{
    std::vector<std::string> words;

    std::for_each(chapter.begin(), chapter.end(), [&words](const std::vector<std::string>& lines) 
    {
        std::transform(lines.begin(), lines.end(), std::back_inserter(words), [](const std::string& word) 
        {
            return word;
        });
    });

    return words;
};

auto countOccurrences = [](const std::vector<std::string>& words)
{
    return [&words](const Terms& terms) -> size_t
    {
        size_t termOccurrences = std::count_if(words.begin(), words.end(), [&terms](const std::string& word) 
        {
            return terms.find(word) != terms.end();
        });

        return termOccurrences;
    };
};

auto calculateDistanceBetweenTerms = [](const std::vector<std::string>& words)
{
    return [&words](const Terms& terms) -> std::vector<size_t>
    {
        std::vector<size_t> distances;
        size_t lastPosition = std::string::npos;
        std::transform(words.begin(), words.end(), std::back_inserter(distances), [&lastPosition, &terms](const std::string& word) -> size_t 
        {
            if (terms.find(word) != terms.end()) 
            {
                size_t distance = lastPosition == std::string::npos ? 0 : lastPosition;
                lastPosition = 0;
                return distance;
            } 
            else 
            {
                ++lastPosition;
                return 0;
            }
        });

        // Remove 0s from distances
        distances.erase(std::remove(distances.begin(), distances.end(), 0), distances.end());

        return distances;
    };
};

auto calculateTermDensity = [](const Chapter& chapter)
{
    return [&chapter](const Terms& terms) -> double 
    {
        std::vector<std::string> words = flattenChapterToWords(chapter);
        size_t termOccurrences = countOccurrences(words)(terms);
        std::vector<size_t> distances = calculateDistanceBetweenTerms(words)(terms);

        // Calculate average distance between terms
        double averageDistance = distances.empty() ? 0.0 : std::accumulate(distances.begin(), distances.end(), 0.0) / distances.size();

        // Density calculation
        double density = termOccurrences;
        
        if (averageDistance > 0.0) 
        {
            density /= averageDistance;
        }

        return density;
    };
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
    {
        return line.find("END") != std::string::npos;
    });

    if (end_marker != lines.end())
    {
        lines.erase(end_marker, lines.end());
    }

    file.close();
    return lines;
};