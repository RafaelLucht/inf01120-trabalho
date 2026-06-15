#include "EventMapper.h"
#include "AudioEvents.h"

#include <memory>
#include <string>

bool EventMapper::isAnEvenDigit(char c) {
	return (c >= '0' && c <= '9') && ((c & 1) == 0);
}

std::unique_ptr<AudioEvent> EventMapper::triggerEvent(char eventKey) {
		
	// --- Muda instrumento (valor relativo) ---
	if (isAnEvenDigit(eventKey))
		return std::make_unique<InstrumentChangeEvent>(eventKey - '0', InstrumentChangeEvent::ChangeMode::RELATIVE);
		
	switch (eventKey) {
		// --- Muda instrumento (valor absoluto) ---
		case '!': return std::make_unique<InstrumentChangeEvent>(24);
		case ';': return std::make_unique<InstrumentChangeEvent>(15);
		case ',': return std::make_unique<InstrumentChangeEvent>(114);
		case 'o':
		case 'i':
		case 'u': return std::make_unique<InstrumentChangeEvent>(110);
		
		// --- Aritmética na oitava ---
		case '?':
		case '.': return std::make_unique<OctaveChangeEvent>(1);
		case 'V': return std::make_unique<OctaveChangeEvent>(-1);
		
		// --- Aritmética na BPM ---
		case '<': return std::make_unique<BpmChangeEvent>(-10);
		case '>': return std::make_unique<BpmChangeEvent>(10);
		
		// --- Dobra volume (limitado superiormente) ---
		case ' ': return std::make_unique<VolumeChangeEvent>();
		
		// --- Notas musicais (*Mi Bemol na sobrecarga com string*) ---
		case 'A': return std::make_unique<NoteEvent>(69); // Lá
		case 'B': return std::make_unique<NoteEvent>(71); // Si
		case 'C': return std::make_unique<NoteEvent>(60); // Dó
		case 'D': return std::make_unique<NoteEvent>(62); // Ré
		case 'E': return std::make_unique<NoteEvent>(64); // Mi
		case 'F': return std::make_unique<NoteEvent>(65); // Fá
		case 'G': return std::make_unique<NoteEvent>(67); // Sol
		case 'H': return std::make_unique<NoteEvent>(70); // Si bemol
	}
	return nullptr; // Evento não mapeado diretamente; pausa ou repete nota
}
	
std::unique_ptr<AudioEvent> EventMapper::triggerEvent(const std::string &eventKey) {
	
	// --- Rede de segurança ---
	if (eventKey.empty())
		return nullptr;
	
	if (eventKey == "Mb")
		return std::make_unique<NoteEvent>(63); // Mi bemol
		
	return nullptr;
}
