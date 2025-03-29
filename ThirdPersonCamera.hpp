#ifndef THIRDPERSONCAMERA_HPP
#define THIRDPERSONCAMERA_HPP

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Global camera parameters (adjust as needed)
extern float cameraDistance;
extern float cameraYaw;    // Horizontal angle in degrees
extern float cameraPitch;  // Vertical angle in degrees

// Callback functions to handle mouse scroll, button, and motion
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

// Computes and returns the view matrix based on current camera parameters
glm::mat4 updateCameraView();

#endif // THIRDPERSONCAMERA_HPP
