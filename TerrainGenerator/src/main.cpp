#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <array>
#include <filesystem>

#include "Shader.h"
#include "Plane.h"
#include "Camera.h"
#include <iostream>

// Screen settings
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Camera
Camera camera({ 7.f, -10.f, 25.f }, { 0.f, 1.f, 0.f }, -90, 25);

// Mouse settings
bool freeCamera = false;
bool freeCameraButtonPressed = false;
bool firstMouse = true;
float lastMouseX = SCREEN_WIDTH / 2.0f;
float lastMouseY = SCREEN_HEIGHT / 2.0f;

// Cursor settings
bool cursorShown = true;

// Time
float currentTime, lastFrameTime = glfwGetTime();
float deltaTime = 0;

// seed
int seed = 0;

void SetWindowHints()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
}

void TerminateWindow(GLFWwindow* window)
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void ProcessInputs(GLFWwindow* window)
{
    // Close the game on espace
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }

    // Process Polygon mode changes
    if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS && freeCameraButtonPressed == false)
    {
        freeCameraButtonPressed = true;
        freeCamera = !freeCamera;
        if (freeCamera)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE)
        freeCameraButtonPressed = false;

    // Process Camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::FORWARD);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::BACKWARD);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::LEFT);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::DOWN);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboardInputs(CameraMovement::UP);
}

void HandleMouseCallback(GLFWwindow* window, double xPos, double yPos)
{
    if (freeCamera)
    {
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

        camera.ProcessMouseMovementInputs(xOffset, yOffset);
    }
}

void HandlescrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
    // camera.ProcessMouseScrollInputs(yOffset);
}

void HandleFramebufferSize(GLFWwindow* windo, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "GLFW Initialisation failed." << std::endl;

        return -1;
    }

    // Initialise GLFW Context
    SetWindowHints();

    // Creating window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Simulation", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "GLFW Window creation failed." << std::endl;
        glfwTerminate();

        return -2;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, HandleFramebufferSize);
    glfwSetCursorPosCallback(window, HandleMouseCallback);
    glfwSetScrollCallback(window, HandlescrollCallback);

    // Initialise GLEW
    glewExperimental = GLFW_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Glew Initialisation failed." << std::endl;
        TerminateWindow(window);

        return -3;
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    // Enable alpha values
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable depth values
    glEnable(GL_DEPTH_TEST);

    // Initialise ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    using TerrainF = Terrain<float>;
    TerrainF terrain(100);

    while (!glfwWindowShouldClose(window))
    {
        lastFrameTime = currentTime;
        currentTime = glfwGetTime();
        deltaTime = currentTime - lastFrameTime;

        camera.SetDeltaTime(deltaTime);

        // Clear render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Inputs
        ProcessInputs(window);

        Mat4<float> V = camera.GetViewMatrix();
        Mat4<float> P = camera.GetProjectionMatrix(SCREEN_WIDTH, SCREEN_HEIGHT);

        // Calcul de la matrice de vue-projection
        Mat4<float> VP = P * V;

        // Rendu du terrain
        terrain.renderTerrain(VP);

        // ImGUI new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Imgui render
        ImGui::Begin("Configs");
        std::string fps = "FPS: " + std::to_string(static_cast<int>(1.f / deltaTime));
        ImGui::Text(fps.c_str()); 

        ImGui::Separator();
        
        ImGui::SliderInt("Integer Slider", &seed, 0, 1000);
        if (ImGui::Button("Regenerate Terrain"))
        {
            terrain.generateTerrain(seed);
        }

        ImGui::Separator();
        ImGui::Text("Escape: Close");
        ImGui::Text("Z: Forward");
        ImGui::Text("Q: Left");
        ImGui::Text("S: BackWard");
        ImGui::Text("D: Right");
        ImGui::Text("Space: Up");
        ImGui::Text("LShift: Down");
        ImGui::Text("F1: Polygon Mode");
        ImGui::Text("F2: Wireframe Mode");
        ImGui::Text("F3: Point Mode");
        ImGui::Text("F5: Free Camera ON / OFF");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap buffer with the front buffer
        glfwSwapBuffers(window);
        // Take care of GLFW events
        glfwPollEvents();
    }

    // Terminate ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    TerminateWindow(window);
    return 0;
}
