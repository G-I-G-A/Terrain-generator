#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "MathHelper.h"

template<typename T>
struct Vertex
{
    Point3d<T> position;
    Point2d<T> texture;
};

template<typename T>
class Triangle
{
public:
    using vertex_type = Vertex<T>;

    Triangle(const vertex_type& p0, const vertex_type& p1, const vertex_type& p2)
        : m_points{ p0, p1, p2 }
        , m_shader("triangle.vert", "triangle.frag")
        , m_vao(0)
        , m_vbo(0)
        , m_texture("texture.bmp")
    {
        load();
    }

    ~Triangle()
    {

    }

    void load()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        glBufferData(GL_ARRAY_BUFFER, sizeof(m_points), m_points.data(), GL_STATIC_DRAW);

        m_shader.use();

        // /!\ Attention, ca ne marche que si T=float : c'est un peu dommage.
        glVertexAttribPointer(0, decltype(vertex_type::position)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, decltype(vertex_type::texture)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<char*>(nullptr) + sizeof(vertex_type::position));
        glEnableVertexAttribArray(1);
    }

    void update()
    {
        m_alpha += 0.025f;
    }

    void render(const Mat4<float>& VP)
    {
        Mat4<float> rot = Mat4<float>::rotationY(m_alpha);

        Mat4<float> trans = Mat4<float>::translation({ 3.f, 0.f, -5.f });

        Mat4<float> M = trans * rot;
        Mat4<float> MVP = VP * M;

        m_shader.use();
        glBindVertexArray(m_vao);

        m_shader.setMat4("MVP", MVP);

        m_texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, int(m_points.size()));
    }

private:
    Shader m_shader;
    Texture m_texture;
    std::array<vertex_type, 3> m_points;
    GLuint m_vao;
    GLuint m_vbo;
    float m_alpha = 0;
};

#endif TRIANGLE_H