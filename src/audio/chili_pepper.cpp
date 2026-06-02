#include <iostream>
#include <string>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <memory>
#include <vector>
#include "RtMidi.h"
#include <thread>
#include <chrono>
#include <fstream>


class AudioVolume {
private:
	static constexpr short MIN_VOLUME = 0;
	static constexpr short MAX_VOLUME = 127;

	short defaultVolume = 100;

	bool muted = false;
	short vol = defaultVolume;
public:
	explicit AudioVolume(short defaultVolume) :
		defaultVolume(defaultVolume) {}
	void set(short vol) {
		if (vol < MIN_VOLUME || vol > MAX_VOLUME)
			throw std::out_of_range("Attempted setting invalid volume value");
		this->vol = vol;
	}
	void setClamped(short vol) {
		short real_set_value = std::clamp(vol, MIN_VOLUME, MAX_VOLUME);
		this->vol = real_set_value;
	}
	short get() const { return vol; }
	short getMin() const { return MIN_VOLUME; }
	short getMax() const { return MAX_VOLUME; }
	void resetToDefault() { set(defaultVolume); }
	void mute() { muted = true; }
	void unmute() { muted = false; }
	bool isMuted() const { return muted; }
};

class AudioBPM {
private:
	static constexpr float MIN_BPM = 30;
	static constexpr float MAX_BPM = 240;
	static constexpr float DEFAULT_BPM = 120;

	float bpm = DEFAULT_BPM;
public:
	void set(float bpm) {
		if (bpm < MIN_BPM || bpm > MAX_BPM)
			throw std::out_of_range("Attempted setting invalid BPM value");
		this->bpm = bpm;
	}
	void setClamped(float bpm) {
		float real_set_value = std::clamp(bpm, MIN_BPM, MAX_BPM);
		this->bpm = real_set_value;
	}
	float get() const { return bpm; }
	float getMin() const { return MIN_BPM; }
	float getMax() const { return MAX_BPM; }
	void resetToDefault() { set(DEFAULT_BPM); }
};

class AudioInstrument {
private:
	static constexpr short MIN_MIDI_VAL = 0;
	static constexpr short MAX_MIDI_VAL = 127;

	short defaultMidiVal = 0;

	static constexpr std::array<const char*, 128> MIDI_NAMES = {
		//Piano
		"Acoustic Grand Piano", "Bright Acoustic Piano", "Electric Grand Piano",
		"Honky-tonk Piano", "Electric Piano 1", "Electric Piano 2", "Harpsichord", "Clavi",
		//Chromatic Percussion
		"Celesta", "Glockenspiel", "Music Box", "Vibraphone", "Marimba",
		"Xylophone", "Tubular Bells", "Dulcimer",
		//Organ
		"Drawbar Organ", "Percussive Organ", "Rock Organ", "Church Organ",
		"Reed Organ", "Accordion", "Harmonica", "Tango Accordion",
		//Guitar
		"Acoustic Guitar (nylon)", "Acoustic Guitar (steel)", "Electric Guitar (jazz)",
		"Electric Guitar (clean)", "Electric Guitar (muted)", "Overdriven Guitar",
		"Distortion Guitar", "Guitar Harmonics",
		//Bass
		"Acoustic Bass", "Electric Bass (finger)", "Electric Bass (pick)", "Fretless Bass",
		"Slap Bass 1", "Slap Bass 2", "Synth Bass 1", "Synth Bass 2",
		//Strings
		"Violin", "Viola", "Cello", "Contrabass", "Tremolo Strings",
		"Pizzicato Strings", "Orchestral Harp", "Timpani",
		//Ensemble
		"String Ensemble 1", "String Ensemble 2", "SynthStrings 1", "SynthStrings 2",
		"Choir Aahs", "Voice Oohs", "Synth Voice", "Orchestra Hit",
		//Brass
		"Trumpet", "Trombone", "Tuba", "Muted Trumpet", "French Horn",
		"Brass Section", "SynthBrass 1", "SynthBrass 2",
		//Reed
		"Soprano Sax", "Alto Sax", "Tenor Sax", "Baritone Sax", "Oboe",
		"English Horn", "Bassoon", "Clarinet",
		//Pipe
		"Piccolo", "Flute", "Recorder", "Pan Flute", "Blown Bottle",
		"Shakuhachi", "Whistle", "Ocarina",
		//Synth Lead
		"Lead 1 (square)", "Lead 2 (sawtooth)", "Lead 3 (calliope)", "Lead 4 (chiff)",
		"Lead 5 (charang)", "Lead 6 (voice)", "Lead 7 (fifths)", "Lead 8 (bass + lead)",
		//Synth Pad
		"Pad 1 (new age)", "Pad 2 (warm)", "Pad 3 (polysynth)", "Pad 4 (choir)",
		"Pad 5 (bowed)", "Pad 6 (metallic)", "Pad 7 (halo)", "Pad 8 (sweep)",
		//Synth Effects
		"FX 1 (rain)", "FX 2 (soundtrack)", "FX 3 (crystal)", "FX 4 (atmosphere)",
		"FX 5 (brightness)", "FX 6 (goblins)", "FX 7 (echoes)", "FX 8 (sci-fi)",
		//Ethnic
		"Sitar", "Banjo", "Shamisen", "Koto", "Kalimba", "Bag pipe", "Fiddle", "Shanai",
		//Percussive
		"Tinkle Bell", "Agogo", "Steel Drums", "Woodblock", "Taiko Drum",
		"Melodic Tom", "Synth Drum", "Reverse Cymbal",
		//Sound Effects
		"Guitar Fret Noise", "Breath Noise", "Seashore", "Bird Tweet",
		"Telephone Ring", "Helicopter", "Applause", "Gunshot"
	};

