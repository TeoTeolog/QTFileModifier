#include "core.h"

/* check is file with given [name] exist */
bool isExistsFile (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

/* check is file with given [name] locked (opened by another process) */
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

// SingleFileHandler

void FileHandler::openFile(const std::string &fileName) {
    if (!isExistsFile(fileName)) {
        throw std::runtime_error("File \"" + fileName + "\" not exist!");
    }
    if (isFileLocked(fileName)) {
        throw std::runtime_error("File \"" + fileName + "\" considered locked!");
    }
    file.open(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("File \"" + fileName + "\" could not be open!");
    }
    sequence.clear();
}

void FileHandler::processFile() {
    int8_t num;
    while (file.read(reinterpret_cast<char *>(&num), sizeof(num))) {
        sequence.push_back(modifier.execute(num));
    }
    file.close();
}

void FileHandler::deleteFile(const std::string &fileName){
    try {
        if (!fs::remove(fileName)) {
            throw std::runtime_error("File \"" + fileName + "\" could not be deleted!");
        }
    } catch (const fs::filesystem_error& e) {
        throw std::runtime_error("Error deleting file: " + std::string(e.what()));
    }
}

void FileHandler::outFile(const std::string &fileName) {
    if (isExistsFile(fileName) && isFileLocked(fileName)) {
        throw std::runtime_error("File \"" + fileName + "\" considered locked!");
    }
    oFile.open(fileName, std::ios::binary);
    if (!oFile.is_open()) {
        throw std::runtime_error("File \"" + fileName + "\" could not be open!");
    }
    for(auto& byte: sequence){
        oFile << byte;
    }
    oFile.close();
}

// All files handler

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
    std::size_t dotPos = fileName.find_last_of(".");
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
    // if file can be opened - it already exist
    while(file.good()){
        newFileName = base + "(" + std::to_string(count) + ")" + ext;
        count++;
        file.close();
        file.open(newFileName);
    }
    return newFileName;
}

void FilesListHandler::processFiles(std::function<void(const std::string&&)> logFunc){
    for(auto & file: listAllFilesInDir()){
        logFunc("Processing file: " + file + " started!");
        singleFileHandler.openFile(directoryPath.string() + "/" + file);
        singleFileHandler.processFile();
        if(needDelete) singleFileHandler.deleteFile(directoryPath.string() + "/" + file);
        switch(actionOnExist){
            case ActionOnExist::replace:
                singleFileHandler.outFile(directoryPath.string() + "/" + file);
                break;
            case ActionOnExist::rename:
                singleFileHandler.outFile(getUniqueName(directoryPath.string() + "/" + file));
                break;
        }
        logFunc("Processing file: " + file + " completed!");
    }
}

