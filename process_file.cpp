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

    filteredTerms.erase(std::remove_if(filteredTerms.begin(), filteredTerms.end(), [](const std::vector<std::string>& line) 
    {
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
    // Flatten the chapter into a single vector of words
    std::vector<std::string> words;
    std::for_each(chapter.begin(), chapter.end(), [&words](const std::vector<std::string>& lines) 
    {
        std::transform(lines.begin(), lines.end(), std::back_inserter(words), [](const std::string& word) 
        {
            return word;
        });
    });

    // Count occurrences of terms
    size_t termOccurrences = std::count_if(words.begin(), words.end(), [&terms](const std::string& word) 
    {
        return terms.find(word) != terms.end();
    });

    // Calculate distances between terms
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
