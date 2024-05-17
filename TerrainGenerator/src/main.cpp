#include <SFML/Window.hpp>
#include <GL/glew.h>
#include <SFML/OpenGL.hpp>
#include <SFML/Graphics.hpp>

#include <array>
#include <filesystem>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Texture.h"
#include "Plane.h"
#include "Mat4.h"

// Les tableaux de sommets et d'indices pour la skybox
constexpr std::array<float, 24> skyboxVertices = {
    // Vertex positions
    -1.0f, -1.0f,  1.0f,  // 0
     1.0f, -1.0f,  1.0f,  // 1
     1.0f, -1.0f, -1.0f,  // 2
    -1.0f, -1.0f, -1.0f,  // 3
    -1.0f,  1.0f,  1.0f,  // 4
     1.0f,  1.0f,  1.0f,  // 5
     1.0f,  1.0f, -1.0f,  // 6
    -1.0f,  1.0f, -1.0f   // 7
};

constexpr std::array<unsigned int, 36> skyboxIndices = {
    // Right face
    1, 2, 6,
    6, 5, 1,
    // Left face
    0, 4, 7,
    7, 3, 0,
    // Top face
    4, 5, 6,
    6, 7, 4,
    // Bottom face
    0, 3, 2,
    2, 1, 0,
    // Back face
    0, 1, 5,
    5, 4, 0,
    // Front face
    3, 7, 6,
    6, 2, 3
};

int main()
{
    const sf::ContextSettings settings(24, 8, 4, 4, 6);

    // crée la fenêtre
    sf::Window window(sf::VideoMode(800, 600), "OpenGL", sf::Style::Default, settings);
    window.setVerticalSyncEnabled(true);

    // activation de la fenêtre
    window.setActive(true);

    using TerrainF = Terrain<float>;

    // Initialisation de GLEW
    glewExperimental = GL_TRUE;
    glEnable(GL_DEPTH_TEST);
    if (glewInit())
        throw std::runtime_error("Error de merde");

    TerrainF terrain(100);

    float aspect = 800.f / 600.f;
    float fov = 45.f / 180.f * 3.141592f;
    float n = 0.1f;
    float f = 100.f;

    Mat4<float> P = Mat4<float>::projection(aspect, fov, n, f);

    float alpha = 0;
    float beta = 0;

    // Initialisation de la skybox
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVertices.size() * sizeof(float), skyboxVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, skyboxIndices.size() * sizeof(unsigned int), skyboxIndices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    std::string parentDir = "path/to/your/resources"; // Remplacez par le chemin correct
    std::array<std::string, 6> facesCubemap = {
        parentDir + "/skybox/right.jpg",
        parentDir + "/skybox/left.jpg",
        parentDir + "/skybox/top.jpg",
        parentDir + "/skybox/bottom.jpg",
        parentDir + "/skybox/front.jpg",
        parentDir + "/skybox/back.jpg"
    };

    unsigned int cubemapTexture;
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < facesCubemap.size(); i++) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(facesCubemap[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            stbi_set_flip_vertically_on_load(false);
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                width,
                height,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );
            stbi_image_free(data);
        }
        else {
            std::cout << "Failed to load texture: " << facesCubemap[i] << std::endl;
            stbi_image_free(data);
        }
    }

    // Charger les shaders pour le skybox
    Shader skyboxShader("path/to/skybox_vertex_shader.glsl", "path/to/skybox_fragment_shader.glsl");

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
        }

        // Effacement des tampons de couleur/profondeur
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Calcul de la matrice de vue
        glm::mat4 view = glm::mat4(1.0f);
        view = glm::rotate(view, -beta, glm::vec3(1.0f, 0.0f, 0.0f));
        view = glm::rotate(view, -alpha, glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glm::mat4 VP = projection * view;

        // Rendu du terrain
        terrain.renderTerrain(VP);

        // Rendu de la skybox
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();

        // Transmettre les matrices de vue et projection au shader
        view = glm::mat4(glm::mat3(view)); // Élimine la translation de la matrice de vue
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        // Dessin de la skybox
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawElements(GL_TRIANGLES, skyboxIndices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);

        // Affichage de la frame courante
        window.display();
    }

    // libération des ressources
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
    glDeleteBuffers(1, &skyboxEBO);
    glDeleteTextures(1, &cubemapTexture);

    return 0;
}
