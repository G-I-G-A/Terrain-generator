#include "Camera.h"

Camera* Camera::main = nullptr;

Camera::Camera(const Point3d<float>& position, const Point3d<float>& up, float yaw, float pitch) :
	m_front({0.f, 0.f, -1.f}),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVITY),
	m_fov(FOV)
{
	m_pressedKeys.fill(false);

	m_position = position;
	m_worldUp = up;
	m_yaw = yaw;
	m_pitch = pitch;

	UpdateCameraVectors();
}
Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
	m_front({ 0.f, 0.f, -1.f }),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVITY),
	m_fov(FOV)
{
	m_position = Point3d<float>(posX, posY, posZ);
	m_worldUp = Point3d<float>(upX, upY, upZ);
	m_yaw = yaw;
	m_pitch = pitch;

	UpdateCameraVectors();
}

Mat4<float> Camera::GetViewMatrix() const
{
	return Math::LookAt(m_position, m_position + m_front, m_up);
}
Mat4<float> Camera::GetProjectionMatrix(int windowWidth, int windowHeight) const
{
	return  Mat4<float>::projection(Math::Radians(m_fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
}

void Camera::ProcessKeyboardInput(CameraMovement direction, bool pressed)
{
	m_pressedKeys[static_cast<int>(direction)] = pressed;
}

void Camera::ProcessKeyboardInputs(float deltatime)
{
	float velocity = m_movementSpeed * deltatime;

	if (m_pressedKeys[0]) // FORWARD
		m_position += velocity * m_front;

	if (m_pressedKeys[1]) // BACKWARD
		m_position -= velocity * m_front;

	if (m_pressedKeys[2]) // LEFT
		m_position -= Math::Normalize(Math::Cross(m_front, m_up)) * velocity;

	if (m_pressedKeys[3]) // RIGHT
		m_position += Math::Normalize(Math::Cross(m_front, m_up)) * velocity;

	if (m_pressedKeys[4]) // UP
		m_position += velocity * m_up;
	
	if (m_pressedKeys[5]) // DOWN
		m_position -= velocity * m_up;
}

void Camera::ProcessMouseMovementInputs(float xPos, float yPos, bool constraintPitch)
{
	xPos *= m_mouseSensitivity;
	yPos *= m_mouseSensitivity;

	m_yaw += xPos;
	m_pitch -= yPos;

	if (constraintPitch)
	{
		if (m_pitch > PITCH_LIMIT)
			m_pitch = PITCH_LIMIT;
		if (m_pitch < -PITCH_LIMIT)
			m_pitch = -PITCH_LIMIT;
	}

	UpdateCameraVectors();
}

void Camera::ProcessMouseScrollInputs(float yOffset)
{
	m_fov -= yOffset;
	if (m_fov < 1.0f)
		m_fov = 1.0f;
	if (m_fov > 80.0f)
		m_fov = 80.0f;
}

void Camera::UpdateCameraVectors()
{
	Point3d<float> front(
		cos(Math::Radians(m_yaw)) * cos(Math::Radians(m_pitch)),
		sin(Math::Radians(m_pitch)),
		sin(Math::Radians(m_yaw)) * cos(Math::Radians(m_pitch))
	);

	m_front = Math::Normalize(front);
	m_right = Math::Normalize(Math::Cross(m_front, m_worldUp));
	m_up = Math::Normalize(Math::Cross(m_right, m_front));
}