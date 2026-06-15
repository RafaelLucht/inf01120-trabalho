#include "AudioSynth.h"
#include "UserInterface.h"

int main() {
    
    FugueScore score;
    
    if(!UI::init())
		return -1;
		
	while(UI::shouldRender())	{
		UI::renderApp(score);
	}
		
	UI::cleanup();
		
    return 0;
}
