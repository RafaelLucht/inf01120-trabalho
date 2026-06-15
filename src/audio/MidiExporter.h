// MidiExporter.h
#ifndef MIDI_EXPORTER_H
#define MIDI_EXPORTER_H

#include "FugueScore.h"
#include <fstream>
#include <stdexcept>

class MidiExporter {
private:
    static void writeInt32(std::ofstream& f, uint32_t val) {
        f.put((val >> 24) & 0xFF);
        f.put((val >> 16) & 0xFF);
        f.put((val >>  8) & 0xFF);
        f.put((val >>  0) & 0xFF);
    }

    static void writeInt16(std::ofstream& f, uint16_t val) {
        f.put((val >> 8) & 0xFF);
        f.put((val >> 0) & 0xFF);
    }

    static void writeVarLen(std::ofstream& f, uint32_t val) {
        if (val < 128) {
            f.put(val);
        } else if (val < 16384) {
            f.put((val >> 7) | 0x80);
            f.put(val & 0x7F);
        } else {
            f.put((val >> 14) | 0x80);
            f.put(((val >> 7) & 0x7F) | 0x80);
            f.put(val & 0x7F);
        }
    }

    static std::vector<unsigned char> buildTrack(const Voice& voice) {
        std::vector<unsigned char> track;
        int channel = voice.getVoiceIndex() % 16;
        AudioParameters params = voice.getParams();

        auto pushVarLen = [&](uint32_t val) {
            if (val < 128) {
                track.push_back(val);
            } else if (val < 16384) {
                track.push_back((val >> 7) | 0x80);
                track.push_back(val & 0x7F);
            } else {
                track.push_back((val >> 14) | 0x80);
                track.push_back(((val >> 7) & 0x7F) | 0x80);
                track.push_back(val & 0x7F);
            }
        };

        pushVarLen(voice.getDelay() * 480);
        track.push_back(0xC0 | channel);
        track.push_back(params.inst.getVal());

        for (const auto& event : voice.getEvents()) {
            NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());
            if (noteEvent) {
                int nota = std::clamp(noteEvent->getNote(params), 0, 127);

                pushVarLen(0);
                track.push_back(0x90 | channel);
                track.push_back((unsigned char)nota);
                track.push_back((unsigned char)params.vol.get());

                pushVarLen(480);
                track.push_back(0x80 | channel);
                track.push_back((unsigned char)nota);
                track.push_back(0);
            } else {
                event->runEvent(params);
            }
        }

        pushVarLen(0);
        track.push_back(0xFF);
        track.push_back(0x2F);
        track.push_back(0x00);

        return track;
    }

public:
    static void exportToFile(FugueScore& score, const std::string& filename) {
        std::ofstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("Nao foi possivel abrir o arquivo: " + filename);

        int numTracks = score.getVoiceCount();
        float beatDuration = 60000.0f / score.getBpm().get();

        f.write("MThd", 4);
        writeInt32(f, 6);
        writeInt16(f, 1);
        writeInt16(f, numTracks);
        writeInt16(f, 480);

        for (const Voice& voice : score.getVoices()) {
            std::vector<unsigned char> track = buildTrack(voice);
            f.write("MTrk", 4);
            writeInt32(f, track.size());
            f.write((char*)track.data(), track.size());
        }

        f.close();
    }
};

#endif // MIDI_EXPORTER_H