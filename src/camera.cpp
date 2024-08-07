#include "camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

glm::mat4 Camera::getViewMatrix()
{
	glm::mat4 cameraTranslation = glm::translate(glm::mat4(1.0f), position);
	glm::mat4 cameraRotation = getRotationMatrix();
	return glm::inverse(cameraTranslation * cameraRotation);
}

glm::mat4 Camera::getRotationMatrix()
{
	glm::quat pitchRotation = glm::angleAxis(pitch, glm::vec3{ 1.0f, 0.0f, 0.0f });
	glm::quat yawRotation = glm::angleAxis(yaw, glm::vec3{ 0.0f, -1.0f, 0.0f });

	return glm::toMat4(yawRotation) * glm::toMat4(pitchRotation);
}

void Camera::processSDLEvent(SDL_Event& e)
{
	if (e.type == SDL_KEYDOWN && !shouldUnfocus) {
		if (e.key.keysym.sym == SDLK_w) { velocity.z = -1; }
		if (e.key.keysym.sym == SDLK_s) { velocity.z = 1; }
		if (e.key.keysym.sym == SDLK_a) { velocity.x = -1; }
		if (e.key.keysym.sym == SDLK_d) { velocity.x = 1; }
		if (e.key.keysym.sym == SDLK_c) { velocity.y = -1; }
		if (e.key.keysym.sym == SDLK_SPACE) { velocity.y = 1; }

		if (e.key.keysym.sym == SDLK_ESCAPE) { shouldUnfocus ? shouldClose = true : shouldUnfocus = true; }
	}

	if (e.type == SDL_KEYUP) {
		if (e.key.keysym.sym == SDLK_w) { velocity.z = 0; }
		if (e.key.keysym.sym == SDLK_s) { velocity.z = 0; }
		if (e.key.keysym.sym == SDLK_a) { velocity.x = 0; }
		if (e.key.keysym.sym == SDLK_d) { velocity.x = 0; }
		if (e.key.keysym.sym == SDLK_c) { velocity.y = 0; }
		if (e.key.keysym.sym == SDLK_SPACE) { velocity.y = 0; }
	}

	if (e.type == SDL_MOUSEMOTION && !shouldUnfocus) {
		yaw += (float)e.motion.xrel / 200.0f;
		pitch -= (float)e.motion.yrel / 200.0f;
	}

	if (e.type == SDL_MOUSEBUTTONDOWN && shouldUnfocus == true) { shouldUnfocus = false; }
}

void Camera::update()
{
	glm::mat4 cameraRotation = getRotationMatrix();
	position += glm::vec3(cameraRotation * glm::vec4(velocity * 0.05f, 0.0f));
} 
