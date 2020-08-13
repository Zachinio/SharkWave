#include "../include/logger/Logger.hpp"
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <iostream>

using std::string;
using std::runtime_error;

void CreateLogFile(string fileName)
{
    std::ifstream infile(fileName);
    if (!infile && !infile.good())
    {
        std::ofstream outfile(fileName);
        if (!outfile || !outfile.good())
        {
            throw runtime_error("failed to create logs file");
        }
    }
}

void Logger::InternalLogger::Dump()
{
    if (0 == buffer.str().size())
    {
        return;
    }
    std::ofstream outfile(LOG_FILE_NAME.c_str(), std::ios::out | std::ios::app);
    if (!outfile || !outfile.good())
    {
        throw runtime_error("Couldn't dump to file - No file found");
    }
    outfile << buffer.str();
    buffer.str("");
    buffer.clear();
}

void Logger::InternalLogger::Log(std::string log)
{
    try
    {
        bufferMutex.lock();
        buffer << log;
        if (buffer.fail())
        {
            Dump();
            buffer << log;
            if (buffer.fail())
            {
                bufferMutex.unlock();
                throw runtime_error("Could not log event after dump");
            }
        }
        else if (BUFFER_SIZE <= buffer.str().size())
        {
            Dump();
        }
        bufferMutex.unlock();
    }
    catch (const std::runtime_error &err)
    {
        printf("something went worng with the logger, %s\n", err.what());
    }
    bufferMutex.unlock();
}

Logger::InternalLogger Logger::InternalLogger::GetInstance()
{
    static InternalLogger instance;
    return instance;
}

Logger::InternalLogger::InternalLogger()
{
    try
    {
        LOG_FILE_NAME = "/data/local/tmp/logs.txt";
        CreateLogFile(LOG_FILE_NAME);
    }
    catch (const runtime_error &err)
    {
        //Handle logger errors here - TODO
    }
}

Logger::InternalLogger::~InternalLogger()
{
    Dump();
}