#ifndef CUBE_H
#define CUBE_H

#include "MathHelper.h"
#include "Color3.h"

template<typename T>
struct CubeVertex
{
    Point3d<T> position;
    Point3d<T> normal;
    Color3<T> color;
};

template<typename T>
class Cube
{
public:
    using vertex_type = CubeVertex<T>;

    Cube()
        : m_shader("cube.vert", "cube.frag")
        , m_vao(0)
        , m_vbo(0)
    {
        load();
    }

    ~Cube()
    {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void load()
    {
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

        // Allocate storage size units of OpenGL
        // Copy data from client to server

        Point3d<T> P000 = { -1, -1, -1 }; Point3d<T> P100 = { +1, -1, -1 }; Point3d<T> P010 = { -1, +1, -1 }; Point3d<T> P110 = { +1, +1, -1 };
        Point3d<T> P001 = { -1, -1, +1 }; Point3d<T> P101 = { +1, -1, +1 }; Point3d<T> P011 = { -1, +1, +1 }; Point3d<T> P111 = { +1, +1, +1 };
        Color3<T> c100 = { +1, +0, +0 }; Color3<T> c010 = { +0, +1, +0 }; Color3<T> c001 = { +0, +0, +1 };
        Color3<T> c011 = { +0, +1, +1 }; Color3<T> c101 = { +1, +0, +1 }; Color3<T> c110 = { +1, +1, +0 };
        Point3d<T> nxn = { -1, 0, 0 }; Point3d<T> nyn = { 0, -1, 0 }; Point3d<T> nzn = { 0, 0, -1 };
        Point3d<T> nxp = { +1, 0, 0 }; Point3d<T> nyp = { 0, +1, 0 }; Point3d<T> nzp = { 0, 0, +1 };

        using vt = vertex_type;
        static std::array<vertex_type, 36> points = {
           vt{P000, nzn, c100}, vt{P100, nzn, c100}, vt{P110, nzn, c100}, vt{P000, nzn, c100}, vt{P110, nzn, c100}, vt{P010, nzn, c100}
         , vt{P001, nzp, c011}, vt{P101, nzp, c011}, vt{P111, nzp, c011}, vt{P001, nzp, c011}, vt{P111, nzp, c011}, vt{P011, nzp, c011}
         , vt{P000, nyn, c010}, vt{P100, nyn, c010}, vt{P101, nyn, c010}, vt{P000, nyn, c010}, vt{P101, nyn, c010}, vt{P001, nyn, c010}
         , vt{P010, nyp, c101}, vt{P110, nyp, c101}, vt{P111, nyp, c101}, vt{P010, nyp, c101}, vt{P111, nyp, c101}, vt{P011, nyp, c101}
         , vt{P000, nxn, c001}, vt{P010, nxn, c001}, vt{P011, nxn, c001}, vt{P000, nxn, c001}, vt{P011, nxn, c001}, vt{P001, nxn, c001}
         , vt{P100, nxp, c110}, vt{P110, nxp, c110}, vt{P111, nxp, c110}, vt{P100, nxp, c110}, vt{P111, nxp, c110}, vt{P101, nxp, c110}
        };

        glBufferData(GL_ARRAY_BUFFER, sizeof(points), points.data(), GL_STATIC_DRAW);

        m_shader.use();

        // /!\ Attention, ca ne marche que si T=float : c'est un peu dommage.
        glVertexAttribPointer(0, decltype(vertex_type::position)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, decltype(vertex_type::normal)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<char*>(nullptr) + sizeof(vertex_type::position));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, decltype(vertex_type::color)::ndim, GL_FLOAT, GL_FALSE, sizeof(vertex_type), reinterpret_cast<char*>(nullptr) + sizeof(vertex_type::position) + sizeof(vertex_type::normal));
        glEnableVertexAttribArray(2);
    }

    void update()
    {
        m_alpha += 0.025f;
    }

    void render(const Mat4<float>& VP)
    {
        struct OpticalProperties
        {
            float ambiant = 0.3f;
            float diffuse = 0.5f;
        } opticalProperties;

        struct DirectionalLight
        {
            Point3d<T> dir = { 0.0f, -1.0f, 0.0f };
            Color3<T> color = { 1.0f, 1.0f, 1.0f };
        } lightProperties;

        Mat4<float> rot = Mat4<float>::rotationY(m_alpha);

        Mat4<float> trans = Mat4<float>::translation({ 0.f, 0.f, -50.f });

        Mat4<float> M = trans * Mat4<float>::rotationX(beta) * Mat4<float>::rotationY(alpha);
        Mat4<float> MVP = VP * M;

        m_shader.use();
        glBindVertexArray(m_vao);

        m_shader.setMat4("MVP", MVP);
        m_shader.setMat4("model", M);
        m_shader.setFloat("mateiral.ambiant", opticalProperties.ambiant);
        m_shader.setFloat("mateiral.diffuse", opticalProperties.diffuse);
        m_shader.setVec3("light.dir", lightProperties.dir);
        m_shader.setColor("light.color", lightProperties.color);

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

public:
    float alpha = 0;
    float beta = 0;

private:
    Shader m_shader;
    GLuint m_vao;
    GLuint m_vbo;
    float m_alpha = 0;
};

#endif CUBE_H