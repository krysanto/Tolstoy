#include <iostream>
#include <functional>
#include <numeric>
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <optional>
#include <string>
#include "process_file.cpp"

TEST_CASE("Testing isNewChapter function") {
    CHECK(isNewChapter("CHAPTER 1: A New Beginning") == true);
    CHECK(isNewChapter("Introduction") == false);
}

TEST_CASE("Testing tokenize function") {
    std::vector<std::string> expected{"This", "is", "a", "test"};
    CHECK(tokenize("This is a test") == expected);
}

TEST_CASE("Testing processLinesToChapters function") {
    std::vector<std::string> lines = {"CHAPTER 1", "First line of first chapter", "Second line of first chapter", "CHAPTER 2", "First line of second chapter"};
    auto tolstoy = processLinesToChapters(lines);
    CHECK(tolstoy.size() == 2);
    CHECK(tolstoy[0].size() == 3);
    CHECK(tolstoy[1].size() == 2);
}

TEST_CASE("Testing calculateTermDensity function") {
    Chapter chapter = {{"This", "is", "war"}, {"Peace", "is", "essential"}};
    Terms terms = {{"war", true}, {"Peace", true}};
    double density = calculateTermDensity(chapter)(terms);
    CHECK(density == 2);
}

TEST_CASE("Testing readFileLines function") {
    auto lines = readFileLines("war_and_peace.txt");
    CHECK(lines.has_value());
    CHECK(!lines->empty());
}

TEST_CASE("Testing isNewChapter with various inputs") {
    CHECK(isNewChapter("CHAPTER 10: The Journey Continues") == true);
    CHECK(isNewChapter("Chapter 11: Not a valid start") == false);
    CHECK(isNewChapter("Something else") == false);
    CHECK(isNewChapter("") == false);
}

TEST_CASE("Testing tokenize with different inputs") {
    CHECK(tokenize("") == std::vector<std::string>{});
    CHECK(tokenize("SingleWord") == std::vector<std::string>{"SingleWord"});
    CHECK(tokenize("Multiple words with spaces") == std::vector<std::string>{"Multiple", "words", "with", "spaces"});
}

TEST_CASE("Testing processLinesToChapters with empty and mixed input") {
    CHECK(processLinesToChapters({}).empty());
    
    std::vector<std::string> mixedLines = {"Random line", "CHAPTER 3", "Content of chapter 3", "", "CHAPTER 4", "Content of chapter 4"};
    auto processed = processLinesToChapters(mixedLines);
    CHECK(processed.size() == 2);
    CHECK(processed[0].size() == 3);
    CHECK(processed[1].size() == 2);
}

TEST_CASE("Testing calculateTermDensity with different chapters") {
    Chapter emptyChapter;
    Terms emptyTerms;
    CHECK(calculateTermDensity(emptyChapter)(emptyTerms) == doctest::Approx(0.0));

    Chapter chapter = {{"This", "is", "peace", "war", "peace"}, {"war", "is", "essential"}};
    Terms terms = {{"war", true}, {"peace", true}};
    double density = calculateTermDensity(chapter)(terms);
    CHECK(density == 4);
}

TEST_CASE("Testing readFileLines with non-existent and specific files") {
    CHECK_FALSE(readFileLines("nonexistentfile.txt").has_value());

    auto lines = readFileLines("war_and_peace.txt"); 
    CHECK(lines.has_value());
    CHECK(lines->size() == 562448);
}

TEST_CASE("Testing isNewChapter with edge cases") {
    CHECK(isNewChapter("CHAPTER") == true);
    CHECK(isNewChapter("chapter 1: A New Start") == false);
    CHECK(isNewChapter("CHAPTER1: A New Start") == true);
    CHECK(isNewChapter(" CHAPTER 1: A New Start") == false);
}

TEST_CASE("Testing tokenize with punctuation and edge cases") {
    CHECK(tokenize("This, is a test.") == std::vector<std::string>{"This", "is", "a", "test"});
    CHECK(tokenize("Word-with-hyphen") == std::vector<std::string>{"Wordwithhyphen"});
    CHECK(tokenize("1234 5678") == std::vector<std::string>{});
    CHECK(tokenize("Mixed1234Content") == std::vector<std::string>{"MixedContent"});
}

TEST_CASE("Testing processLinesToChapters with chapters having no content") {
    std::vector<std::string> linesWithEmptyChapters = {"CHAPTER 1", "CHAPTER 2", "CHAPTER 3"};
    auto tolstoy = processLinesToChapters(linesWithEmptyChapters);
    CHECK(tolstoy.size() == 3);
    CHECK(tolstoy[0][0][0].compare("CHAPTER "));
    CHECK(tolstoy[1][0][0].compare("CHAPTER "));
    CHECK(tolstoy[2][0][0].compare("CHAPTER "));
}

TEST_CASE("Testing calculateTermDensity with repetitive terms") {
    Chapter chapter = {{"war", "war", "war"}, {"peace", "peace", "peace"}};
    Terms terms = {{"war", true}, {"peace", true}};
    double density = calculateTermDensity(chapter)(terms);
    CHECK(density == doctest::Approx(6.0));
}

TEST_CASE("Testing readFileLines with various file contents") {
    auto emptyFileLines = readFileLines("empty_file.txt");
    CHECK(emptyFileLines.has_value());
    CHECK(emptyFileLines->empty());

    auto singleLineFileLines = readFileLines("single_line_file.txt");
    CHECK(singleLineFileLines.has_value());
    CHECK(singleLineFileLines->size() == 1);
}

TEST_CASE("Error handling in readFileLines with invalid file path") {
    auto result = readFileLines("invalid_path.txt");
    CHECK_FALSE(result.has_value());
}

TEST_CASE("Error handling in processLinesToChapters with invalid input format") {
    std::vector<std::string> invalidInput = {"Invalid", "Input", "Format"};
    auto error = processLinesToChapters(invalidInput);
    CHECK(error.empty());
}

TEST_CASE("Performance testing for processLinesToChapters with large input") {
    std::vector<std::string> largeInput(1000000, "CHAPTER 1000000: Large Input Test");
    auto start = std::chrono::high_resolution_clock::now();
    
    auto result = processLinesToChapters(largeInput);

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Elapsed time: " << elapsed.count() << " s\n";
    CHECK(result.size() == 1000000);
}

TEST_CASE("Boundary condition in tokenize with extremely long strings") {
    std::string longString(10000, 'a');
    auto result = tokenize(longString);
    CHECK(result.size() == 1);
    CHECK(result[0].length() == 10000);
}

TEST_CASE("Boundary condition in processLinesToChapters with empty lines") {
    std::vector<std::string> emptyLines(1000);
    auto result = processLinesToChapters(emptyLines);
    CHECK(result.empty());
}

