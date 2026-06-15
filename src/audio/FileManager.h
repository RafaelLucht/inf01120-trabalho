// FileManager.h
#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <string>
#include <fstream>
#include <stdexcept>

class FileManager {
public:
    static std::string loadText(const std::string& filename) {
        std::ifstream f(filename);
        if (!f.is_open())
            throw std::runtime_error("Nao foi possivel abrir o arquivo: " + filename);
        std::string content((std::istreambuf_iterator<char>(f)),
                             std::istreambuf_iterator<char>());
        f.close();
        return content;
    }

    static void saveText(const std::string& filename, const std::string& content) {
        std::ofstream f(filename);
        if (!f.is_open())
            throw std::runtime_error("Nao foi possivel salvar o arquivo: " + filename);
        f << content;
        f.close();
    }
};

#endif // FILE_MANAGER_H