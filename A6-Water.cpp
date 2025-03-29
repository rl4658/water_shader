// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM for matrix computations
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include your mesh and camera headers
#include "PlaneMesh.hpp"
#include "ThirdPersonCamera.hpp"

int main(int argc, char* argv[])
{
    float screenW = 1200;
    float screenH = 700;
    float stepsize = 1.0f;
    float xmin = -10;
    float xmax = 10;

    if (argc > 1) {
        screenW = atof(argv[1]);
    }
    if (argc > 2) {
        screenH = atof(argv[2]);
    }
    if (argc > 3) {
        stepsize = atof(argv[3]);
    }
    if (argc > 4) {
        xmin = atof(argv[4]);
    }
    if (argc > 5) {
        xmax = atof(argv[5]);
    }

    // Initialize GLFW
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(screenW, screenH, "A6 Water", NULL, NULL);
    if (window == NULL)
    {
        fprintf(stderr, "Failed to open GLFW window.\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        fprintf(stderr, "Failed to initialize GLEW\n");
        glfwTerminate();
        return -1;
    }

    // Register camera callbacks with GLFW
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);

    // Initialize your scene mesh (e.g., water plane)
    PlaneMesh plane(xmin, xmax, stepsize);

    // Enable sticky keys
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Set clear color and disable face culling
    glClearColor(0.2f, 0.2f, 0.3f, 0.0f);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glDisable(GL_CULL_FACE);

    // Set up projection matrix
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenW / screenH, 0.001f, 1000.0f);

    // Light position for shading
    glm::vec3 lightpos(5.0f, 30.0f, 5.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Main render loop
    while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
           !glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the view matrix using the third-person camera
        glm::mat4 V = updateCameraView();

        // Render the plane with the updated view and projection matrices
        plane.draw(lightpos, V, Projection);

        // Swap buffers and poll for events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up and terminate GLFW
    glfwTerminate();
    return 0;
}
