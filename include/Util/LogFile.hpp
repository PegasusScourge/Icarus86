#ifndef _logger_hpp
#define _logger_hpp

/*
    **> \File           LogFile.hpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Header for Logger.cpp
    **> \Details        None
    **> \Notes          None
*/

#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

namespace i86 {

namespace util {

// ****************************************************************
// Class LogFile
// ****************************************************************

class LogFile {

private:
    // These can be accessed from both threads
    std::string m_logFile;
    std::atomic_bool m_logThreadRunning;
    std::mutex m_logMutex;
    std::vector<std::string> m_logQueue;
    std::thread m_logThread;

    // This can only be accessed from the logging thread outside of the constructor
    std::ofstream mT_logStream;

    // This can only be accessed from the main program thread outside of the constructor
    std::stringstream mP_stringQueue;

    /*
        \Function   loggingThread
        \Brief      Constructs loggingThread
        \Details    Constructs loggingThread
        \Parameter  None
        \Returns    None
    */
    void loggingThread();

    /*
        \Function   log
        \Brief      Pushes a new log piece onto the queue
        \Details    Pushes a new log piece onto the queue
        \Parameter  std::string& output
        \Returns    None
    */
    void log(std::string& output);

public:
    /*
        \Function   LogFile
        \Brief      Constructs LogFile
        \Details    Constructs LogFile
        \Parameter  std::string file
        \Returns    None
    */
    LogFile(std::string file);
    ~LogFile();

    /*
        \Function   strs
        \Brief      Exposes a string stream object to write to
        \Details    Exposes a string stream object to write to
        \Parameter  None
        \Returns    std::stringstream& stream
    */
    std::stringstream& strs();

    /*
        \Function   sstream
        \Brief      Flushes the string stream to the queue
        \Details    Flushes the string stream to the queue
        \Parameter  None
        \Returns    None
    */
    void flushss();

    /*
        \Function   log_rawString
        \Brief      Logs the raw string on a new line
        \Details    Logs the raw string on a new line
        \Parameter  std::string str
        \Returns    None
    */
    void log_rawString(std::string str);

}; // end class LogFile

} // end namespace util

} // end namespace i86

#endif