	static std::string nameMIDI(short midi_val) {
		return MIDI_NAMES[midi_val];
	}

	struct MIDI_Instrument {
		short midi_val;
		std::string inst_name;
	};
	MIDI_Instrument inst = {defaultMidiVal, nameMIDI(defaultMidiVal)};

public:
	explicit AudioInstrument(short defaultMidiVal = 0) :
		defaultMidiVal(defaultMidiVal) {}
	void set(short midi_val) {
		if (midi_val < MIN_MIDI_VAL || midi_val > MAX_MIDI_VAL)
			throw std::out_of_range("Attempted setting invalid MIDI instrument value");
		inst.midi_val = midi_val;
		inst.inst_name = nameMIDI(midi_val);
	}
	void setClamped(short midi_val) {
		short real_set_val = std::clamp(midi_val, MIN_MIDI_VAL, MAX_MIDI_VAL);
		inst.midi_val = real_set_val;
		inst.inst_name = nameMIDI(real_set_val);
	}
	short getVal() const { return inst.midi_val; }
	short getMin() const { return MIN_MIDI_VAL; }
	short getMax() const { return MAX_MIDI_VAL; }
	std::string getName() const { return inst.inst_name; }
	void resetToDefault() { set(defaultMidiVal); }
};

class AudioOctave {
private:
	static constexpr short MIN_OCTAVE = 1;
	static constexpr short MAX_OCTAVE = 9;

	short defaultOctave = 6;

	static std::string nameOct(short oct) {
		std::string num_suffix;
		switch (oct) {
			case 1: num_suffix = "st"; break;
			case 2: num_suffix = "nd"; break;
			case 3: num_suffix = "rd"; break;
			default: num_suffix = "th";
		}
		return std::to_string(oct) + num_suffix;
	}

	struct Octave {
		short oct_val;
		std::string oct_name;
	};
	Octave oct = {defaultOctave, nameOct(defaultOctave)};

public:
	explicit AudioOctave(short defaultOctave) :
		defaultOctave(defaultOctave) {}
	void set(short oct_val) {
		if (oct_val < MIN_OCTAVE || oct_val > MAX_OCTAVE)
			throw std::out_of_range("Attempted setting invalid octave value");
		oct.oct_val = oct_val;
		oct.oct_name = nameOct(oct_val);
	}
	void setClamped(short oct_val) {
		short real_set_value = std::clamp(oct_val, MIN_OCTAVE, MAX_OCTAVE);
		oct.oct_val = real_set_value;
		oct.oct_name = nameOct(real_set_value);
	}
	short getVal() const { return oct.oct_val; }
	short getMin() const { return MIN_OCTAVE; }
	short getMax() const { return MAX_OCTAVE; }
	std::string getName() { return oct.oct_name; }
	void resetToDefault() { set(defaultOctave); }
};

class AudioParameters {
public:
	AudioVolume vol;
	AudioBPM bpm;
	AudioInstrument inst;
	AudioOctave oct;

	// corrigido: passa defaultMidiVal=0 para inst
	AudioParameters() : vol(100), inst(0), oct(6) {}

	void resetToDefaultAll() {
		vol.resetToDefault();
		bpm.resetToDefault();
		inst.resetToDefault();
		oct.resetToDefault();
	}
};

class AudioEvent {
public:
	virtual ~AudioEvent() = default;
	virtual void runEvent(AudioParameters &params) = 0;
};

class InstrumentChangeEvent : public AudioEvent {
public:
	enum class ChangeMode { ABSOLUTE, RELATIVE };
private:
	short midi_val;
	ChangeMode mode;
public:
	explicit InstrumentChangeEvent(short midi_val, ChangeMode mode = ChangeMode::ABSOLUTE) :
		midi_val(midi_val), mode(mode) {}
	void runEvent(AudioParameters &params) override {
		switch (mode) {
			case ChangeMode::ABSOLUTE: params.inst.set(midi_val); break;
			case ChangeMode::RELATIVE: params.inst.setClamped(params.inst.getVal() + midi_val); break;
		}
	}
};

