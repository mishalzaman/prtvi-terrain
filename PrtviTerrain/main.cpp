#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include "InitSystem.h"
#include "CameraFP.h"
#include "Input.h"
#include "Terrain.h"
#include "Shader.h"
#include "Light.h"
#include "Player.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
SDL_Window* window;
SDL_GLContext context;
InitSystem initSystem = InitSystem();
const int MINIMUM_FPS_FRAME  = 6;                           
float minimum_fps_delta_time = 1000 / MINIMUM_FPS_FRAME;
float deltaTime, lastTime    = 0.0f;
float previous_timestep      = SDL_GetTicks();
bool quit                    = false;
bool mouseLook               = false;

int main( int argc, char* args[] )
{
	int iSError = initSystem.initialize(
		window, 
		context, 
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		3, 
		3
	);

	if (iSError > 0) {
		return iSError;
	}

    // OpenGL options
    initSystem.enableDepthTest(true);
    initSystem.enableMouseCursor(true);
    initSystem.enableMouseCapture(true);
    initSystem.centerMouse(window, SCREEN_WIDTH, SCREEN_HEIGHT);
    initSystem.enableWireframe(false);
    initSystem.enableCulling(true); 

	// Shaders
	Shader terrainShader	= Shader("shaders/terrain.vert", "shaders/terrain.frag");
	Shader terrainDiffuseShader = Shader("shaders/terrain_diffuse.vert", "shaders/terrain_diffuse.frag");
	Shader lightShader		= Shader("shaders/light.vert", "shaders/light.frag");

    // Initializations
    CameraFP cameraFP       = CameraFP(SCREEN_WIDTH, SCREEN_HEIGHT);
    Input input             = Input();
    Terrain terrain         = Terrain("assets/test.pgm");
	Light light			    = Light();
	Player player			= Player();

	// Projection / View
	glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view			= cameraFP.getViewMatrix();


    /*------
    LOAD
    ------*/

    // https://tangrams.github.io/heightmapper/#13.2625/17.9482/-98.9671
    
    // terrain shader colouring
    // https://gamedev.stackexchange.com/questions/111875/terrain-shader-from-heightmap-opengl-glsl
    terrain.load(glm::vec3(0,0,0));
	terrain.diffuseMap("assets/placeholder.png");
	light.load(glm::vec3(0,2,0));
	player.load(glm::vec3(0, 1, 0));

    while (!quit)
    {
        float current_timestep = SDL_GetTicks();

        if (previous_timestep < current_timestep) {
            float deltaTime = current_timestep - previous_timestep;

			if (deltaTime > minimum_fps_delta_time) {
				deltaTime = minimum_fps_delta_time; // slow down if the computer is too slow
			}
            previous_timestep = current_timestep;

            /*------
            INPUT
            ------*/

            input.update(deltaTime);

			if (input.isLShift()) {
				initSystem.enableMouseCursor(false);

				if (!mouseLook) {
					SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
					mouseLook = true;
				}
				else {
					if (input.isMouseMotion()) {
						int x, y;
						SDL_GetMouseState(&x, &y);
						cameraFP.mousePositionUpdate(deltaTime, x, y);
						SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
					}

					view = cameraFP.getViewMatrix();
				}

				if (input.isW()) { cameraFP.forward(deltaTime); };
				if (input.isS()) { cameraFP.backward(deltaTime); };
				if (input.isA()) { cameraFP.strafeLeft(deltaTime); };
				if (input.isD()) { cameraFP.strafeRight(deltaTime); };
				if (input.isUpArrow()) { light.forward(); }
				if (input.isDownArrow()) { light.backward(); }
			} else if (input.isZ()){
				terrain.increaseHeightScale();
			} else if (input.isX()) {
				terrain.decreaseHeightScale();
			} else if (input.isP()) {
				initSystem.enableWireframe(true);
			}
			else if (input.isO()) {
				initSystem.enableWireframe(false);
			} else if (input.isLeftArrow()) {
				light.left();
			} else if (input.isRightArrow()) {
				light.right();
			} else if (input.isUpArrow()) {
				light.up();
			} else if (input.isDownArrow() ) {
				light.down();
			} else {
                initSystem.enableMouseCursor(true);

                if (mouseLook) {
                    mouseLook = false;
                }
            }

            if (input.isQuit()) { quit = true; }

            // printf("%f\n", deltaTime);

            /*------
            RENDER
            ------*/

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

            terrain.draw(projection, view, terrainDiffuseShader, light.position);
			light.draw(projection, view, lightShader);
			player.draw(projection, view, lightShader);

            SDL_GL_SwapWindow(window);
        } else {
            SDL_Delay(1);
        }
    }
	
	initSystem.deInitialize(window, context);

	cout << "successful exit";
	return 0;
}