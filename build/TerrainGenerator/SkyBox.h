#pragma once
#include "iostream"
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
class SkyBox
{
public:

	SkyBox();

	virtual ~SkyBox();

	// coordinate
	float SkyBoxVertices[24] =
	{
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f
	};

	unsigned int SkyBoxIndices[36] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,

		// Left
		0, 4, 7,
		7, 3, 0,

		// Top
		4, 5, 6,
		6, 7, 4,

		// Bottom
		0, 3, 2,
		2, 1, 0,

		// Back
		0, 1, 5,
		5, 4, 1,

		// Front
		3, 7, 6,
		6, 2, 3
	};

	unsigned int skyboxVAO;
	unsigned int skyboxVBO;
	unsigned int skyboxEBO;

	std::string FacesCubemap[6] =
	{
		"image1", "image2", "image3",
		"image4", "image5", "image6"
	};
		
};

