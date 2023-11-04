#include <iostream>
#include <string>
#include <functional>
#include <numeric>
#include <vector>
#include <ranges>
#include <fstream>

using Chapter = std::vector<std::string>;
using Tolstoy = std::vector<Chapter>;


auto read_file = [](const auto file_path){
    Tolstoy content;

    std::ifstream input_file(file_path);
    if (!input_file.is_open()){
        return content;
    }

    std::string line;
    Chapter current_chapter;
    bool intro = true;

    while (getline(input_file, line)){
        if (line.substr(0, 7) == "CHAPTER" || line.substr(0, 7) == "*** END"){
            if (!intro){
                content.push_back(current_chapter);
            }
            intro = false;
            current_chapter.clear();
        }
        current_chapter.push_back(line);
    }

    input_file.close();
    return content;
};