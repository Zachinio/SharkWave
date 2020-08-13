#include <cstdio>
#include <string>
#include <logger/Logger.hpp>

void showUsage();

using std::string;
using std::stoi;

int pid = -1;

int main(int argc, char *argv[]) {
    if (argc < 3) {
        showUsage();
        return 0;
    }
    for (int i = 1; i < argc; i += 2) {
        if (i + 1 == argc) {
            printf("no value passed for %s\n", argv[i]);
            return -1;
        }
        string parameter = string(argv[i]);
        string value = string(argv[i + 1]);

        if ("--pid" == parameter) {
            pid = stoi(parameter);
        }
    }

    try {
        Logger::Log("------------------- Start of Log -------------------\n");

    }
    catch (const std::runtime_error &err) {
        Logger::Log("error" + string(err.what()));
    }

    Logger::Log("------------------- End of Log -------------------\n");
    return 0;
}

void showUsage() {
    printf("Usage of SharkWave\n");
    printf("--pid <PID>\n   A pid to sniff its network\n" );
}
