#include "AudioParameters.h"

#include <string>
#include <string_view>
#include <array>
#include <algorithm>


// --- Volume ---
AudioVolume::AudioVolume(short defaultVolume) :
	defaultVolume(defaultVolume) {}
	
void AudioVolume::set(short vol) {
	this->vol = vol;
}
void AudioVolume::setClamped(short vol) {
	this->vol = std::clamp(vol, MIN_VOLUME, MAX_VOLUME);
}

short AudioVolume::get() const {
	return vol;
}
short AudioVolume::getMin() const {
	return MIN_VOLUME;
}
short AudioVolume::getMax() const {
	return MAX_VOLUME;
}

void AudioVolume::resetToDefault() {
	set(defaultVolume);
}

// --- BPM ---
void AudioBPM::set(float bpm) {
	this->bpm = bpm;
}
void AudioBPM::setClamped(float bpm) {
	this->bpm = std::clamp(bpm, MIN_BPM, MAX_BPM);
}

float AudioBPM::get() const {
	return bpm;
}
float AudioBPM::getMin() const {
	return MIN_BPM;
}
float AudioBPM::getMax() const {
	return MAX_BPM;
}

void AudioBPM::resetToDefault() {
	set(DEFAULT_BPM);
}

// --- Instrument ---
const std::array<std::string_view, 128> AudioInstrument::MIDI_NAMES = {
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

AudioInstrument::AudioInstrument(short defaultMidiVal) :
	defaultMidiVal(defaultMidiVal) {}
	
void AudioInstrument::set(short midi_val) {
	this->midi_val = midi_val;
}
void AudioInstrument::setClamped(short midi_val) {
	this->midi_val = std::clamp(midi_val, MIN_MIDI_VAL, MAX_MIDI_VAL);
}

short AudioInstrument::getVal() const {
	return midi_val;
}
short AudioInstrument::getMin() const {
	return MIN_MIDI_VAL;
}
short AudioInstrument::getMax() const {
	return MAX_MIDI_VAL;
}
std::string_view AudioInstrument::getName() const {
	return MIDI_NAMES[midi_val];
}

void AudioInstrument::resetToDefault() {
	set(defaultMidiVal);
}

// --- Octave ---	
AudioOctave::AudioOctave(short defaultOctave) :
	defaultOctave(defaultOctave) {}
	
void AudioOctave::set(short oct_val) {
	this->oct_val = oct_val;
}
void AudioOctave::setClamped(short oct_val) {
	this->oct_val = std::clamp(oct_val, MIN_OCTAVE, MAX_OCTAVE);
}

short AudioOctave::getVal() const {
	return oct_val;
}
short AudioOctave::getMin() const {
	return MIN_OCTAVE;
}
short AudioOctave::getMax() const {
	return MAX_OCTAVE;
}
std::string AudioOctave::getName() const {
	std::string num_suffix;
	switch (oct_val) {
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
	return std::to_string(oct_val) + num_suffix;
}

void AudioOctave::resetToDefault() {
	set(defaultOctave);
}

// --- Parameters ---
void AudioParameters::resetToDefaultAll() {
	vol.resetToDefault();
	bpm.resetToDefault();
	inst.resetToDefault();
	oct.resetToDefault();
}
