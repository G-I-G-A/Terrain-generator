#include "Shader.h"

#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

#include "Utils.h"

static std::string shaderFolderPath = "Resources/Shaders/";

Shader::Shader(const char* vertexPath, const char* fragmentPath) : m_ID(0)
{
	GLuint vertexShader = createAndCompileShader(vertexPath, GL_VERTEX_SHADER);
	GLuint fragmentShader = createAndCompileShader(fragmentPath, GL_FRAGMENT_SHADER);

	GLuint shader = glCreateProgram();
	glAttachShader(shader, vertexShader);
	glAttachShader(shader, fragmentShader);
	glLinkProgram(shader);

	int success;
	glGetProgramiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetProgramInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Shader program error: " << infoLog << std::endl;

		throw std::runtime_error("Shader does not compile!");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	m_ID = shader;
}

Shader::~Shader()
{
	glDeleteProgram(m_ID);
}

// USe the shader
void Shader::use() const
{
	glUseProgram(m_ID);
}

const GLuint& Shader::getID() const
{
	return m_ID;
}

// Utility uniform functions
// Bool
void Shader::setBool(const char* name, bool v) const
{
	glUniform1i(glGetUniformLocation(m_ID, name), (int)v);
}
void Shader::setBool2(const char* name, bool v1, bool v2) const
{
	glUniform2i(glGetUniformLocation(m_ID, name), (int)v1, (int)v2);
}
void Shader::setBool3(const char* name, bool v1, bool v2, bool v3) const
{
	glUniform3i(glGetUniformLocation(m_ID, name), (int)v1, (int)v2, (int)v3);
}
void Shader::setBool4(const char* name, bool v1, bool v2, bool v3, bool v4) const
{
	glUniform4i(glGetUniformLocation(m_ID, name), (int)v1, (int)v2, (int)v3, (int)v4);
}

// Int
void Shader::setInt(const char* name, int v) const
{
	glUniform1i(glGetUniformLocation(m_ID, name), v);
}
void Shader::setInt2(const char* name, int v1, int v2) const
{
	glUniform2i(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setInt3(const char* name, int v1, int v2, int v3) const
{
	glUniform3i(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setInt4(const char* name, int v1, int v2, int v3, int v4) const
{
	glUniform4i(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

// Float
void Shader::setFloat(const char* name, float v) const
{
	glUniform1f(glGetUniformLocation(m_ID, name), v);
}
void Shader::setFloat2(const char* name, float v1, float v2) const
{
	glUniform2f(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setFloat3(const char* name, float v1, float v2, float v3) const
{
	glUniform3f(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setFloat4(const char* name, float v1, float v2, float v3, float v4) const
{
	glUniform4f(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

// Double
void Shader::setDouble(const char* name, double v) const
{
	glUniform1d(glGetUniformLocation(m_ID, name), v);
}
void Shader::setDouble2(const char* name, double v1, double v2) const
{
	glUniform2d(glGetUniformLocation(m_ID, name), v1, v2);
}
void Shader::setDouble3(const char* name, double v1, double v2, double v3) const
{
	glUniform3d(glGetUniformLocation(m_ID, name), v1, v2, v3);
}
void Shader::setDouble4(const char* name, double v1, double v2, double v3, double v4) const
{
	glUniform4d(glGetUniformLocation(m_ID, name), v1, v2, v3, v4);
}

// Vector
void Shader::setVec2(const char* name, const Point2d<float>& value) const
{
	glUniform2fv(glGetUniformLocation(m_ID, name), 1, reinterpret_cast<const float*>(&value));
}
void Shader::setVec3(const char* name, const Point3d<float>& value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name), 1, reinterpret_cast<const float*>(&value));
}
void Shader::setVec4(const char* name, const Point4d<float>& value) const
{
	glUniform4fv(glGetUniformLocation(m_ID, name), 1, reinterpret_cast<const float*>(&value));
}

// Matrix
void Shader::setMat4(const char* name, const Mat4<float>& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, value.data());
}

void Shader::setColor(const char* name, const Color3<float>& value) const
{
	glUniform3fv(glGetUniformLocation(m_ID, name), 1, reinterpret_cast<const float*>(&value));
}

GLuint Shader::createAndCompileShader(const char* shaderPath, GLuint shaderType)
{
	const std::string path = shaderFolderPath + shaderPath;
	const std::string shaderSource = Utils::StringFromFile(path);
	const char* cShaderSource = shaderSource.c_str();

	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, &cShaderSource, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << "Shader error: " << infoLog << std::endl;

		throw std::runtime_error("Shader error !");
	}

	return shader;
}