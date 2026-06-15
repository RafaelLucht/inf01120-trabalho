// VoiceParser.h
#ifndef VOICE_PARSER_H
#define VOICE_PARSER_H

#include "Voice.h"
#include "EventMapper.h"
#include <string>
#include <vector>

class VoiceParser {
private:
    static std::vector<std::string> splitLines(const std::string& text) {
        std::vector<std::string> lines;
        std::string current;
        for (char c : text) {
            if (c == '\n') {
                if (!current.empty()) lines.push_back(current);
                current.clear();
            } else {
                current += c;
            }
        }
        if (!current.empty()) lines.push_back(current);
        return lines;
    }

    static int parseDelay(std::string& line) {
        if (line.empty() || line[0] != '[') return 0;
        size_t end = line.find(']');
        if (end == std::string::npos) return 0;
        int delay = std::stoi(line.substr(1, end - 1));
        line = line.substr(end + 1);
        return delay;
    }

    static Voice parseLine(const std::string& line, int voiceIndex) {
        Voice voice(voiceIndex);
        std::string linecopy = line;
        int delay = parseDelay(linecopy);
        voice.setDelay(delay);

        int lastBaseNote = -1;
        std::unique_ptr<AudioEvent> event;

        for (int i = 0; i < (int)linecopy.size(); i++) {
            char c = linecopy[i];

            if (c == 'M' && i + 1 < (int)linecopy.size() && linecopy[i + 1] == 'b') {
                event = EventMapper::triggerEvent("Mb");
                i++;
            } else {
                event = EventMapper::triggerEvent(c);
            }

            if (event != nullptr) {
                NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());
                if (noteEvent) lastBaseNote = noteEvent->getBaseNote();
                voice.addEvent(std::move(event));
            } else if (lastBaseNote >= 0) {
                voice.addEvent(std::make_unique<NoteEvent>(lastBaseNote));
            }
        }
        return voice;
    }

public:
    static std::vector<Voice> parse(const std::string& text) {
        std::vector<Voice> voices;
        std::vector<std::string> lines = splitLines(text);
        for (int i = 0; i < (int)lines.size(); i++) {
            voices.push_back(parseLine(lines[i], i));
        }
        return voices;
    }
};

#endif // VOICE_PARSER_H