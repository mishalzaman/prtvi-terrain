#include "UserInput.h"

void UserInput::update(double& dt, bool& mouseLook, bool& quit, bool& showNormals, const int& SCREEN_WIDTH, const int& SCREEN_HEIGHT, glm::mat4 &view, SDL_Window*& window, SDL_GLContext& context, CameraFreeLook& camera, Terrain& terrain, Light& light, Input& input)
{
	input.update(dt);

	if (input.isLShift()) {
		SysOpenGLSetting::mouseCursor(false);

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
		SysOpenGLSetting::wireframe(true);
	}
	else if (input.isO()) {
		SysOpenGLSetting::mouseCursor(false);
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
		SysOpenGLSetting::mouseCursor(true);

		if (mouseLook) {
			mouseLook = false;
		}
	}

	if (input.isQuit()) { quit = true; }
}
