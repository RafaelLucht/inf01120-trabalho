#pragma once

class AudioParameters;

class AudioEvent {
public:
	virtual ~AudioEvent() = default;
	virtual void runEvent(AudioParameters &params) = 0;
};

class InstrumentChangeEvent : public AudioEvent {
public:
	enum class ChangeMode { ABSOLUTE, RELATIVE };
	explicit InstrumentChangeEvent(short midi_val, ChangeMode mode = ChangeMode::ABSOLUTE);
	
	void runEvent(AudioParameters &params) override;
	
private:
	short midi_val;
	ChangeMode mode;
};

class OctaveChangeEvent : public AudioEvent {
private:
	short octShift;
public:
	explicit OctaveChangeEvent(short octShift);
	
	void runEvent(AudioParameters &params) override;
};

class BpmChangeEvent : public AudioEvent {
private:
	float bpmShift;
public:
	explicit BpmChangeEvent(float bpmShift);
	
	void runEvent(AudioParameters &params) override;
};

class VolumeChangeEvent : public AudioEvent {
public:
	void runEvent(AudioParameters &params) override;
};

class NoteEvent : public AudioEvent {
private:
    int baseNote;
public:
    explicit NoteEvent(int baseNote);
    NoteEvent(const NoteEvent& other);

    void runEvent(AudioParameters& params) override {}

    int getBaseNote() const;

    int getNote(const AudioParameters& params) const;
};
