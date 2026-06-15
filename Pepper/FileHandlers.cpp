#include "FileHandlers.h"
#include "AudioParameters.h"
#include "AudioSynth.h"
#include "AudioEvents.h"

#include <string>
#include <stdexcept>
#include <fstream>
#include <vector>
#include <algorithm>
#include <cstdint>

// --- File Manager ---
std::string FileManager::loadText(const std::string& filename) {
	std::ifstream f(filename);
	if (!f.is_open())
		throw std::runtime_error("Unable to open file: " + filename);
		
	std::string content	(
		(std::istreambuf_iterator<char>(f)),
		std::istreambuf_iterator<char>()
	);
	f.close();
	return content;
}

void FileManager::saveText(const std::string& filename, const std::string& content) {
	std::ofstream f(filename);
	if (!f.is_open())
		throw std::runtime_error("Unable to save file: " + filename);
		
	f << content;
	f.close();
}


// --- Midi Exporter ---
// escreve um inteiro de 4 bytes em big-endian no arquivo
void MidiExporter::writeInt32(std::ofstream& f, uint32_t val) {
	f.put((val >> 24) & 0xFF);
	f.put((val >> 16) & 0xFF);
	f.put((val >>  8) & 0xFF);
	f.put((val >>  0) & 0xFF);
}

// escreve um inteiro de 2 bytes em big-endian no arquivo
void MidiExporter::writeInt16(std::ofstream& f, uint16_t val) {
	f.put((val >> 8) & 0xFF);
	f.put((val >> 0) & 0xFF);
}

// escreve um número em formato variable-length (padrão MIDI para delta time)
void MidiExporter::writeVarLen(std::ofstream& f, uint32_t val) {
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

// gera os bytes de uma trilha MIDI a partir de uma Voice
std::vector<unsigned char> MidiExporter::buildTrack(const Voice& voice, float beatDuration) {
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

	// delay inicial
	pushVarLen(voice.getDelay() * 480);
	track.push_back(0xC0 | channel);
	track.push_back(params.inst.getVal());

	for (const auto& event : voice.getEvents()) {
		NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());
		if (noteEvent) {
			int nota = noteEvent->getNote(params);
			nota = std::clamp(nota, 0, 127);

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

	// end of track
	pushVarLen(0);
	track.push_back(0xFF);
	track.push_back(0x2F);
	track.push_back(0x00);

	return track;
}

void MidiExporter::exportToFile(FugueScore& score, const std::string& filename) {
	std::ofstream f(filename, std::ios::binary);
	if (!f.is_open())
		throw std::runtime_error("Unable to open file: " + filename);

	int numTracks = score.getVoiceCount();

	// header chunk
	f.write("MThd", 4);        // identificador
	writeInt32(f, 6);          // tamanho do header sempre 6
	writeInt16(f, 1);          // formato 1 = múltiplas trilhas simultâneas
	writeInt16(f, numTracks);  // número de trilhas
	writeInt16(f, 480);        // 480 ticks por beat

	// uma trilha por voz
	for (const Voice& voice : score.getVoices()) {
		std::vector<unsigned char> track = buildTrack	(
			voice,
			60000.0f / (voice.getParams().bpm.get())
		);

		f.write("MTrk", 4);
		writeInt32(f, track.size());
		f.write((char*)track.data(), track.size());
	}

	f.close();
}
