#pragma once

#include <string>
#include "FileUtils.h"

static string OUTPUT_FILE_PATH = "/data/local/tmp/output.txt";

static std::string executeCommand(string command) {
    string commandRedirect =
            string("/system/bin/sh -c \"") + command + string(" > " + OUTPUT_FILE_PATH + "\"");
    system(commandRedirect.c_str());

    return ReadFile(OUTPUT_FILE_PATH);
}