#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <imgui-SFML.h>

#include <array>
#include <filesystem>

#include "Shader.h"
#include "Texture.h"
#include "Plane.h"
#include "Camera.h"

int main()
{
    const sf::ContextSettings settings(24, 8, 4, 4, 6);

    // crée la fenêtre
    sf::Clock clock;
    sf::RenderWindow  window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // activation de la fenêtre
    window.setActive(true);

    // Init ImGUI
    ImGui::SFML::Init(window);

    using TerrainF = Terrain<float>;

    // Les putains de lignes de l’enfer
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    if (glewInit())
        throw std::runtime_error("Error de merde");

    TerrainF terrain(100);
    Camera camera({ 0.0f, 0.0f, 3.0f });

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
            else if (event.type == sf::Event::KeyPressed)
            {
                if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::Up)
                    camera.ProcessKeyboardInput(CameraMovement::FORWARD, true);
                if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
                    camera.ProcessKeyboardInput(CameraMovement::BACKWARD, true);
                if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Left)
                    camera.ProcessKeyboardInput(CameraMovement::LEFT, true);
                if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
                    camera.ProcessKeyboardInput(CameraMovement::RIGHT, true);
                if (event.key.code == sf::Keyboard::Space)
                    camera.ProcessKeyboardInput(CameraMovement::UP, true);
                if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
                    camera.ProcessKeyboardInput(CameraMovement::DOWN, true);
            }
            else if (event.type == sf::Event::KeyReleased)
            {
                if (event.key.code == sf::Keyboard::Z || event.key.code == sf::Keyboard::Up)
                    camera.ProcessKeyboardInput(CameraMovement::FORWARD, false);
                if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down)
                    camera.ProcessKeyboardInput(CameraMovement::BACKWARD, false);
                if (event.key.code == sf::Keyboard::Q || event.key.code == sf::Keyboard::Left)
                    camera.ProcessKeyboardInput(CameraMovement::LEFT, false);
                if (event.key.code == sf::Keyboard::D || event.key.code == sf::Keyboard::Right)
                    camera.ProcessKeyboardInput(CameraMovement::RIGHT, false);
                if (event.key.code == sf::Keyboard::Space)
                    camera.ProcessKeyboardInput(CameraMovement::UP, false);
                if (event.key.code == sf::Keyboard::LShift || event.key.code == sf::Keyboard::RShift)
                    camera.ProcessKeyboardInput(CameraMovement::DOWN, false);
            }
        }

        float deltaTime = clock.restart().asSeconds();
        camera.ProcessKeyboardInputs(deltaTime);

        // Effacement des tampons de couleur/profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Mat4<float> V = camera.GetViewMatrix();

        // Calcul de la matrice de vue
        Mat4<float> V = Mat4<float>::rotationX(-beta) * Mat4<float>::rotationY(-alpha);
        Mat4<float> P = camera.GetProjectionMatrix(800, 600);

        // Calcul de la matrice de vue-projection
        Mat4<float> VP = P * V;

        // Rendu du terrain
        terrain.renderTerrain(VP);

        // Affichage de la frame courante
        window.display();
    }

    // libération des ressources...
    ImGui::SFML::Shutdown();

    return 0;
}
