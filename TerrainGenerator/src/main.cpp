#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <array>
#include <filesystem>

#include "Shader.h"
#include "Texture.h"
#include "Triangle.h"
#include "Cube.h"
#include "Plane.h"

int main()
{
    const sf::ContextSettings settings(24, 8, 4, 4, 6);

    // crée la fenêtre
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // activation de la fenêtre
    window.setActive(true);

    using CubeF = Cube<float>;

    // Les putains de lignes de l’enfer
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    if (glewInit())
        throw std::runtime_error("Error de merde");

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

    TerrainGenerator terrainGenerator(100);
    terrainGenerator.generateTerrain();

    while (running)
    {
        // Gestion des événements
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                // Fermeture de la fenêtre
                running = false;
            }
            else if (event.type == sf::Event::Resized)
            {
                // Ajustement du viewport lorsque la fenêtre est redimensionnée
                glViewport(0, 0, event.size.width, event.size.height);
            }
            else if (event.type == sf::Event::MouseMoved)
            {
                // Déplacement de la souris (rotation de la caméra)
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
                // Bouton de la souris enfoncé
                if (event.mouseButton.button == sf::Mouse::Left)
                    leftMouseButtonPressed = true;
            }
            else if (event.type == sf::Event::MouseButtonReleased)
            {
                // Bouton de la souris relâché
                if (event.mouseButton.button == sf::Mouse::Left)
                    leftMouseButtonPressed = false;
            }
        }

        // Effacement des tampons de couleur/profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calcul de la matrice de vue
        Mat4<float> V = Mat4<float>::rotationX(-beta) * Mat4<float>::rotationY(-alpha);

        // Calcul de la matrice de projection
        float aspect = 800.f / 600.f;
        float fov = 45.f / 180.f * 3.141592f;
        float n = 0.1f;
        float f = 100.f;
        Mat4<float> P = Mat4<float>::projection(aspect, fov, n, f);

        // Calcul de la matrice de vue-projection
        Mat4<float> VP = P * V;

        // Rendu du terrain
        terrainGenerator.renderTerrain(VP);

        // Rendu du cube
        cube.update();
        cube.render(VP);

        // Affichage de la trame courante
        window.display();
    }

    // libération des ressources...

    return 0;
}
