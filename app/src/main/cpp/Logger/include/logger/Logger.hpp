#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <mutex>

using std::string;
using std::mutex;
using std::stringstream;

class Logger
{

private:
    class InternalLogger
    {
    public:
        static const int BUFFER_SIZE = 1024;
        string LOG_FILE_NAME;
        mutex bufferMutex;
        mutex dumpMutex;
        stringstream buffer;

        ~InternalLogger();
        InternalLogger(const InternalLogger &) : InternalLogger() {}
        InternalLogger();

        static InternalLogger GetInstance();

        void Log(string log);
        void Dump();
    };

public:
    static void Log(const string log) noexcept
    {
        string brString ="- " + log + "\n";
        Logger::InternalLogger::GetInstance().Log(brString);
    }
};