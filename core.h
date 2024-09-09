#ifndef CORE_H
#define CORE_H

#include <QCoreApplication>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <regex>
#include <QTimer>

namespace fs = std::filesystem;

bool isExistsFile (const std::string& name);
bool isFileLocked(const std::string &filename);

/* class for modify single file*/
class FileHandler {
private:
    /* all modify function collected in one place */
    class ModifierFunctions {
    public:
        static int8_t myXor (int8_t val, int8_t modifier) { return val ^ modifier; };
        static int8_t myAnd (int8_t val, int8_t modifier) { return val & modifier; };
        static int8_t myOr (int8_t val, int8_t modifier) { return val | modifier; };
        static int8_t shiftLeft(int8_t val, int8_t modifier) { return val << modifier; };
        static int8_t shiftRight(int8_t val, int8_t modifier) { return val >> modifier; };
    };

    /* class to encapsulate modifying */
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

    /* temp solution - save data in memory*/
    std::vector<int8_t> sequence;

public:
    FileHandler(){};

    void setModifier(int8_t val) { modifier.setModifierValue(val); }

    void setXor() { modifier.setModifierFunc(ModifierFunctions::myXor); }
    void setAnd() { modifier.setModifierFunc(ModifierFunctions::myAnd); }
    void setOr() { modifier.setModifierFunc(ModifierFunctions::myOr); }
    void setShiftL() { modifier.setModifierFunc(ModifierFunctions::shiftLeft); }
    void setShiftR() { modifier.setModifierFunc(ModifierFunctions::shiftRight); }

    /* modify opened file */
    void processFile();

    void openFile(const std::string &fileName);
    void deleteFile(const std::string &fileName);
    void outFile(const std::string &fileName);
};

class FilesListHandler{
private:
    enum class actionCode {
        AND,
        XOR,
        OR,
        ShiftL,
        ShiftR
    };

    enum class ActionOnExist{
        replace,
        rename,
    };

    std::unordered_map<std::string, actionCode>actions{
        {"AND", actionCode::AND},
        {"OR", actionCode::OR},
        {"XOR", actionCode::XOR},
        {"Побитовый сдвиг влево", actionCode::ShiftL},
        {"Побитовый сдвиг вправо", actionCode::ShiftR}
    };

    std::unordered_map<std::string, ActionOnExist>actionsOnExist{
        {"Заменить", ActionOnExist::replace},
        {"Добавить с счетчиком", ActionOnExist::rename},
    };

    ActionOnExist actionOnExist;
    fs::path directoryPath;
    std::string maskString = "";
    bool needDelete = false;

    FileHandler singleFileHandler;

    /* get list of all files in dir[directoryPath] */
    std::unordered_set<std::string> listAllFilesInDir();

    /* convert mask in conditional myMask format to cpp regex*/
    std::string convertMaskToRegex(const std::string& mask);

    /* check mask by regex */
    bool checkMask(std::string mask, std::string str);

    // [TODO] think about path
    /* return unique name for folder with given file [fileName - file name include path to file] */
    std::string getUniqueName(const std::string& fileName);
public:
    std::vector<std::string> getActionsNames(){
        std::vector<std::string> result;
        for(auto kv: actions){
            result.push_back(kv.first);
        }
        return std::move(result);
    }

    std::vector<std::string> getActionsOnExistNames(){
        std::vector<std::string> result;
        for(auto kv: actionsOnExist){
            result.push_back(kv.first);
        }
        return std::move(result);
    }

    void setDir(const std::string &dirPath){
        directoryPath = dirPath;
    }

    void setMaskString(const std::string &mask){
        maskString = mask;
    }

    void setDeleteFlag(bool flag){
        needDelete = flag;
    }

    void setActionOnExist(const std::string& actionName){
        actionOnExist = actionsOnExist[actionName];
    }

    void setModifier(const std::string& val){
        singleFileHandler.setModifier(val != "" ? std::stoi(val) : 0);
    }

    void setModifyAction(const std::string& actionName){
        switch(actions[actionName]){
            case actionCode::AND:
                singleFileHandler.setAnd();
                break;
            case actionCode::XOR:
                singleFileHandler.setXor();
                break;
            case actionCode::OR:
                singleFileHandler.setOr();
                break;
            case actionCode::ShiftL:
                singleFileHandler.setShiftL();
                break;
            case actionCode::ShiftR:
                singleFileHandler.setShiftR();
                break;
        }
    }

    /* execute files processing [logFunc - lambda function to log] */
    void processFiles(std::function<void(const std::string&&)> logFunc);
};

class Core
{
public:
    /* I assumed more logic will be here*/
    Core(){
        handler = new FilesListHandler;
    };
    FilesListHandler* handler;
};

#endif // CORE_H
