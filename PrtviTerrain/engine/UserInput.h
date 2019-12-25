#ifndef USER_INPUT_H
#define USER_INPUT_H

#include <sdl2/SDL.h>
#include <glm/glm.hpp>
#include <Input.h>
#include <CameraFreeLook.h>
#include <Terrain.h>
#include <SysOpenGLSetting.h>
#include <Light.h>

namespace UserInput
{
	void update(
		double &dt,
		bool &mouseLook,
		bool &quit,
		bool & showNormals,
		const int &SCREEN_WIDTH,
		const int &SCREEN_HEIGHT,
		glm::mat4 &view,
		SDL_Window*& window,
		SDL_GLContext& context,
		CameraFreeLook&camera,
		Terrain &terrain,
		Light &light,
		Input& input
	);
};

#endif
