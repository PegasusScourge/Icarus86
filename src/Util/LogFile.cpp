/*
    **> \File           LogFile.cpp
    **> \Author         Matthew Clarke
    **> \Create Date    2021-03-10
    **> \Brief          Main logging utility
    **> \Details        None
    **> \Notes          None
*/

#include "Util/LogFile.hpp"

#include <iostream>

// ****************************************************************
// Class LogFile
// ****************************************************************

i86::util::LogFile::LogFile(std::string file) {
    m_logFile = file;
    mT_logStream.open(m_logFile);
    m_logThreadRunning = true;

    m_logThread = std::thread(&LogFile::loggingThread, this);
}

i86::util::LogFile::~LogFile() {
    m_logThreadRunning = false;
    std::cout << "Awaiting logging thread for \"" << m_logFile << "\" to terminate" << std::endl;
    m_logThread.join();
    std::cout << "Logging thread for \"" << m_logFile << "\" terminating" << std::endl;
    mT_logStream.close();
}

void i86::util::LogFile::log(std::string& output) {
    std::lock_guard guard(m_logMutex);

    m_logQueue.push_back(output);
}

void i86::util::LogFile::loggingThread() {

    std::vector<std::string> queueToWrite;

    while(m_logThreadRunning) {
        
        {
            std::lock_guard guard(m_logMutex);
            queueToWrite = m_logQueue;
            m_logQueue.clear();
        }

        for (size_t i = 0; i < queueToWrite.size(); i++) {
            mT_logStream << queueToWrite[i] << std::endl;
        }
        queueToWrite.clear();
    }

    mT_logStream.flush();
}

void i86::util::LogFile::log_rawString(std::string str) {
    log(str);
}

std::stringstream& i86::util::LogFile::strs() {
    return mP_stringQueue;
}

void i86::util::LogFile::flushss() {
    log_rawString(mP_stringQueue.str());
    mP_stringQueue.clear();
}