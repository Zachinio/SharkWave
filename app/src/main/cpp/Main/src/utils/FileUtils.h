#pragma once

#include <string>
#include <fstream>

using std::runtime_error;
using std::ifstream;

static string ReadFile(string path) {
    std::ifstream input(path);
    if (!input.good()) {
        return "";
    }
    if (input.peek() == ifstream::traits_type::eof()) {
        return "";
    }
    std::vector<char> bytes(
            (std::istreambuf_iterator<char>(input)),
            (std::istreambuf_iterator<char>()));
    input.close();
    if (bytes.empty()) {
        return "";
    }
    return string(bytes.begin(), bytes.end());
}