#include "core.h"

bool isExistsFile (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

bool isFileLocked(const std::string &filename) {
    // crutch
    auto tempFilename = filename + ".tmp";

    try {
        fs::rename(filename, tempFilename);
        fs::rename(tempFilename, filename);
        return false;
    } catch (const fs::filesystem_error &) {
        return true;
    }
}

std::string FilesListHandler::convertMaskToRegex(const std::string& mask) {
    std::string regexPattern = mask;
    // replacing of '*' to ".*"
    size_t pos = 0;
    while ((pos = regexPattern.find('*', pos)) != std::string::npos) {
        regexPattern.replace(pos, 1, ".*");
        pos += 2;
    }

    // replacing of '?' to ".*"
    pos = 0;
    while ((pos = regexPattern.find('?', pos)) != std::string::npos) {
        regexPattern.replace(pos, 1, ".");
        pos += 1;
    }
    return regexPattern;
}

bool FilesListHandler::checkMask(std::string mask, std::string str){
    std::string regexPattern = convertMaskToRegex(mask);
    std::regex regex(regexPattern);
    return regex_match(str, regex);
}

std::unordered_set<std::string> FilesListHandler::listAllFilesInDir() {
    std::unordered_set<std::string> result;
    try {
        if(directoryPath.empty()){
            throw std::runtime_error("Path \"" + directoryPath.string() + "\" invalid!");
        }
        if(maskString == ""){
            maskString = "*";
        }
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            if (entry.is_regular_file()) {
                if (checkMask(maskString, entry.path().filename().string())){
                    result.insert(entry.path().filename().string());
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        throw std::runtime_error("Error with listing files: " + std::string(e.what()));
    }

    return result;
}

std::string FilesListHandler::getUniqueName(const std::string& fileName){
    std::string base, ext;
    std::size_t dotPos = fileName.find(".");
    if(dotPos != std::string::npos){
        base = fileName.substr(0,dotPos);
        ext = fileName.substr(dotPos);
    }
    else{
        base = fileName;
        ext = "";
    }

    std::string newFileName = fileName;
    std::ifstream file(newFileName);
    int count = 1;

    while(file.good()){
        newFileName = base + "(" + std::to_string(count) + ")" + ext;
        count++;
        file.open(newFileName);
    }
    return newFileName;
}

int Core::oldMainTest(std::string targetDirName, std::string mask) {
    FilesListHandler handler;
    handler.setDir(targetDirName);
    handler.setMaskString(mask);
    handler.listAllFilesInDir();
    return 0;
}
