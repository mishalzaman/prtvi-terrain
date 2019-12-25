#ifndef SYS_OPENGL_H
#define SYS_OPENG_H

#include <SDL2/SDL.h>
#include <GL/glew.h>

namespace SystemOpenGLInit {
    const int ERROR_SDL_INIT = 1;
    const int ERROR_CREATE_WINDOW = 2;
    const int ERROR_CREATE_CONTEXT = 3;
    const int ERROR_GLEW_INIT = 4;

    int initSDL();
    int setGlAttributes(unsigned int major, unsigned int minor);
    int initWindow(SDL_Window* &window, unsigned int width, unsigned int height);
    int initContext(SDL_GLContext& context, SDL_Window*& window);
    int initGlew();
    int shutDown(SDL_Window*& window, SDL_GLContext& context);
}

#endif