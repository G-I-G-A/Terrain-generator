#ifndef PLANE_H
#define PLANE_H

#include "Texture.h"
#include "MathHelper.h"

#include <vector>
#include <GL/glew.h>

class TerrainGenerator 
{
public:
    TerrainGenerator(int size);
    ~TerrainGenerator();

    void generateTerrain();
    void renderTerrain(const Mat4<float>& VP);

private:
    int m_size;
    std::vector<float> m_map;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_program;

    void generateMap();
};

#endif PLANE_H