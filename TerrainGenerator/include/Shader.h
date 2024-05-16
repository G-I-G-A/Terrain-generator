#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include "MathHelper.h"
#include "Color3.h"

class Shader
{
public:
	Shader(const char* vertexPath, const char* fragmentPath);
	~Shader();

	void use() const;

	const GLuint& getID() const;

	// Utility uniform functions
	// Bool
	void setBool(const char* name, bool v) const;
	void setBool2(const char* name, bool v1, bool v2) const;
	void setBool3(const char* name, bool v1, bool v2, bool v3) const;
	void setBool4(const char* name, bool v1, bool v2, bool v3, bool v4) const;

	// Int
	void setInt(const char* name, int v) const;
	void setInt2(const char* name, int v1, int v2) const;
	void setInt3(const char* name, int v1, int v2, int v3) const;
	void setInt4(const char* name, int v1, int v2, int v3, int v4) const;

	// Float
	void setFloat(const char* name, float v) const;
	void setFloat2(const char* name, float v1, float v2) const;
	void setFloat3(const char* name, float v1, float v2, float v3) const;
	void setFloat4(const char* name, float v1, float v2, float v3, float v4) const;

	// Double
	void setDouble(const char* name, double v) const;
	void setDouble2(const char* name, double v1, double v2) const;
	void setDouble3(const char* name, double v1, double v2, double v3) const;
	void setDouble4(const char* name, double v1, double v2, double v3, double v4) const;

	// Vector
	void setVec2(const char* name, const Point2d<float>& value) const;
	void setVec3(const char* name, const Point3d<float>& value) const;
	void setVec4(const char* name, const Point4d<float>& value) const;

	// Matrix
	void setMat4(const char* name, const Mat4<float>& value) const;

	// Color
	void setColor(const char* name, const Color3<float>& value) const;

private:
	static GLuint createAndCompileShader(const char* shaderPath, GLuint shaderType);

	GLuint m_ID;
};

#endif SHADER_H