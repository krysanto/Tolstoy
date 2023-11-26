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

auto filterTermsInChapter = [](const Chapter& chapter, const Terms& warTerms, const Terms& peaceTerms) -> Chapter
{
    Chapter filteredTerms(chapter.size());

    std::transform(chapter.begin(), chapter.end(), filteredTerms.begin(), [&warTerms, &peaceTerms](const std::vector<std::string>& line)
    {
        std::vector<std::string> filteredLine;

        std::copy_if(line.begin(), line.end(), std::back_inserter(filteredLine), [&warTerms, &peaceTerms](const std::string& word)
        {
            return warTerms.find(word) != warTerms.end() || peaceTerms.find(word) != peaceTerms.end();
        });

        return filteredLine;
    });

    filteredTerms.erase(
        std::remove_if(filteredTerms.begin(), filteredTerms.end(), [](const std::vector<std::string>& line) {
            return line.empty();
        }), filteredTerms.end());

    return filteredTerms;
};

auto countOccurrencesInChapter = [](const Chapter& filteredTerms) -> WordCount 
{
    return std::accumulate(filteredTerms.begin(), filteredTerms.end(), WordCount{}, [](WordCount acc, const std::vector<std::string>& line) 
    {
        std::for_each(line.begin(), line.end(), [&acc](const std::string& word) 
        {
            ++acc[word];
        });
            
        return acc;
    });
};

auto processChapters = [](const Tolstoy& book, const Terms& warTerms, const Terms& peaceTerms) -> std::vector<WordCount>
{
    std::vector<WordCount> chaptersWordCounts(book.size());

    std::transform(book.begin(), book.end(), chaptersWordCounts.begin(), [&warTerms, &peaceTerms](const Chapter& chapter)
    {
        Chapter filteredTerms = filterTermsInChapter(chapter, warTerms, peaceTerms);
        return countOccurrencesInChapter(filteredTerms);
    });

    return chaptersWordCounts;
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

auto calculateTermDensity = [](const Chapter& chapter, const Terms& terms) -> double 
{
    // Flatten Chapter
    std::vector<std::string> words = std::accumulate(chapter.begin(), chapter.end(), std::vector<std::string>{},
        [](std::vector<std::string> acc, const std::vector<std::string>& lines) 
        {
            acc.insert(acc.end(), lines.begin(), lines.end());
            return acc;
        });

    // Calculate distances
    auto distances = std::accumulate(words.begin(), words.end(), std::pair<std::vector<size_t>, size_t>{{}, 0},
        [&terms](std::pair<std::vector<size_t>, size_t> acc, const std::string& word) 
        {
            auto& [distances, lastPosition] = acc;
            if (terms.find(word) != terms.end()) 
            {
                if (lastPosition > 0) 
                {
                    distances.push_back(lastPosition);
                }
                lastPosition = 0;
            } 
            else 
            {
                ++lastPosition;
            }
            return acc;
        });

    // Calculate average distance
    double totalDistance = std::accumulate(distances.first.begin(), distances.first.end(), 0.0);
    double averageDistance = distances.first.empty() ? 0.0 : totalDistance / distances.first.size();

    // Calculate term frequency
    auto counts = std::count_if(words.begin(), words.end(), [&terms](const std::string& word) 
    {
        return terms.find(word) != terms.end();
    });

    // Normalize average distance by term frequency
    double termDensity = counts == 0 ? 0.0 : 1.0 / averageDistance;

    return termDensity;
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
