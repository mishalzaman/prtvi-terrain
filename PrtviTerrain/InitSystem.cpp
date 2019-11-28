#include "InitSystem.h"

using namespace std;

InitSystem::InitSystem() {

}

InitSystem::~InitSystem() {

}

int InitSystem::initialize(SDL_Window*& window, SDL_GLContext& context, int width, int height, int major, int minor) {
	if (!sdlInit()) {
		return this->ERROR_SDL_INIT;
	}

	setGlewExperimental(GL_TRUE);
	setGlAttributes(major, minor);

    if (!createWindow(window, width, height)) {
		return this->ERROR_CREATE_WINDOW;
	}

    if (!createContext(context, window)) {
		return this->ERROR_CREATE_CONTEXT;
	}

    if (!initializeGlew()) {
		return this->ERROR_GLEW_INIT;
	}

	return 0;
}

void InitSystem::deInitialize(SDL_Window* &window, SDL_GLContext &context) {
    cout << "shutting down system" << endl;
    SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void InitSystem::enableDepthTest(bool state) {
	state ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
}

void InitSystem::enableMouseCapture(bool state)
{
	state ? SDL_CaptureMouse(SDL_TRUE) : SDL_CaptureMouse(SDL_FALSE);
}

void InitSystem::enableMouseCursor(bool state)
{
	state ? SDL_ShowCursor(1) : SDL_ShowCursor(0);
}

void InitSystem::centerMouse(SDL_Window* window, unsigned int width, unsigned int height)
{
	SDL_WarpMouseInWindow(window, width / 2.0f, height / 2.0f);
}

void InitSystem::enableWireframe(bool state)
{
	state ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void InitSystem::enableCulling(bool state)
{
	if (state == true)
	{
		glEnable(GL_CULL_FACE);
		glCullFace(GL_FRONT);

		return;
	}

	glDisable(GL_CULL_FACE);
}

bool InitSystem::sdlInit() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}
    return true;
}

bool InitSystem::createWindow(SDL_Window*& window, int width, int height) {
    window = SDL_CreateWindow(
		"Test SDl2 and Glew",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		width,
		height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	if (!window) {
		return false;
	}
    return true;
}

bool InitSystem::createContext(SDL_GLContext &context, SDL_Window* &window) {
    context = SDL_GL_CreateContext(window);

    if (!context) {
        return false;
    }
    return true;
}

void InitSystem::setGlewExperimental(bool state) {
    glewExperimental = GL_TRUE;
}

bool InitSystem::initializeGlew() {
    GLenum err = glewInit();

	if (err) {
		return false;
	}
    return true;
}

void InitSystem::setGlAttributes(int major, int minor) {
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, major);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, minor);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
}