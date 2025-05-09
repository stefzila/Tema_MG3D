#pragma once
#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

//default camera values
const float YAW			= -90.0f;
const float SPEED		= 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM		= 45.0f;
const float PITCH		= 0.0f;

class Camera
{
public:
	// camera Attributes
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;
	// euler Angles
	float Yaw;
	float Pitch;
	// camera options
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(	glm::vec3	position	= glm::vec3(0.0f, 0.0f, 0.0f), 
			glm::vec3	up			= glm::vec3(0.0f, 1.0f, 0.0f),
			float		yaw			= YAW, 
			float		pitch		= PITCH)
			: Front(
			glm::vec3(0.0f, 0.0f, -1.0f)), 
			MovementSpeed(SPEED),
			MouseSensitivity(SENSITIVITY),
			Zoom(ZOOM)
	{

		Position	= position;
		WorldUp		= up;
		Yaw			= yaw;
		Pitch		= pitch;
		UpdateCameraVectors();

	}

	// returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(Position, Position + Front, Up);
	}

	void ProcessKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;

		if (direction == FORWARD)
			Position += Front * velocity;

		if (direction == BACKWARD)
			Position -= Front * velocity;

		if (direction == LEFT)
			Position -= Right * velocity;

		if (direction == RIGHT)
			Position += Right * velocity;

		if (direction == UP)
			Position += Up * velocity;

		if (direction == DOWN)
			Position -= Up * velocity;
	}

	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;

		UpdateCameraVectors();
	}

private:

	void UpdateCameraVectors()
	{
		// calculate the new Front vector
		glm::vec3 front;

		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));

		Front = glm::normalize(front);

		// also re-calculate the Right and Up vector
		Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
		Up = glm::normalize(glm::cross(Right, Front));
	}

};

#endif