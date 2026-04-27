#include <iostream>
#include <string>
#include <algorithm>


class AudioVolume	{
private:
	static constexpr short MIN_VOLUME = 0;
	static constexpr short MAX_VOLUME = 100;
	static constexpr short DEFAULT_VOLUME = 50;
	
	bool muted = false;
	short vol = DEFAULT_VOLUME;
public:
	int set(int vol)	{
		int real_set_value = std::clamp(static_cast<short int>(vol), MIN_VOLUME, MAX_VOLUME);
		this->vol = real_set_value;
		return real_set_value;
	}
	int get()	{
		return vol;
	}
	int getMin()	{
		return MIN_VOLUME;
	}
	int getMax()	{
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
	float set(float bpm)	{
		float real_set_value = std::clamp(bpm, MIN_BPM, MAX_BPM);
		this->bpm = real_set_value;
		return real_set_value;
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

	static std::string nameMIDI(int midi_val)	{
		return "MIDI#" + std::to_string(midi_val);
		}
	
	struct MIDI_Instrument	{
		short midi_val;
		std::string inst_name;
	};
	MIDI_Instrument inst = {DEFAULT_MIDI_VAL, nameMIDI(DEFAULT_MIDI_VAL)};
public:
	int set(int midi_val)	{
		short real_set_val = std::clamp(static_cast<short>(midi_val), MIN_MIDI_VAL, MAX_MIDI_VAL);
		inst.midi_val = real_set_val;
		inst.inst_name = nameMIDI(real_set_val);
		return real_set_val;
	}
	int getVal()	{
		return inst.midi_val;
	}
	int getMin()	{
		return MIN_MIDI_VAL;
	}
	int getMax()	{
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
	
	static std::string nameOct(int oct)	{
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
	int set(int oct_val)	{
		short real_set_value = std::clamp(static_cast<short>(oct_val), MIN_OCTAVE, MAX_OCTAVE);
		oct.oct_val = real_set_value;
		oct.oct_name = nameOct(real_set_value);
		return real_set_value;
	}
	int getVal()	{
		return oct.oct_val;
	}
	int getMin()	{
		return MIN_OCTAVE;
	}
	int getMax()	{
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

class EventMapper	{
private:
	
public:

};

int main()	{
	

	
	return 0;
}
