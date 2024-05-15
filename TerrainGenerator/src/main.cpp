#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include "Shader.h"
#include "MathHelper.h"

#include <array>
#include <filesystem>


template<typename T>
struct Color3
{
    static constexpr int ndim = 3;

    Color3(const T& r_ = 0, const T& g_ = 0, const T& b_ = 0)
        : r(r_)
        , g(g_)
        , b(b_)
    {}

    Color3(const Color3& pt)
        : r(pt.r)
        , g(pt.g)
        , b(pt.b)
    {}

    T r, g, b;
};

template<typename T>
struct Vertex
{
    Point3d<T> position;
    Point2d<T> texture;
};

struct Texture
{
    explicit Texture(const std::filesystem::path& path)
    {
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        sf::Image image;
        image.loadFromFile(path.generic_string());
        auto size = image.getSize();

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.x, size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.getPixelsPtr());
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void bind()
    {
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

private:
    GLuint m_texture;
};

template<typename T>
class Triangle
{
public:
    using vertex_type = Vertex<T>;

    Triangle(const vertex_type& p0, const vertex_type& p1, const vertex_type& p2)
        : m_points{p0, p1, p2}
        , m_vao(0)
        , m_vbo(0)
        , m_texture("D:\\Cours\\Master2\\C++\\Terrain-generator\\Resources\\Textures\\texture.bmp")
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

        ShaderInfo shaders[] = {
            {GL_VERTEX_SHADER, "D:\\Cours\\Master2\\C++\\Terrain-generator\\Resources\\Shaders\\triangle.vert"},
            {GL_FRAGMENT_SHADER, "D:\\Cours\\Master2\\C++\\Terrain-generator\\Resources\\Shaders\\triangle.frag"},
            {GL_NONE, nullptr}
        };

        m_program = Shader::loadShaders(shaders);
        glUseProgram(m_program);

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

        Mat4<float> trans = Mat4<float>::translation({3.f, 0.f, -5.f});

        Mat4<float> M = trans * rot;
        Mat4<float> MVP = VP * M;

        glUseProgram(m_program);
        glBindVertexArray(m_vao);

        GLuint mvpLocation = glGetUniformLocation(m_program, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, 0, MVP.data());

        m_texture.bind();
        glDrawArrays(GL_TRIANGLES, 0, int(m_points.size()));
    }

private:
    Texture m_texture;
    std::array<vertex_type, 3> m_points;
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_program;
    float m_alpha = 0;
};



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
        : m_vao(0)
        , m_vbo(0)
    {
        load();
    }

    ~Cube()
    {

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

        ShaderInfo shaders[] = {
            {GL_VERTEX_SHADER, "D:\\Cours\\Master2\\C++\\Terrain-generator\\Resources\\Shaders\\cube.vert"},
            {GL_FRAGMENT_SHADER, "D:\\Cours\\Master2\\C++\\Terrain-generator\\Resources\\Shaders\\cube.frag"},
            {GL_NONE, nullptr}
        };

        m_program = Shader::loadShaders(shaders);
        glUseProgram(m_program);

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

        glUseProgram(m_program);
        glBindVertexArray(m_vao);

        GLuint mvpLocation = glGetUniformLocation(m_program, "MVP");
        glUniformMatrix4fv(mvpLocation, 1, 0, MVP.data());

        GLuint modelLocation = glGetUniformLocation(m_program, "model");
        glUniformMatrix4fv(modelLocation, 1, 0, M.data());

        GLuint ambiant = glGetUniformLocation(m_program, "material.ambiant");
        glUniform1fv(ambiant, 1, &opticalProperties.ambiant);

        GLuint diffuse = glGetUniformLocation(m_program, "material.diffuse");
        glUniform1fv(diffuse, 1, &opticalProperties.diffuse);

        GLuint lightDir = glGetUniformLocation(m_program, "light.dir");
        glUniform3fv(lightDir, 1, reinterpret_cast<float*>(&lightProperties.dir));

        GLuint lightColor = glGetUniformLocation(m_program, "light.color");
        glUniform3fv(lightColor, 1, reinterpret_cast<float*>(&lightProperties.color));

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }

public:
    float alpha = 0;
    float beta = 0;

private:
    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_program;
    float m_alpha = 0;
};

int main()
{
    const sf::ContextSettings settings(24, 8, 4, 4, 6);

    // crée la fenêtre
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // activation de la fenêtre
    window.setActive(true);

    using VertexF = Vertex<float>;
    using TriangleF = Triangle<float>;
    using CubeF = Cube<float>;

    // Les putains de lignes de l’enfer
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    if (glewInit())
        throw std::runtime_error("Error de merde");

    VertexF p0{ { -0.9f, -0.9f, 0.f }, {-0.9f, 0.9f} };
    VertexF p1{ { 0.9f, -0.9f, 0.f }, {0.9f, 0.9f} };
    VertexF p2{ { 0.9f, 0.9f, 0.f }, {0.9f, -0.9f} };

    TriangleF triangle(p0, p1, p2);
    CubeF cube;

    float aspect = 800.f / 600.f;
    float fov = 45.f / 180.f * 3.141592f;
    float n = 0.1f;
    float f = 100.f;

    Mat4<float> P = Mat4<float>::projection(aspect, fov, n, f);

    float alpha = 0;
    float beta = 0;

    // la boucle principale
    bool running = true;
    bool leftMouseButtonPressed = false;

    sf::Mouse::setPosition({ 400, 300 }, window);

    while (running)
    {
        // gestion des évènements
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // on stoppe le programme
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // on ajuste le viewport lorsque la fen�tre est redimensionn�e
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                float dx = event.mouseMove.x - 400.f;
                float dy = event.mouseMove.y - 300.f;

                sf::Mouse::setPosition({ 400, 300 }, window);

                float coef = 0.001f;

                if (!leftMouseButtonPressed)
                {
                    alpha += coef * dx;
                    beta += -coef * dy;
                }
                else
                {
                    cube.alpha += coef * dx;
                    cube.beta += -coef * dy;
                }
            }
            else if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                    leftMouseButtonPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                    leftMouseButtonPressed = false;
            }
        }

        // effacement les tampons de couleur/profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4<float> V = Mat4<float>::rotationX(-beta) * Mat4<float>::rotationY(-alpha);
        auto VP = P * V;

        // dessin...
        triangle.update();
        triangle.render(VP);

        cube.update();
        cube.render(VP);

        glFlush();

        // termine la trame courante (en interne, échange les deux tampons de rendu)
        window.display();
    }

    // libération des ressources...

    return 0;
}