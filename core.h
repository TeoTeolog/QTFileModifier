#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <regex>
#include <QTimer>

class Core
{
public:
    Core();
    int oldMainTest(std::string targetFileName);
    static bool isFileLocked(const std::string &filename);
    static bool isExistsFile (const std::string& name);    
};

#endif // CORE_H
