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
#include <StateMachine.h>
#include <future>
#include <thread>
#include <functional>

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

void loadTerrainThread(Terrain& terrain, std::atomic<bool>& done);

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
	SM::State state				= SM::State();
	glm::mat4 projection		= glm::perspective(glm::radians(45.0f), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view				= camera.getViewMatrix();

	double dt = 1 / 60.0;

	state.ToLoad();

	std::atomic<bool> done(false);

	// https://www.tutorialcup.com/cplusplus/multithreading.htm
	std::thread loadTerrain([&] {
		terrain.loadVertices("assets/heightmap1025.pgm", "assets/diffuse1025.png", "assets/normal1025.png");
		light.loadVertices(glm::vec3(0, 10, 0));
		done = true;
		});
	
	text.load(SCREEN_WIDTH, SCREEN_HEIGHT);

    while (!quit)
    {
		switch (state.current()) {
			case SM::LOAD: {
				/*------------------
								LOAD
				-------------------*/
				if (done) {
					terrain.loadOGLBuffers("assets/diffuse1025.png", "assets/normal1025.png");
					light.loadOGLBuffers();
					skybox.load();
					 
					loadTerrain.join();

					state.ToUpdate();
					break;
				}

				glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glViewport(0, 0, (int)SCREEN_WIDTH, (int)SCREEN_HEIGHT);

				text.renderText(textShader, "LOADING...", 25.0f, 25.0f);

				SDL_GL_SwapWindow(window);

				break;
			}
			case SM::UPDATE:
/*-----------------------------
				INPUT / PHYSICS
------------------------------*/

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

				state.ToRenderGeometry();
				break;
			case SM::RENDER_GEOMETRY:
/*-----------------------------
				RENDER GEOMETRY
------------------------------*/

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
				text.renderText(textShader, "vertices: " + std::to_string(terrain.verticesCount()), 25.0f, 400.0f);
				text.renderText(textShader, "indices: " + std::to_string(terrain.indicesCount()), 25.0f, 425.0f);

				glm::vec3 cPos = camera.getCameraPosition();
				text.renderText(textShader, "Camera: x" + std::to_string(cPos.x) + " y: " + std::to_string(cPos.y) + " z: " + std::to_string(cPos.x), 25.0f, 50.0f);

				SDL_GL_SwapWindow(window);

				state.ToUpdate();
				break;
		}
    }
	
	oglSystem.deInitialize(window, context);

	std::cout << "successful exit" << std::endl;;
	return 0;
}