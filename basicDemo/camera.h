#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\euler_angles.hpp> // glm::yawPitchRoll

#include <iostream>

class camera
{
	glm::mat4 View;

	bool cameraMode;
	float mouseSpeed = 0.05f;
	float keyboardSpeed = 0.5f;
	float deltaTime = 1.0f;

	//Keyboard
	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 right;
	glm::vec3 up;

	//mouse
	GLfloat yaw;
	GLfloat pitch;

	float speed;
	float speedMouse;

	void updateCamera();
public:
	camera();
	~camera();
	glm::mat4 getRotation();
	glm::mat4 getView();
	glm::vec3 getPosition();
	glm::vec3 getFront();
	void updateInputMouse(GLfloat offsetX, GLfloat offsetY);
	void updateInputKeyboard(const char direction);
	bool getCameraMode();
	void changeCameraMode();
	void setCameraMode(bool cameraMod);
	float getSpeedMouse();
	void setSpeedMouse(float speed);
	float getSpeed();
	void setSpeed(float speedK);
	float getDeltatime();
	void setDeltatime(float speed);
private:

};