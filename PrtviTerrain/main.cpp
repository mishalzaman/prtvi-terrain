#include <SDL2/SDL.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
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
#include <SysOpenGLInit.h>
#include <SysOpenGLSetting.h>
#include <UserInput.h>

using namespace std;

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
SDL_Window* window;
SDL_GLContext context;
bool quit                    = false;
bool mouseLook               = false;
bool showNormals			 = false;
int engineError;

int main( int argc, char* args[] )
{
	if (engineError = SystemOpenGLInit::initSDL() > 0) return engineError;
	if (engineError = SystemOpenGLInit::setGlAttributes(3, 3) > 0) return engineError;
	if (engineError = SystemOpenGLInit::initWindow(window, SCREEN_WIDTH, SCREEN_HEIGHT) > 0) return engineError;
	if (engineError = SystemOpenGLInit::initContext(context, window) > 0) return engineError;
	if (engineError = SystemOpenGLInit::initGlew() > 0) return engineError;

	/*-------------
	OPENGL SETTINGS
	--------------*/
	SysOpenGLSetting::depthTest(true);
	SysOpenGLSetting::culling(true);
	SysOpenGLSetting::mouseCursor(true);
	SysOpenGLSetting::mouseCapture(true);
	SysOpenGLSetting::mouseCenter(window, SCREEN_WIDTH, SCREEN_HEIGHT);
	SysOpenGLSetting::wireframe(false);

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
		
    while (!quit)
    {
		switch (state.current()) {
			case SM::LOAD:
/*------------------
				LOAD
-------------------*/
				terrain.load("assets/heightmap1025.pgm", "assets/diffuse1025.png", "assets/normal1025.png");
				light.load(glm::vec3(0, 10, 0));
				skybox.load();
				text.load(SCREEN_WIDTH, SCREEN_HEIGHT);

				state.ToUpdate();
				break;
			case SM::UPDATE:
/*-----------------------------
				INPUT / PHYSICS
------------------------------*/
				UserInput::update(dt, mouseLook, quit, showNormals, SCREEN_WIDTH, SCREEN_HEIGHT, view, window, context, camera, terrain, light, input);

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
	

	return SystemOpenGLInit::shutDown(window, context);
}