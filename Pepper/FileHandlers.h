#pragma once

#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <cstdint>

class Voice;
class FugueScore;

class FileManager {
public:
    static std::string loadText(const std::string& filename);

    static void saveText(const std::string& filename, const std::string& content);
};

class MidiExporter {
private:
    // escreve um inteiro de 4 bytes em big-endian no arquivo
    static void writeInt32(std::ofstream& f, uint32_t val);

    // escreve um inteiro de 2 bytes em big-endian no arquivo
    static void writeInt16(std::ofstream& f, uint16_t val);
    
    // escreve um número em formato variable-length (padrão MIDI para delta time)
    static void writeVarLen(std::ofstream& f, uint32_t val);

    // gera os bytes de uma trilha MIDI a partir de uma Voice
    static std::vector<unsigned char> buildTrack(const Voice& voice, float beatDuration);
    
public:
    static void exportToFile(FugueScore& score, const std::string& filename);
};
