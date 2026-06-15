#pragma once

#include <memory>
#include <string>

class AudioEvent;

class EventMapper {
private:
	static bool isAnEvenDigit(char c);
public:
	static std::unique_ptr<AudioEvent> triggerEvent(char eventKey);
	// --- Nova sobrecarga para string ---
	static std::unique_ptr<AudioEvent> triggerEvent(const std::string &eventKey);
};