class OctaveChangeEvent : public AudioEvent {
private:
	short octShift;
public:
	explicit OctaveChangeEvent(short octShift) : octShift(octShift) {}
	void runEvent(AudioParameters &params) override {
		// TODO: discutir com Johann — resetar vs clampar quando sai do range
		short oct = params.oct.getVal() + octShift;
		if (oct > params.oct.getMax() || oct < params.oct.getMin())
			params.oct.resetToDefault();
		else
			params.oct.set(oct);
	}
};

class BpmChangeEvent : public AudioEvent {
private:
	float bpmShift;
public:
	explicit BpmChangeEvent(float bpmShift) : bpmShift(bpmShift) {}
	void runEvent(AudioParameters &params) override {
		params.bpm.setClamped(params.bpm.get() + bpmShift);
	}
};

class VolumeChangeEvent : public AudioEvent {
public:
	// TODO: discutir com Johann — vol*2 fixo vs shift configurável
	void runEvent(AudioParameters &params) override {
		params.vol.setClamped(params.vol.get() * 2);
	}
};

class NoteEvent : public AudioEvent {
private:
    int baseNote; // nota MIDI base (oitava 5)
public:
    explicit NoteEvent(int baseNote) : baseNote(baseNote) {}

    void runEvent(AudioParameters& params) override {
        // NoteEvent não muda params — só marca que há uma nota
        // a nota real é calculada pelo play() considerando a oitava atual
    }

    int getNote(const AudioParameters& params) const {
        int nota = baseNote + (params.oct.getVal() - 5) * 12;
        return std::clamp(nota, 0, 127);
    }
};

class EventMapper {
private:
	static bool isAnEvenDigit(char c) {
		return (c >= '0' && c <= '9') && ((c & 1) == 0);
	}
public:
	static std::unique_ptr<AudioEvent> triggerEvent(char eventKey) {
		if (isAnEvenDigit(eventKey))
			return std::make_unique<InstrumentChangeEvent>(eventKey - '0', InstrumentChangeEvent::ChangeMode::RELATIVE);
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
			case 'A': return std::make_unique<NoteEvent>(69); // Lá
			case 'B': return std::make_unique<NoteEvent>(71); // Si
			case 'C': return std::make_unique<NoteEvent>(60); // Dó
			case 'D': return std::make_unique<NoteEvent>(62); // Ré
			case 'E': return std::make_unique<NoteEvent>(64); // Mi
			case 'F': return std::make_unique<NoteEvent>(65); // Fá
			case 'G': return std::make_unique<NoteEvent>(67); // Sol
			case 'H': return std::make_unique<NoteEvent>(70); // Si bemol
		}
		return nullptr; // corrigido: evita comportamento indefinido
	}
};

class Voice {
private:
	int voiceIndex;
	int delay;
	AudioParameters params;
	std::vector<std::unique_ptr<AudioEvent>> events;

	static constexpr std::array<short, 4> BASE_OCTAVES     = {6, 5, 4, 3};
	static constexpr std::array<short, 4> BASE_VOLUMES     = {100, 80, 60, 40};
	static constexpr std::array<short, 4> BASE_INSTRUMENTS = {6, 20, 0, 70};

public:
	explicit Voice(int voiceIndex) :
		voiceIndex(voiceIndex), delay(0), params()
	{
		int idx = voiceIndex % 4;
		params.oct.set(BASE_OCTAVES[idx]);
		params.vol.set(BASE_VOLUMES[idx]);
		params.inst.set(BASE_INSTRUMENTS[idx]);
	}

	void setDelay(int n) { delay = n; }
	void addEvent(std::unique_ptr<AudioEvent> event) {
		events.push_back(std::move(event));
	}

