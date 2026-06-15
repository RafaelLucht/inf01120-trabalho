#include "AudioEvents.h"
#include "AudioParameters.h"

#include <algorithm>

// --- Instrument Change ---
InstrumentChangeEvent::InstrumentChangeEvent(short midi_val, ChangeMode mode) :
	midi_val(midi_val), mode(mode) {}
	
void InstrumentChangeEvent::runEvent(AudioParameters &params) {
	switch (mode) {
		case ChangeMode::ABSOLUTE:
			params.inst.set(midi_val);
			break;
		case ChangeMode::RELATIVE:
			params.inst.setClamped(params.inst.getVal() + midi_val);
			break;
	}
}

// --- Octave Change ---
OctaveChangeEvent::OctaveChangeEvent(short octShift) : octShift(octShift) {}

void OctaveChangeEvent::runEvent(AudioParameters &params) {
	short oct = params.oct.getVal() + octShift;
	if (oct > params.oct.getMax() || oct < params.oct.getMin())
		params.oct.resetToDefault();
	else
		params.oct.set(oct);
}

// --- BPM Change ---
BpmChangeEvent::BpmChangeEvent(float bpmShift) : bpmShift(bpmShift) {}

void BpmChangeEvent::runEvent(AudioParameters &params) {
	params.bpm.setClamped(params.bpm.get() + bpmShift);
}

// --- Volume Change ---
void VolumeChangeEvent::runEvent(AudioParameters &params) {
	params.vol.setClamped(params.vol.get() * 2);
}

// --- Note Event ---
NoteEvent::NoteEvent(int baseNote) : baseNote(baseNote) {}

NoteEvent::NoteEvent(const NoteEvent& other) : baseNote(other.baseNote) {}

int NoteEvent::getBaseNote() const {
	return baseNote;
}

int NoteEvent::getNote(const AudioParameters& params) const {
	int nota = baseNote + (params.oct.getVal() - 5) * 12;
	return std::clamp(nota, 0, 127);
}
