#include "AudioSynth.h"
#include "AudioParameters.h"
#include "AudioEvents.h"
#include "VoiceParser.h"
#include "RtMidi.h"

#include <memory>
#include <utility>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <chrono>

// --- Voice ---
Voice::Voice(int voiceIndex) :
	voiceIndex(voiceIndex), delay(0), params()	{
	int idx = voiceIndex % 4;
	params.oct.set(BASE_OCTAVES[idx]);
	params.vol.set(BASE_VOLUMES[idx]);
	params.inst.set(BASE_INSTRUMENTS[idx]);
}

void Voice::setDelay(int n)	{
	delay = n;
}
void Voice::addEvent(std::unique_ptr<AudioEvent> event) {
	events.push_back(std::move(event));
}

int Voice::getDelay() const	{
	return delay;
}
int Voice::getVoiceIndex() const	{
	return voiceIndex;
}
const AudioParameters& Voice::getParams() const	{
	return params;
}
const std::vector<std::unique_ptr<AudioEvent>>& Voice::getEvents() const {
	return events;
}


// --- Fugue Score ---
void FugueScore::addVoice(Voice voice)	{
	voices.push_back(std::move(voice));
}

const std::vector<Voice> &FugueScore::getVoices() const	{
	return voices;
}
int FugueScore::getVoiceCount() const	{
	return voices.size();
}

void FugueScore::play() {
	RtMidiOut midi;
	midi.openPort(0);

	// --- Threads para as vozes; mutex para previnir conflito de dados ---
	std::vector<std::thread> threads;
	std::mutex midiMutex;
	
	// --- Cada voz em sua thread reproduz som com essa função helper ---
	auto playVoice = [&](const Voice& voice) {
		
		// --- Inicializa parâmetros de voz ---
		AudioParameters params = voice.getParams();
		int channel = voice.getVoiceIndex() % 16;
        float beatDuration = 60000.0f / params.bpm.get();
	
		// --- Inicializa vetores MIDI ---
		std::vector<unsigned char> progChange =	{
			(unsigned char)(192 + channel),
			(unsigned char)params.inst.getVal()
		};
		std::vector<unsigned char> noteOn =	{
			(unsigned char)(144 + channel),
			0, (unsigned char)params.vol.get()
		};
		std::vector<unsigned char> noteOff =	{
			(unsigned char)(128 + channel),
			0, 0 
		};
		
		// --- Atraso inicial da voz ---
        if (voice.getDelay() > 0)
            std::this_thread::sleep_for(std::chrono::milliseconds((int)(voice.getDelay() * beatDuration)));
            
		// --- Configura instrumento inicial; fluxo de dados da thread protegido por mutex ---
		{
			std::lock_guard<std::mutex> lock(midiMutex);
			midi.sendMessage(&progChange);
		}
		
		
		// --- Loop de áudio da voz ---
		for (const auto &event : voice.getEvents()) {
			NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());
			if (noteEvent) {
				
				noteOn[1] = (unsigned char)noteEvent->getNote(params);
				noteOff[1] = noteOn[1];
				
				// --- Inicia reprodução da nota ---
				{
					std::lock_guard<std::mutex> lock(midiMutex);
					midi.sendMessage(&noteOn);
				}
				
				// --- Espera pela duração da nota ---
				std::this_thread::sleep_for(std::chrono::milliseconds((int)beatDuration));
				
				// --- Desativa reprodução da nota ---
				{
				std::lock_guard<std::mutex> lock(midiMutex);
				midi.sendMessage(&noteOff);
				}
			}
			else {
				// --- Evento diferente de nota; provavelmente altera parâmetros ---
				event->runEvent(params);
				
				// --- Atualiza parâmetros, caso tenham mudado ---
				progChange[1] = (unsigned char)params.inst.getVal();
				{
	                std::lock_guard<std::mutex> lock(midiMutex);
	                midi.sendMessage(&progChange);
	            }
				beatDuration = 60000.0f / params.bpm.get();
				noteOn[2]  = (unsigned char)params.vol.get();
			}
		}
	};
	// --- Dispara as threads de vozes ---
    for (const Voice &voice : voices) {
        threads.emplace_back(playVoice, std::ref(voice));
    }
    
    // --- Espera todas threads terminarem antes de sair---
    for (std::thread& t : threads) {
        if (t.joinable()) {
            t.join();
        }
	}
}
FugueScore FugueScore::fromText(const std::string& text) {
	FugueScore score;
	std::vector<Voice> voices = VoiceParser::parse(text);
	for (auto& voice : voices)
		score.addVoice(std::move(voice));
	return score;
}
