// EventMapper.h
#ifndef EVENT_MAPPER_H
#define EVENT_MAPPER_H

#include "AudioEvent.h"
#include <memory>

class EventMapper {
private:
    static bool isAnEvenDigit(char c) {
        return (c >= '0' && c <= '9') && ((c & 1) == 0);
    }
public:
    static std::unique_ptr<AudioEvent> triggerEvent(char eventKey) {
        if (isAnEvenDigit(eventKey))
            return std::make_unique<InstrumentChangeEvent>(
                eventKey - '0', InstrumentChangeEvent::ChangeMode::RELATIVE
            );
        switch (eventKey) {
            case '!': return std::make_unique<InstrumentChangeEvent>(24);
            case ';': return std::make_unique<InstrumentChangeEvent>(15);
            case ',': return std::make_unique<InstrumentChangeEvent>(114);
            case 'o':
            case 'i':
            case 'u': return std::make_unique<InstrumentChangeEvent>(110);
            case '?':
            case '.': return std::make_unique<OctaveChangeEvent>(1);
            case 'V': return std::make_unique<OctaveChangeEvent>(-1);
            case '<': return std::make_unique<BpmChangeEvent>(-10);
            case '>': return std::make_unique<BpmChangeEvent>(10);
            case ' ': return std::make_unique<VolumeChangeEvent>();
            case 'A': return std::make_unique<NoteEvent>(69);
            case 'B': return std::make_unique<NoteEvent>(71);
            case 'C': return std::make_unique<NoteEvent>(60);
            case 'D': return std::make_unique<NoteEvent>(62);
            case 'E': return std::make_unique<NoteEvent>(64);
            case 'F': return std::make_unique<NoteEvent>(65);
            case 'G': return std::make_unique<NoteEvent>(67);
            case 'H': return std::make_unique<NoteEvent>(70);
        }
        return nullptr;
    }

    static std::unique_ptr<AudioEvent> triggerEvent(const std::string& eventKey) {
        if (eventKey.empty()) return nullptr;
        if (eventKey == "Mb") return std::make_unique<NoteEvent>(63);
        return nullptr;
    }
};

#endif // EVENT_MAPPER_H