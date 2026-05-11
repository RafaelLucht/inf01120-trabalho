#include <iostream>
#include <string>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <memory>


class AudioVolume	{
private:
	static constexpr short MIN_VOLUME = 0;
	static constexpr short MAX_VOLUME = 100;
	static constexpr short DEFAULT_VOLUME = 50;
	
	bool muted = false;
	short vol = DEFAULT_VOLUME;
public:
	void set(short vol)	{
		if (vol < MIN_VOLUME || vol > MAX_VOLUME)
			throw std::out_of_range("Attempted setting invalid volume value");
		this->vol = vol;
	}
	void setClamped(short vol)	{
		short real_set_value = std::clamp(vol, MIN_VOLUME, MAX_VOLUME);
		this->vol = real_set_value;
	}
	short get()	{
		return vol;
	}
	short getMin()	{
		return MIN_VOLUME;
	}
	short getMax()	{
		return MAX_VOLUME;
	}
	void resetToDefault()	{
		set(DEFAULT_VOLUME);
	}
	void mute()	{
		muted = true;
	}
	void unmute()	{
		muted = false;
	}
	bool isMuted()	{
		return muted;
	}
};
class AudioBPM	{
private:
	static constexpr float MIN_BPM = 30;
	static constexpr float MAX_BPM = 240;
	static constexpr float DEFAULT_BPM = 50;
	
	float bpm = DEFAULT_BPM;
public:
	void set(float bpm)	{
		if (bpm < MIN_BPM || bpm > MAX_BPM)
			throw std::out_of_range("Attempted setting invalid BPM value");
		this->bpm = bpm;
	}
	void setClamped(float bpm)	{
		float real_set_value = std::clamp(bpm, MIN_BPM, MAX_BPM);
		this->bpm = real_set_value;
	}
	float get()	{
		return bpm;
	}
	float getMin()	{
		return MIN_BPM;
	}
	float getMax()	{
		return MAX_BPM;
	}
	void resetToDefault()	{
		set(DEFAULT_BPM);
	}
};
class AudioInstrument	{
private:
	static constexpr short MIN_MIDI_VAL = 0;
	static constexpr short MAX_MIDI_VAL = 127;
	static constexpr short DEFAULT_MIDI_VAL = 27;
	static constexpr std::array<const char*, 128> MIDI_NAMES = {
	//Piano
		"Acoustic Grand Piano",
		"Bright Acoustic Piano",
		"Electric Grand Piano",
		"Honky-tonk Piano",
		"Electric Piano 1",
		"Electric Piano 2",
		"Harpsichord",
		"Clavi",
	//Chromatic Percussion
		"Celesta",
		"Glockenspiel",
		"Music Box",
		"Vibraphone",
		"Marimba",
		"Xylophone",
		"Tubular Bells",
		"Dulcimer",
	//Organ
		"Drawbar Organ",
		"Percussive Organ",
		"Rock Organ",
		"Church Organ",
		"Reed Organ",
		"Accordion",
		"Harmonica",
		"Tango Accordion",
	//Guitar
		"Acoustic Guitar (nylon)",
		"Acoustic Guitar (steel)",
		"Electric Guitar (jazz)",
		"Electric Guitar (clean)",
		"Electric Guitar (muted)",
		"Overdriven Guitar",
		"Distortion Guitar",
		"Guitar Harmonics",
	//Bass
		"Acoustic Bass",
		"Electric Bass (finger)",
		"Electric Bass (pick)",
		"Fretless Bass",
		"Slap Bass 1",
		"Slap Bass 2",
		"Synth Bass 1",
		"Synth Bass 2",
	//Strings
		"Violin",
		"Viola",
		"Cello",
		"Contrabass",
		"Tremolo Strings",
		"Pizzicato Strings",
		"Orchestral Harp",
		"Timpani",
	//Ensemble
		"String Ensemble 1",
		"String Ensemble 2",
		"SynthStrings 1",
		"SynthStrings 2",
		"Choir Aahs",
		"Voice Oohs",
		"Synth Voice",
		"Orchestra Hit",
	//Brass
		"Trumpet",
		"Trombone",
		"Tuba",
		"Muted Trumpet",
		"French Horn",
		"Brass Section",
		"SynthBrass 1",
		"SynthBrass 2",
	//Reed
		"Soprano Sax",
		"Alto Sax",
		"Tenor Sax",
		"Baritone Sax",
		"Oboe",
		"English Horn",
		"Bassoon",
		"Clarinet",
	//Pipe
		"Piccolo",
		"Flute",
		"Recorder",
		"Pan Flute",
		"Blown Bottle",
		"Shakuhachi",
		"Whistle",
		"Ocarina",
	//Synth Lead
		"Lead 1 (square)",
		"Lead 2 (sawtooth)",
		"Lead 3 (calliope)",
		"Lead 4 (chiff)",
		"Lead 5 (charang)",
		"Lead 6 (voice)",
		"Lead 7 (fifths)",
		"Lead 8 (bass + lead)",
	//Synth Pad
		"Pad 1 (new age)",
		"Pad 2 (warm)",
		"Pad 3 (polysynth)",
		"Pad 4 (choir)",
		"Pad 5 (bowed)",
		"Pad 6 (metallic)",
		"Pad 7 (halo)",
		"Pad 8 (sweep)",
	//Synth Effects
		"FX 1 (rain)",
		"FX 2 (soundtrack)",
		"FX 3 (crystal)",
		"FX 4 (atmosphere)",
		"FX 5 (brightness)",
		"FX 6 (goblins)",
		"FX 7 (echoes)",
		"FX 8 (sci-fi)",
	//Ethnic
		"Sitar",
		"Banjo",
		"Shamisen",
		"Koto",
		"Kalimba",
		"Bag pipe",
		"Fiddle",
		"Shanai",
	//Percussive
		"Tinkle Bell",
		"Agogo",
		"Steel Drums",
		"Woodblock",
		"Taiko Drum",
		"Melodic Tom",
		"Synth Drum",
		"Reverse Cymbal",
	//Sound Effects
		"Guitar Fret Noise",
		"Breath Noise",
		"Seashore",
		"Bird Tweet",
		"Telephone Ring",
		"Helicopter",
		"Applause",
		"Gunshot"
	};

