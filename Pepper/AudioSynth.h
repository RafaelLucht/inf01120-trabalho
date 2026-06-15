#pragma once

#include "AudioParameters.h"
#include "AudioEvents.h"

#include <array>
#include <memory>
#include <string>
#include <vector>

class AudioParameters;
class AudioEvent;

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
	explicit Voice(int voiceIndex);

	void setDelay(int n);
	
	void addEvent(std::unique_ptr<AudioEvent> event);
	
	int getDelay() const;
	int getVoiceIndex() const;
	const AudioParameters& getParams() const;
	const std::vector<std::unique_ptr<AudioEvent>>& getEvents() const;
};

class FugueScore {
private:
	std::vector<Voice> voices;

public:
	FugueScore() = default;

	void addVoice(Voice voice);
	
	const std::vector<Voice> &getVoices() const;
	int getVoiceCount() const;

	void play();
	
	static FugueScore fromText(const std::string& text);
};
