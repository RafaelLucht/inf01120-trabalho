#pragma once

class FugueScore;

namespace UI	{
	
	bool init();
	
	bool shouldRender();
	
	void renderApp(FugueScore &score);
	
	void cleanup();
	
}
