#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <iostream>
#include "InitSystem.h"
#include "CameraFreeLook.h"
#include "Input.h"
#include "entities/Terrain.h"
#include "Shader.h"
#include "entities/Light.h"
#include "EntSkyBox.h"
#include "RndrText.h"
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
bool showNormals			 = false;

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

	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);

	// Shaders
	Shader terrainShader		= Shader("shaders/terrain.vert", "shaders/terrain.frag");
	Shader terrainDiffuseShader = Shader("shaders/terrain_diffuse.vert", "shaders/terrain_diffuse.frag");
	Shader normalsShader		= Shader("shaders/normals.vert", "shaders/normals.frag", "shaders/normals.geo");
	Shader lightShader			= Shader("shaders/light.vert", "shaders/light.frag");
	Shader skyboxShader			= Shader("shaders/skybox.vert", "shaders/skybox.frag");
	Shader textShader			= Shader("shaders/text.vert", "shaders/text.frag");

    // Initializations
	CameraFreeLook camera		= CameraFreeLook(SCREEN_WIDTH, SCREEN_HEIGHT);
    Input input					= Input();
	Terrain terrain             = Terrain();
	Light light			        = Light();
	EntSkyBox skybox			= EntSkyBox();
	RndrText text				= RndrText();

	// Projection / View
	glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view			= camera.getViewMatrix();


    /*------
    LOAD
    ------*/

	terrain.load("assets/heightmap513.pgm", "assets/diffuse513.png");
	light.load(glm::vec3(0,10,0));
	skybox.load();
	text.load(SCREEN_WIDTH, SCREEN_HEIGHT);

	/*--------
	UPDATE
	-------*/

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
						camera.mousePositionUpdate(deltaTime, x, y);
						SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
					}

					view = camera.getViewMatrix();
				}

				if (input.isW()) { camera.forward(deltaTime); };
				if (input.isS()) { camera.backward(deltaTime); };
				if (input.isA()) { camera.strafeLeft(deltaTime); };
				if (input.isD()) { camera.strafeRight(deltaTime); };
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
			}
			else if (input.isDownArrow()) {
				light.down();
			} else if (input.isL()) {
				showNormals = !showNormals;
			} else {
                initSystem.enableMouseCursor(true);

                if (mouseLook) {
                    mouseLook = false;
                }
            }

            if (input.isQuit()) { quit = true; }

            /*------
            RENDER
            ------*/

            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

            terrain.draw(projection, view, terrainDiffuseShader, light.position);

			if (showNormals) {
				terrain.draw(projection, view, normalsShader, light.position);
			}
			
			light.draw(projection, view, lightShader);
			skybox.draw(projection, view, skyboxShader);

			text.renderText(textShader, "Framerate: "+std::to_string(deltaTime), 25.0f, 25.0f, 1.0f, glm::vec3(1, 1, 1));
			glm::vec3 cPos = camera.getCameraPosition();
			text.renderText(textShader, "Camera: x" + std::to_string(cPos.x) + " y: " + std::to_string(cPos.y) + " z: " + std::to_string(cPos.x), 25.0f, 50.0f, 1.0f, glm::vec3(1, 1, 1));

            SDL_GL_SwapWindow(window);
        } else {
            SDL_Delay(1);
        }
    }
	
	initSystem.deInitialize(window, context);

	cout << "successful exit";
	return 0;
}