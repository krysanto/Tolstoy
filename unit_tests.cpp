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

TEST_CASE("Testing countOccurrencesInChapter function") {
    Chapter filteredTerms = {{"war"}, {"Peace", "war"}};
    auto wordCount = countOccurrencesInChapter(filteredTerms);
    CHECK(wordCount["war"] == 2);
    CHECK(wordCount["Peace"] == 1);
}

TEST_CASE("Testing calculateTermDensity function") {
    Chapter chapter = {{"This", "is", "war"}, {"Peace", "is", "essential"}};
    Terms terms = {{"war", true}, {"Peace", true}};
    double density = calculateTermDensity(chapter, terms);
    CHECK(density == 0.5);
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

TEST_CASE("Testing filterTermsInChapter with empty and specific inputs") {
    Chapter emptyChapter;
    Terms emptyTerms;
    CHECK(filterTermsInChapter(emptyChapter, emptyTerms, emptyTerms).empty());

    Chapter chapter = {{"war", "peace", "battle"}, {"negotiation", "treaty", "conflict"}};
    Terms warTerms = {{"war", true}, {"battle", true}};
    Terms peaceTerms = {{"peace", true}, {"treaty", true}};
    auto filtered = filterTermsInChapter(chapter, warTerms, peaceTerms);
    CHECK(filtered.size() == 2);
    CHECK(filtered[0].size() == 3);
    CHECK(filtered[1].size() == 1);
}

TEST_CASE("Testing countOccurrencesInChapter with different scenarios") {
    Chapter emptyFilteredTerms;
    CHECK(countOccurrencesInChapter(emptyFilteredTerms).empty());

    Chapter filteredTerms = {{"war", "peace"}, {"peace", "war", "peace"}};
    auto wordCount = countOccurrencesInChapter(filteredTerms);
    CHECK(wordCount["war"] == 2);
    CHECK(wordCount["peace"] == 3);
}

TEST_CASE("Testing processChapters with varying chapters") {
    Tolstoy emptyBook;
    Terms emptyTerms;
    CHECK(processChapters(emptyBook, emptyTerms, emptyTerms).empty());

    Tolstoy book = {{{"war", "peace"}}, {{"peace", "war", "peace"}}};
    Terms warTerms = {{"war", true}};
    Terms peaceTerms = {{"peace", true}};
    auto result = processChapters(book, warTerms, peaceTerms);
    CHECK(result.size() == 2);
    CHECK(result[0]["war"] == 1);
    CHECK(result[0]["peace"] == 1);
    CHECK(result[1]["war"] == 1);
    CHECK(result[1]["peace"] == 2);
}

TEST_CASE("Testing calculateTermDensity with different chapters") {
    Chapter emptyChapter;
    Terms emptyTerms;
    CHECK(calculateTermDensity(emptyChapter, emptyTerms) == doctest::Approx(0.0));

    Chapter chapter = {{"This", "is", "peace", "war", "peace"}, {"war", "is", "essential"}};
    Terms terms = {{"war", true}, {"peace", true}};
    double density = calculateTermDensity(chapter, terms);
    CHECK(density == 0.5);
}

TEST_CASE("Testing readFileLines with non-existent and specific files") {
    CHECK_FALSE(readFileLines("nonexistentfile.txt").has_value());

    auto lines = readFileLines("war_and_peace.txt"); 
    CHECK(lines.has_value());
    CHECK(lines->size() == 562448);
}