#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <OGLSystem.h>
#include <CameraFreeLook.h>
#include <Input.h>
#include <Shader.h>
#include <Light.h>
#include <SkyBox.h>
#include <Terrain.h>
#include <RndrText.h>
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
SDL_Window* window;
SDL_GLContext context;
OGLSystem oglSystem = OGLSystem();
bool quit                    = false;
bool mouseLook               = false;
bool showNormals			 = false;

int main( int argc, char* args[] )
{
	int iSError = oglSystem.initialize(
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
	oglSystem.enableDepthTest(true);
	oglSystem.enableMouseCursor(true);
	oglSystem.enableMouseCapture(true);
	oglSystem.centerMouse(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	oglSystem.enableWireframe(false);
	oglSystem.enableCulling(true);

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
	SkyBox skybox			    = SkyBox();
	RndrText text				= RndrText();

	// Projection / View
	glm::mat4 projection	= glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view			= camera.getViewMatrix();


    /*------
    LOAD
    ------*/

	terrain.load("assets/heightmap1025.pgm", "assets/diffuse1025.png", "assets/normal1025.png");
	light.load(glm::vec3(0,10,0));
	skybox.load();
	text.load(SCREEN_WIDTH, SCREEN_HEIGHT);

	// semi fixed timestep
	// https://gafferongames.com/post/fix_your_timestep/

	double t = 0.0;
	double dt = 1 / 60.0;

	double currentTime = SDL_GetTicks();
	double accumulator = 0.0;
	double numFrames = 0;
	double framesMS = 0.0;
		
    while (!quit)
    {
		double newTime = SDL_GetTicks();
		double frameTime = newTime - currentTime;
		if (frameTime > 0.25) {
			frameTime = 0.25;
		}

		currentTime = newTime;
		accumulator += frameTime;

		while (accumulator >= dt) {
			/*---------------------
			INPUT / UPDATE PHYSICS
			---------------------*/

			input.update(dt);

			if (input.isLShift()) {
				oglSystem.enableMouseCursor(false);

				if (!mouseLook) {
					SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
					mouseLook = true;
				}
				else {
					if (input.isMouseMotion()) {
						int x, y;
						SDL_GetMouseState(&x, &y);
						camera.mousePositionUpdate(dt, x, y);
						SDL_WarpMouseInWindow(window, SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
					}

					view = camera.getViewMatrix();
				}

				if (input.isW()) { camera.forward(dt); };
				if (input.isS()) { camera.backward(dt); };
				if (input.isA()) { camera.strafeLeft(dt); };
				if (input.isD()) { camera.strafeRight(dt); };
				if (input.isUpArrow()) { light.forward(); }
				if (input.isDownArrow()) { light.backward(); }
			}
			else if (input.isZ()) {
				terrain.increaseHeightScale();
			}
			else if (input.isX()) {
				terrain.decreaseHeightScale();
			}
			else if (input.isP()) {
				oglSystem.enableWireframe(true);
			}
			else if (input.isO()) {
				oglSystem.enableWireframe(false);
			}
			else if (input.isLeftArrow()) {
				light.left();
			}
			else if (input.isRightArrow()) {
				light.right();
			}
			else if (input.isUpArrow()) {
				light.up();
			}
			else if (input.isDownArrow()) {
				light.down();
			}
			else if (input.isL()) {
				showNormals = !showNormals;
			}
			else {
				oglSystem.enableMouseCursor(true);

				if (mouseLook) {
					mouseLook = false;
				}
			}

			if (input.isQuit()) { quit = true; }

			accumulator -= dt;
			t += dt;
			numFrames++;
		}

		if (t >= 1.0) {
			framesMS = 1000.0 / double(numFrames);
			t = 0.0;
			numFrames = 0;
		}

		/*--------------------
		SCREEN CLEAR AND RESET
		---------------------*/

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

		/*--------------
		RENDER ENTITIES
		---------------*/

		terrain.draw(projection, view, terrainDiffuseShader, light.position, camera.getCameraPosition());

		if (showNormals) {
			terrain.draw(projection, view, normalsShader, light.position, camera.getCameraPosition());
		}

		light.draw(projection, view, lightShader);
		skybox.draw(projection, view, skyboxShader);

		/*--------------
		RENDER TEXT
		---------------*/
		text.renderText(textShader, "Frames per second: "+std::to_string(framesMS), 25.0f, 25.0f, 1.0f, glm::vec3(1, 1, 1));
		text.renderText(textShader, "vertices: " + std::to_string(terrain.verticesCount()), 25.0f, 400.0f, 1.0f, glm::vec3(1, 1, 1));
		text.renderText(textShader, "indices: " + std::to_string(terrain.indicesCount()), 25.0f, 425.0f, 1.0f, glm::vec3(1, 1, 1));

		glm::vec3 cPos = camera.getCameraPosition();
		text.renderText(textShader, "Camera: x" + std::to_string(cPos.x) + " y: " + std::to_string(cPos.y) + " z: " + std::to_string(cPos.x), 25.0f, 50.0f, 1.0f, glm::vec3(1, 1, 1));

		SDL_GL_SwapWindow(window);
    }
	
	oglSystem.deInitialize(window, context);

	std::cout << "successful exit" << std::endl;;
	return 0;
}