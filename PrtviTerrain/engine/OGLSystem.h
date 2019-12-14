#ifndef INIT_SYSTEM
#define INIT_SYSTEM

#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <string>
#include <iostream>

class OGLSystem
{
    public:
		OGLSystem();
        ~OGLSystem();
        int initialize(SDL_Window* &window, SDL_GLContext &context, int width, int height, int major, int minor);
        void deInitialize(SDL_Window* &window, SDL_GLContext & context);

        // options
        void enableDepthTest(bool state);
        void enableCulling(bool state);
        void enableWireframe(bool state);
        void enableMouseCapture(bool state);
        void enableMouseCursor(bool state);
        void centerMouse(SDL_Window* window, unsigned int width, unsigned int height);

        // Error codes
        const int ERROR_SDL_INIT = 1;
        const int ERROR_CREATE_WINDOW = 2;
        const int ERROR_CREATE_CONTEXT = 3;
        const int ERROR_GLEW_INIT = 4;
    private:
        bool sdlInit();
        bool createWindow(SDL_Window*& window, int width, int height);
        bool createContext(SDL_GLContext &context, SDL_Window* &window);
        void setGlewExperimental(bool state);
        bool initializeGlew();
        void setGlAttributes(int major, int minor);
};

#endif 