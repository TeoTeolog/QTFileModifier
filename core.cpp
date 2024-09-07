#include "core.h"

namespace fs = std::filesystem;

Core::Core() {

}

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

    std::filesystem::path directoryPath;
    std::ifstream file;
    std::ofstream oFile;
    Modifier modifier = {};
    std::vector<int8_t> sequence;

public:
    explicit FileHandler(const std::string &directoryPath) : directoryPath(directoryPath) {}

    void setModifier(int8_t val) { modifier.setModifierValue(val); }

    void setXor() { modifier.setModifierFunc(ModifierFunctions:: xor1); }
    void setAnd() { modifier.setModifierFunc(ModifierFunctions::and1); }
    void setOr() { modifier.setModifierFunc(ModifierFunctions:: or1); }
    void setShiftL() { modifier.setModifierFunc(ModifierFunctions::shiftLeft); }
    void setShiftR() { modifier.setModifierFunc(ModifierFunctions::shiftRight); }

    void openFile(const std::string &fileName) {
        if (!Core::isExistsFile(fileName)) {
            throw std::runtime_error("File \"" + fileName + "\" not exist!");
        }
        if (Core::isFileLocked(fileName)) {
            throw std::runtime_error("File \"" + fileName + "\" considered locked!");
        }
        file.open(fileName, std::ios::binary);
        if (!file.is_open()) {
            throw std::runtime_error("File \"" + fileName + "\" could not be open!");
        }
    }

    void processFile() {
        int8_t num;
        while (file.read(reinterpret_cast<char *>(&num), sizeof(num))) {
            sequence.push_back(modifier.execute(num));
        }
    }

    void outFile(const std::string &fileName) {
        if (Core::isExistsFile(fileName) && Core::isFileLocked(fileName)) {
            throw std::runtime_error("File \"" + fileName + "\" considered locked!");
        }
        oFile.open(fileName, std::ios::binary);
        if (!oFile.is_open()) {
            throw std::runtime_error("File \"" + fileName + "\" could not be open!");
        }
        for(auto& byte: sequence){
            oFile << byte;
        }
    }
};

std::string convertMaskToRegex(const std::string& mask) {
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

bool checkMask(std::string mask, std::string str){
    std::string regexPattern = convertMaskToRegex(mask);
    std::regex regex(regexPattern);
    return regex_match(str, regex);
}

std::unordered_set<std::string> listAllFilesInDir() {
    std::unordered_set<std::string> result;
    try {
        // current working directory
        std::filesystem::path cwd = std::filesystem::current_path();

        for (const auto& entry : std::filesystem::directory_iterator(cwd)) {
            if (entry.is_regular_file()) {  // Check if the entry is a regular file
                if (checkMask("*.txt", entry.path().filename().string())){
                    result.insert(entry.path().filename().string());
                }
            }
        }
    } catch (const std::filesystem::filesystem_error& e) {
        throw e;
    }

    return result;
}

std::string getUniqueName(const std::string fileName){
    std::string base, ext;
    std::size_t dotPos = fileName.find(".");
    if(dotPos != std::string::npos){
        base = fileName.substr(0,dotPos);
        ext = fileName.substr(0,dotPos);
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

bool Core::isExistsFile (const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

bool Core::isFileLocked(const std::string &filename) {
    namespace fs = std::filesystem;

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

int Core::oldMainTest(std::string targetFileName) {
    FileHandler FileHandler("./");
    FileHandler.openFile(targetFileName);
    FileHandler.setModifier(0xC0);
    FileHandler.setOr();
    FileHandler.processFile();
    FileHandler.outFile(targetFileName + "~out");
    return 0;
}
