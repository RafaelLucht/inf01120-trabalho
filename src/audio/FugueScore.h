// FugueScore.h
#ifndef FUGUE_SCORE_H
#define FUGUE_SCORE_H

#include "VoiceParser.h"
#include "RtMidi.h"
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>

class FugueScore {
private:
    std::vector<Voice> voices;
    AudioBPM bpm;

public:
    FugueScore() = default;

    void addVoice(Voice voice) { voices.push_back(std::move(voice)); }
    AudioBPM& getBpm() { return bpm; }
    const std::vector<Voice>& getVoices() const { return voices; }
    int getVoiceCount() const { return voices.size(); }

    void play() {
        RtMidiOut midi;
        midi.openPort(0);

        std::vector<std::thread> threads;
        std::mutex midiMutex;

        auto playVoice = [&](const Voice& voice) {
            AudioParameters params = voice.getParams();
            int channel = voice.getVoiceIndex() % 16;
            float beatDuration = 60000.0f / params.bpm.get();

            std::vector<unsigned char> progChange = {
                (unsigned char)(192 + channel),
                (unsigned char)params.inst.getVal()
            };
            std::vector<unsigned char> noteOn = {
                (unsigned char)(144 + channel),
                0, (unsigned char)params.vol.get()
            };
            std::vector<unsigned char> noteOff = {
                (unsigned char)(128 + channel),
                0, 0
            };

            if (voice.getDelay() > 0)
                std::this_thread::sleep_for(
                    std::chrono::milliseconds((int)(voice.getDelay() * beatDuration))
                );

            {
                std::lock_guard<std::mutex> lock(midiMutex);
                midi.sendMessage(&progChange);
            }

            for (const auto& event : voice.getEvents()) {
                NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());
                if (noteEvent) {
                    noteOn[1]  = (unsigned char)noteEvent->getNote(params);
                    noteOff[1] = noteOn[1];

                    {
                        std::lock_guard<std::mutex> lock(midiMutex);
                        midi.sendMessage(&noteOn);
                    }

                    std::this_thread::sleep_for(
                        std::chrono::milliseconds((int)beatDuration)
                    );

                    {
                        std::lock_guard<std::mutex> lock(midiMutex);
                        midi.sendMessage(&noteOff);
                    }
                } else {
                    event->runEvent(params);
                    progChange[1] = (unsigned char)params.inst.getVal();
                    {
                        std::lock_guard<std::mutex> lock(midiMutex);
                        midi.sendMessage(&progChange);
                    }
                    beatDuration = 60000.0f / params.bpm.get();
                    noteOn[2] = (unsigned char)params.vol.get();
                }
            }
        };

        for (const Voice& voice : voices) {
            threads.emplace_back(playVoice, std::ref(voice));
        }

        for (std::thread& t : threads) {
            if (t.joinable()) t.join();
        }
    }

    static FugueScore fromText(const std::string& text) {
        FugueScore score;
        std::vector<Voice> voices = VoiceParser::parse(text);
        for (auto& voice : voices)
            score.addVoice(std::move(voice));
        return score;
    }
};

#endif // FUGUE_SCORE_H