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

namespace fs = std::filesystem;

bool isExistsFile (const std::string& name);
bool isFileLocked(const std::string &filename);

class FileHandler {
private:
    class ModifierFunctions {
    public:
        static int8_t xor1 (int8_t val, int8_t modifier) { return val ^ modifier; };
        static int8_t and1 (int8_t val, int8_t modifier) { return val & modifier; };
        static int8_t or1 (int8_t val, int8_t modifier) { return val | modifier; };
        static int8_t shiftLeft(int8_t val, int8_t modifier) { return val << modifier; };
        static int8_t shiftRight(int8_t val, int8_t modifier) { return val >> modifier; };
    };

    class Modifier {
        int8_t modifier = 0;
        std::function<int8_t(int8_t, int8_t)> modify;

    public:
        void setModifierValue(int8_t val) { modifier = val; }
        void setModifierFunc(std::function<int8_t(int8_t, int8_t)> modifierFunction) { modify = std::move(modifierFunction); }
        int8_t execute(int8_t val) { return modify(val, modifier); }
    };

    std::ifstream file;
    std::ofstream oFile;
    Modifier modifier;
    std::vector<int8_t> sequence;

public:
    explicit FileHandler(){};

    void setModifier(int8_t val) { modifier.setModifierValue(val); }

    void setXor() { modifier.setModifierFunc(ModifierFunctions:: xor1); }
    void setAnd() { modifier.setModifierFunc(ModifierFunctions::and1); }
    void setOr() { modifier.setModifierFunc(ModifierFunctions:: or1); }
    void setShiftL() { modifier.setModifierFunc(ModifierFunctions::shiftLeft); }
    void setShiftR() { modifier.setModifierFunc(ModifierFunctions::shiftRight); }

    void openFile(const std::string &fileName) {
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

    void processFile() {
        int8_t num;
        while (file.read(reinterpret_cast<char *>(&num), sizeof(num))) {
            sequence.push_back(modifier.execute(num));
        }
        file.close();
    }

    void deleteFile(const std::string &fileName){
        try {
            if (!fs::remove(fileName)) {
                throw std::runtime_error("File \"" + fileName + "\" could not be deleted!");
            }
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error("Error deleting file: " + std::string(e.what()));
        }
    }

    void outFile(const std::string &fileName) {
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
};

class FilesListHandler{
private:
    enum class ActionOnExist{
        replace,
        rename,
    };
    ActionOnExist actionOnExist = ActionOnExist::replace;
    FileHandler singleFileHandler;
    fs::path directoryPath;
    std::string maskString = "";
    bool needDelete = false;

    std::string convertMaskToRegex(const std::string& mask);
    bool checkMask(std::string mask, std::string str);
    std::string getUniqueName(const std::string& fileName);
public:

    void setDir(const std::string &dirPath){
        directoryPath = dirPath;
    }

    void setMaskString(const std::string &mask){
        maskString = mask;
    }

    void setDeleteFlag(bool flag){
        needDelete = flag;
    }

    void setActionOnExist(int val){
        actionOnExist = static_cast<ActionOnExist>(val);
    }

    std::unordered_set<std::string> listAllFilesInDir();

    void processFiles(std::function<void(const std::string&&)> logFunc){
        singleFileHandler.setAnd();
        singleFileHandler.setModifier(7);

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
};

class Core
{
public:
    Core(){
        handler = new FilesListHandler;
    };
    FilesListHandler* handler;
    int oldMainTest(std::string targetDirName, std::string mask);
};

#endif // CORE_H