	// corrigido: getters marcados como const
	int getDelay()                    const { return delay; }
	int getVoiceIndex()               const { return voiceIndex; }
	const AudioParameters& getParams() const { return params; }
	const std::vector<std::unique_ptr<AudioEvent>>& getEvents() const {
		return events;
	}
};

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
		for (char c : linecopy) {
			auto event = EventMapper::triggerEvent(c);
			if (event != nullptr)
				voice.addEvent(std::move(event));
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

class FugueScore {
private:
	std::vector<Voice> voices;
	AudioBPM bpm;

public:
	FugueScore() = default;

	void addVoice(Voice voice) { voices.push_back(std::move(voice)); }
	AudioBPM& getBpm() { return bpm; }
	const std::vector<Voice>& getVoices() { return voices; }
	int getVoiceCount() { return voices.size(); }

	void play() {
		RtMidiOut midi;
		midi.openPort(0);

		float beatDuration = 60000.0f / bpm.get();

		for (const Voice& voice : voices) {
			std::this_thread::sleep_for(
				std::chrono::milliseconds((int)(voice.getDelay() * beatDuration))
			);

			AudioParameters params = voice.getParams();
			int channel = voice.getVoiceIndex() % 16;

			// define instrumento no canal
			std::vector<unsigned char> progChange = {
				(unsigned char)(192 + channel),
				(unsigned char)params.inst.getVal()
			};
			midi.sendMessage(&progChange);

			// toca os eventos
			for (const auto& event : voice.getEvents()) {
				// tenta converter para NoteEvent
				NoteEvent* noteEvent = dynamic_cast<NoteEvent*>(event.get());

				if (noteEvent) {
					// é uma nota — toca
					int nota = noteEvent->getNote(params);
					std::vector<unsigned char> noteOn  = {(unsigned char)(144 + channel), (unsigned char)nota, (unsigned char)params.vol.get()};
					std::vector<unsigned char> noteOff = {(unsigned char)(128 + channel), (unsigned char)nota, 0};

					midi.sendMessage(&noteOn);
					std::this_thread::sleep_for(std::chrono::milliseconds((int)beatDuration));
					midi.sendMessage(&noteOff);
				} else {
					// é um evento de controle — só executa, não toca nota
					event->runEvent(params);
				}
			}
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

class MidiExporter {
private:
    // escreve um inteiro de 4 bytes em big-endian no arquivo
    static void writeInt32(std::ofstream& f, uint32_t val) {
        f.put((val >> 24) & 0xFF);
        f.put((val >> 16) & 0xFF);
        f.put((val >>  8) & 0xFF);
        f.put((val >>  0) & 0xFF);
    }

    // escreve um inteiro de 2 bytes em big-endian no arquivo
    static void writeInt16(std::ofstream& f, uint16_t val) {
        f.put((val >> 8) & 0xFF);
        f.put((val >> 0) & 0xFF);
    }

    // escreve um número em formato variable-length (padrão MIDI para delta time)
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

    // gera os bytes de uma trilha MIDI a partir de uma Voice
    static std::vector<unsigned char> buildTrack(const Voice& voice, float beatDuration) {
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

public:
    static void exportToFile(FugueScore& score, const std::string& filename) {
        std::ofstream f(filename, std::ios::binary);
        if (!f.is_open())
            throw std::runtime_error("Nao foi possivel abrir o arquivo: " + filename);

        int numTracks = score.getVoiceCount();
        float beatDuration = 60000.0f / score.getBpm().get();

        // header chunk
        f.write("MThd", 4);        // identificador
        writeInt32(f, 6);          // tamanho do header sempre 6
        writeInt16(f, 1);          // formato 1 = múltiplas trilhas simultâneas
        writeInt16(f, numTracks);  // número de trilhas
        writeInt16(f, 480);        // 480 ticks por beat

        // uma trilha por voz
        for (const Voice& voice : score.getVoices()) {
            std::vector<unsigned char> track = buildTrack(voice, beatDuration);

            f.write("MTrk", 4);
            writeInt32(f, track.size());
            f.write((char*)track.data(), track.size());
        }

        f.close();
    }
};

int main() {
    std::cout << "=== Gerador Musical ===" << std::endl;
    std::cout << "Digite o texto (linha vazia para terminar):" << std::endl;
    std::cout << "Use [n] no inicio de cada linha para delay." << std::endl;
    std::cout << "Notas: A=La B=Si C=Do D=Re E=Mi F=Fa G=Sol H=SiBemol" << std::endl;
    std::cout << "--------------------------------------" << std::endl;

    std::string texto = "";
    std::string linha;

    while (true) {
        std::getline(std::cin, linha);
        if (linha.empty()) break;
        texto += linha + "\n";
    }

    if (texto.empty()) {
        std::cout << "Nenhum texto inserido." << std::endl;
        return 0;
    }

    std::cout << "\nProcessando " << std::count(texto.begin(), texto.end(), '\n') << " vozes..." << std::endl;

    FugueScore score = FugueScore::fromText(texto);

    std::cout << "Tocando..." << std::endl;
    score.play();

    std::cout << "\nSalvar arquivo MIDI? (s/n): ";
    char resp;
    std::cin >> resp;
    if (resp == 's' || resp == 'S') {
        std::cout << "Nome do arquivo (sem extensao): ";
        std::string nome;
        std::cin >> nome;
        MidiExporter::exportToFile(score, nome + ".mid");
        std::cout << "Arquivo " << nome << ".mid salvo!" << std::endl;
    }

    std::cout << "Fim." << std::endl;
    return 0;
}