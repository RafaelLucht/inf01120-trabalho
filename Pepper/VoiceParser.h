#pragma once

#include <vector>
#include <string>

class Voice;

class VoiceParser {
private:
	static std::vector<std::string> splitLines(const std::string& text);

	static int parseDelay(std::string& line);

	static Voice parseLine(const std::string& line, int voiceIndex);

public:
	static std::vector<Voice> parse(const std::string& text);
};