	static std::string nameMIDI(short midi_val)	{
		return MIDI_NAMES[midi_val];
		}
	
	struct MIDI_Instrument	{
		short midi_val;
		std::string inst_name;
	};
	MIDI_Instrument inst = {DEFAULT_MIDI_VAL, nameMIDI(DEFAULT_MIDI_VAL)};
public:
	void set(short midi_val)	{
		if (midi_val < MIN_MIDI_VAL || midi_val > MAX_MIDI_VAL)
			throw std::out_of_range("Attempted setting invalid MIDI instrument value");
		inst.midi_val = midi_val;
		inst.inst_name = nameMIDI(midi_val);
	}
	void setClamped(short midi_val)	{
		short real_set_val = std::clamp(midi_val, MIN_MIDI_VAL, MAX_MIDI_VAL);
		inst.midi_val = real_set_val;
		inst.inst_name = nameMIDI(real_set_val);
	}
	short getVal()	{
		return inst.midi_val;
	}
	short getMin()	{
		return MIN_MIDI_VAL;
	}
	short getMax()	{
		return MAX_MIDI_VAL;
	}
	std::string getName()	{
		return inst.inst_name;
	}
	void resetToDefault()	{
		set(DEFAULT_MIDI_VAL);
	}
};
class AudioOctave	{
private:
	static constexpr short MIN_OCTAVE = 1;
	static constexpr short MAX_OCTAVE = 8;
	static constexpr short DEFAULT_OCTAVE = 4;
	
	static std::string nameOct(short oct)	{
		std::string num_suffix;
		switch (oct)	{
			case 1:
				num_suffix = "st";
				break;
			case 2:
				num_suffix = "nd";
				break;
			case 3:
				num_suffix = "rd";
				break;
			default:
				num_suffix = "th";
		}
		return std::to_string(oct) + num_suffix;
	}

