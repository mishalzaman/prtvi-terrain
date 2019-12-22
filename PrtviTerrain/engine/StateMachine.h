#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

namespace SM {
	const int INIT = 0;
	const int LOAD = 1;
	const int UPDATE = 2;
	const int RENDER_GEOMETRY = 3;
	const int RENDER_TEXT = 4;
	const int QUIT = 5;

	class State
	{
	public:
		State();
		~State();
		void ToLoad();
		void ToUpdate();
		void ToRenderGeometry();
		void ToRenderText();
		void ToQuit();
		int current();
	private:
		int state = INIT;
	};
}




#endif

