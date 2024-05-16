#include "../include/plane.h"
#include "../include/Texture.h"

#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <cmath>

TerrainGenerator::TerrainGenerator(int size) 
    : m_size(size)
{
    // Initialize OpenGL objects
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    generateTerrain();
}

TerrainGenerator::~TerrainGenerator() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteVertexArrays(1, &m_vao);
}

void TerrainGenerator::generateTerrain() {
    // Generate terrain data
    generateMap();

    // Bind VBO and buffer terrain data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * m_map.size(), m_map.data(), GL_STATIC_DRAW);
}

void TerrainGenerator::renderTerrain(const Mat4<float>& VP) {
    glBindVertexArray(m_vao);
    glEnableVertexAttribArray(0);

    // Set up shader program and texture
    glUseProgram(m_program);

    // Set up MVP matrix
    GLuint mvpLocation = glGetUniformLocation(m_program, "MVP");
    glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, VP.data());

    // Draw terrain
    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>((m_size - 1) * (m_size - 1) * 6));
}

void TerrainGenerator::generateMap() {
    // Generate terrain heights
    m_map.resize(m_size * m_size);

    float step = 2.0f / (m_size - 1);
    float x, z;

    for (int i = 0; i < m_size; ++i) {
        for (int j = 0; j < m_size; ++j) {
            x = -1.0f + j * step;
            z = -1.0f + i * step;

            // Define a simple height function (e.g., sine wave)
            m_map[i * m_size + j] = std::sin(x * 3.14f) * std::cos(z * 3.14f);
        }
    }

    // Generate terrain geometry
    // Each grid cell is represented by two triangles
    std::vector<float> vertices;
    for (int i = 0; i < m_size - 1; ++i) {
        for (int j = 0; j < m_size - 1; ++j) {
            // Triangle 1
            vertices.push_back(-1.0f + j * step);
            vertices.push_back(m_map[i * m_size + j]);
            vertices.push_back(-1.0f + i * step);

            vertices.push_back(-1.0f + (j + 1) * step);
            vertices.push_back(m_map[i * m_size + j + 1]);
            vertices.push_back(-1.0f + i * step);

            vertices.push_back(-1.0f + (j + 1) * step);
            vertices.push_back(m_map[(i + 1) * m_size + j + 1]);
            vertices.push_back(-1.0f + (i + 1) * step);

            // Triangle 2
            vertices.push_back(-1.0f + j * step);
            vertices.push_back(m_map[i * m_size + j]);
            vertices.push_back(-1.0f + i * step);

            vertices.push_back(-1.0f + (j + 1) * step);
            vertices.push_back(m_map[(i + 1) * m_size + j + 1]);
            vertices.push_back(-1.0f + (i + 1) * step);

            vertices.push_back(-1.0f + j * step);
            vertices.push_back(m_map[(i + 1) * m_size + j]);
            vertices.push_back(-1.0f + (i + 1) * step);
        }
    }

    // Bind VBO and buffer terrain data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}