	struct Octave	{
		short oct_val;
		std::string oct_name;
	};
	Octave oct = {DEFAULT_OCTAVE, nameOct(DEFAULT_OCTAVE)};
public:
	void set(short oct_val)	{
		if (oct_val < MIN_OCTAVE || oct_val > MAX_OCTAVE)
			throw std::out_of_range("Attempted setting invalid octave value");
		oct.oct_val = oct_val;
		oct.oct_name = nameOct(oct_val);
	}
	void setClamped(short oct_val)	{
		short real_set_value = std::clamp(oct_val, MIN_OCTAVE, MAX_OCTAVE);
		oct.oct_val = real_set_value;
		oct.oct_name = nameOct(real_set_value);
	}
	short getVal()	{
		return oct.oct_val;
	}
	short getMin()	{
		return MIN_OCTAVE;
	}
	short getMax()	{
		return MAX_OCTAVE;
	}
	std::string getName()	{
		return oct.oct_name;
	}
	void resetToDefault()	{
		set(DEFAULT_OCTAVE);
	}
};
class AudioParameters	{
public:
	AudioVolume vol;
	AudioBPM bpm;
	AudioInstrument inst;
	AudioOctave oct;
	
	void resetToDefaultAll()	{
		vol.resetToDefault();
		bpm.resetToDefault();
		inst.resetToDefault();
		oct.resetToDefault();
	}
};

class AudioEvent	{
public:
		virtual ~AudioEvent() = default;
		virtual void runEvent(AudioParameters &params) = 0;
};
class InstrumentChangeEvent : public AudioEvent	{ //Muda instrumento com valor absoluto ou relativo. Absoluto é o default; somente relativo precisa ser especificado
public:
	enum class ChangeMode	{
		ABSOLUTE,
		RELATIVE
	};
private:
	short midi_val;
	ChangeMode mode;
public:
	explicit InstrumentChangeEvent(short midi_val, ChangeMode mode = ChangeMode::ABSOLUTE) :
		midi_val(midi_val), mode(mode) {}

	void runEvent(AudioParameters &params) override	{
		switch (mode)	{
			case ChangeMode::ABSOLUTE :
				params.inst.set(midi_val);
				break;
			case ChangeMode::RELATIVE :
				params.inst.set(params.inst.getVal() + midi_val);
				break;
		}
	}
};
class OctaveChangeEvent	: public AudioEvent	{ //Único método nas especificações é incrementar a oitava
public:
	void runEvent(AudioParameters &params) override	{
		short nextOct = params.oct.getVal()+1;
		if(nextOct > params.oct.getMax())
			params.oct.resetToDefault();
		else
			params.oct.set(nextOct);
	}
};
class EventMapper {
private:
	static bool isAnEvenDigit(char c)	{
		return (c >= '0' && c <= '9') && ((c & 1) == 0);
	}
public:
    static std::unique_ptr<AudioEvent> triggerEvent(char eventKey) { //Recebe caractere chave e retorna uma classe evento via ponteiro especial
		if (isAnEvenDigit(eventKey))
			return std::make_unique<InstrumentChangeEvent>(eventKey-'0', InstrumentChangeEvent::ChangeMode::RELATIVE);
        switch (eventKey) {
            case '!':
                return std::make_unique<InstrumentChangeEvent>(24);
            case ';':
                return std::make_unique<InstrumentChangeEvent>(15);
			case ',':
				return std::make_unique<InstrumentChangeEvent>(114);
            case '\n':
                return std::make_unique<InstrumentChangeEvent>(123);
			case 'o':
			case 'O':
			case 'i':
			case 'I':
			case 'u':
			case 'U':
				return std::make_unique<InstrumentChangeEvent>(110);
			case '?':
			case '.':
				return std::make_unique<OctaveChangeEvent>();
			
        }
	}
};

int main()	{
	

	
	return 0;
}
