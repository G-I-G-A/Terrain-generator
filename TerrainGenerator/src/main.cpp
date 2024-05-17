#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>
#include <imgui.h>
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
    sf::RenderWindow window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
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

    float lastMouseX;
    float lastMouseY;

    // la boucle principale
    bool running = true;
    bool firstMouse = true;

    sf::Mouse::setPosition({ 400, 300 }, window);

    while (running)
    {
        // Gestion des événements
        sf::Event event;
        while (window.pollEvent(event))
        {
            ImGui::SFML::ProcessEvent(event);

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
                float xPos = event.mouseMove.x;
                float yPos = event.mouseMove.y;

                if (firstMouse)
                {
                    lastMouseX = xPos;
                    lastMouseY = yPos;

                    firstMouse = false;
                }

                float xOffset = xPos - lastMouseX;
                float yOffset = yPos - lastMouseY;
                lastMouseX = xPos;
                lastMouseY = yPos;

                camera.ProcessMouseMovementInputs(xOffset, yOffset, false);
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

        sf::Time deltaTime = clock.restart();
        camera.ProcessKeyboardInputs(deltaTime.asSeconds());

        // Effacement des tampons de couleur/profondeur 
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Mat4<float> V = camera.GetViewMatrix();
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
