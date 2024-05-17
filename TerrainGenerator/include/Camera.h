#ifndef CAMERA_H
#define CAMERA_H

#include "MathHelper.h"

// Camera movement options
enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN,
	COUNT = 6
};

// Default constant camera values
constexpr float YAW = -90.0f;
constexpr float PITCH = 0.0f;
constexpr float PITCH_LIMIT = 89.9f;
constexpr float SPEED = 5.0f;
constexpr float SENSITIVITY = 0.1f;
constexpr float FOV = 45.0f;

// Free movement and view camera
class Camera
{
public:
	explicit Camera(const Point3d<float>& position = {0.f, 0.f, 0.f}, const Point3d<float>& up = {0.f, 1.f, 0.f}, float yaw = YAW, float pitch = PITCH);
	explicit Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw = YAW, float pitch = PITCH);

	Mat4<float> GetViewMatrix() const;
	Mat4<float> GetProjectionMatrix(int windowWidth, int windowHeight) const;

	virtual void ProcessKeyboardInput(CameraMovement direction, bool pressed);
	virtual void ProcessKeyboardInputs(float deltatime);
	virtual void ProcessMouseMovementInputs(float xPos, float yPos, bool constraintPitch = true);
	virtual void ProcessMouseScrollInputs(float yoffset);

	static Camera* main;

private:
	Point3d<float> m_position;
	Point3d<float> m_up;
	Point3d<float> m_right;
	Point3d<float> m_front;
	Point3d<float> m_worldUp;

	float m_yaw;
	float m_pitch;

	float m_movementSpeed;
	float m_mouseSensitivity;
	float m_fov;

	std::array<bool, 6> m_pressedKeys;

	void UpdateCameraVectors();
};

#endif CAMERA_H