#include "ThirdPersonCamera.hpp"
#include <cmath>

// Initialize camera parameters
float cameraDistance = 5.0f;
float cameraYaw = 0.0f;    // starts facing a default direction
float cameraPitch = 45.0f; // moderate angle for a "top-down" view

// Variables to track mouse movement
static double lastMouseX = 0.0, lastMouseY = 0.0;
static bool isLeftMouseButtonPressed = false;

// Adjusts camera distance (zoom) when scrolling
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    cameraDistance -= static_cast<float>(yoffset);
    if (cameraDistance < 1.0f)
        cameraDistance = 1.0f; // Prevent zooming too close
}

// Tracks left mouse button press/release
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            isLeftMouseButtonPressed = true;
            glfwGetCursorPos(window, &lastMouseX, &lastMouseY);
        } else if (action == GLFW_RELEASE) {
            isLeftMouseButtonPressed = false;
        }
    }
}

// Updates camera yaw and pitch based on mouse movement when dragging
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (isLeftMouseButtonPressed) {
        float xoffset = static_cast<float>(xpos - lastMouseX);
        float yoffset = static_cast<float>(ypos - lastMouseY);
        const float sensitivity = 0.1f; // Adjust sensitivity as needed
        
        cameraYaw   += xoffset * sensitivity;
        cameraPitch += yoffset * sensitivity;
        
        // Clamp the pitch to avoid flipping over
        if (cameraPitch > 89.0f)
            cameraPitch = 89.0f;
        if (cameraPitch < -89.0f)
            cameraPitch = -89.0f;
        
        lastMouseX = xpos;
        lastMouseY = ypos;
    }
}

// Computes the view matrix using spherical coordinates to position the camera
glm::mat4 updateCameraView() {
    float yawRad   = glm::radians(cameraYaw);
    float pitchRad = glm::radians(cameraPitch);
    
    glm::vec3 cameraPos;
    cameraPos.x = cameraDistance * cos(pitchRad) * sin(yawRad);
    cameraPos.y = cameraDistance * sin(pitchRad);
    cameraPos.z = cameraDistance * cos(pitchRad) * cos(yawRad);
    
    glm::vec3 target(0.0f, 0.0f, 0.0f);
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    
    return glm::lookAt(cameraPos, target, up);
}
