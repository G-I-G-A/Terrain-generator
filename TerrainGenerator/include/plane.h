#ifndef PLANE_H
#define PLANE_H

#include <vector>
#include <GL/glew.h>

#include "Texture.h"
#include "Color3.h"
#include "MathHelper.h"
#include "Shader.h"
#include "PerlinNoise.h"

template<typename T>
struct PlaneVertex
{
    Point3d<T> position;
    // Color3<T> color;
};

template<typename T>
class Terrain
{
public:
    using vertex_type = PlaneVertex<T>;

    Terrain(int size)
        : m_size(size)
        , m_shader("plane.vert", "plane.frag")
    {
        load();
    }
    ~Terrain()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void load()
    {
        // Initialize OpenGL objects
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        generateTerrain();
    }
    void generateTerrain()
    {
        float step = 4.0f / (m_size - 1);
        generateMap(step);

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

        m_shader.use();

        glVertexAttribPointer(0, decltype(vertex_type::position)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), 0);
        glEnableVertexAttribArray(0);
        // glVertexAttribPointer(1, decltype(vertex_type::color)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<char*>(nullptr) + sizeof(vertex_type::position));
        // glEnableVertexAttribArray(1);
    }

    void renderTerrain(const Mat4<float>& VP)
    {
        glBindVertexArray(m_vao);
        glEnableVertexAttribArray(0);

        // Set up shader program and texture
        m_shader.use();
        glBindVertexArray(m_vao);

        // Set up MVP matrix
        m_shader.setMat4("MVP", VP);

        // Draw terrain
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>((m_size - 1) * (m_size - 1) * 6));
    }

private:
    Shader m_shader;
    int m_size;
    std::vector<float> m_map;
    GLuint m_vao;
    GLuint m_vbo;

    void generateMap(const float& step)
    {
        // Generate terrain heights
        m_map.resize(m_size * m_size);
        float x, z;

        for (int i = 0; i < m_size; ++i) {
            for (int j = 0; j < m_size; ++j) {
                x = -1.0f + j * step;
                z = -1.0f + i * step;

                // Define a simple height function (e.g., sine wave)
                m_map[i * m_size + j] = perlin(x, z, 0);
            }
        }
    }
};

#endif PLANE_H