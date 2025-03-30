#ifndef A6_WATER_HPP
#define A6_WATER_HPP

// Standard and OpenGL/GLM includes
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <algorithm>

// Global GLFW window pointer (needed by some functions)
extern GLFWwindow *pMainWin;

// -----------------------------
// Texture Class Declaration
class Texture
{
public:
    // Loads a texture from file using stb_image.
    // (Implementation is in the cpp file.)
    static GLuint loadImage(const std::string &path, bool useMipmaps = true);
};

// -----------------------------
// Shader Class Declaration
class Shader
{
public:
    GLuint shaderProgram;
    Shader();
    ~Shader();

    // Load and compile shaders from the provided file paths.
    bool load(const std::string &vertFile,
              const std::string &tcsFile,
              const std::string &tesFile,
              const std::string &geomFile,
              const std::string &fragFile);

    void use() const;
    GLuint getShaderProgram() const;

    // Uniform setters.
    void setMat4(const std::string &uniformName, const glm::mat4 &mat) const;
    void setVec3(const std::string &uniformName, const glm::vec3 &vector) const;
    void setVec4(const std::string &uniformName, const glm::vec4 &vector) const;
    void setFloat(const std::string &uniformName, float floatValue) const;
    void setInt(const std::string &uniformName, int intValue) const;
    void setVec2(const std::string &uniformName, const glm::vec2 &vector) const;

private:
    // Helper functions.
    GLuint compile(const std::string &filePath, GLenum type);
    std::string readShaderFile(const std::string &fileName);
};

// -----------------------------
// Camera Class Declaration
class Camera
{
public:
    float radius, angleTheta, anglePhi;
    glm::vec3 targetPosition;
    glm::vec3 upVector;
    glm::vec3 camPosition;

    // Constructor: initializes with spherical coordinates (radius, theta in degrees, phi in degrees).
    Camera(float rad, float degTheta, float degPhi);

    // Update camera based on mouse movement.
    void processMouseMovement(float offsetX, float offsetY);
    // Zoom in/out.
    void processKeyboardInput(bool isZoomIn);
    // Get the view matrix.
    glm::mat4 getViewMatrix() const;
    // Get current camera position.
    glm::vec3 getPosition() const;
    // Update internal position from spherical coordinates.
    void updateCameraPosition();
};

// -----------------------------
// Callbacks Namespace Declaration
namespace Callbacks
{
    void cursorPosCallback(GLFWwindow *win, double posX, double posY);
    void mouseButtonCallback(GLFWwindow *win, int btn, int act, int modifiers);
    void scrollCallback(GLFWwindow *win, double offsetX, double offsetY);
    void keyCallback(GLFWwindow *win, int keyCode, int scanCode, int act, int modifiers);
    // Register all callbacks and assign the Camera pointer to the GLFW window.
    void initializeCallbacks(GLFWwindow *win, Camera *camPtr);
}

// -----------------------------
// PlaneMesh Class Declaration
// Builds a quad grid mesh (with normals) from (lowerBound,0,lowerBound) to (upperBound,0,upperBound)
class PlaneMesh
{
public:
    std::vector<float> vertices;
    std::vector<float> normalVectors;
    std::vector<int> indexData;
    float lowerBound, upperBound;
    GLuint vertexArrayObj, vertexBufferObj, normalBufferObj, elementBufferObj;
    int vertexCount, indexCount;

    // Constructor: creates the mesh from the specified bounds and step size.
    PlaneMesh(float lowerBound, float upperBound, float step);

    // Draws the mesh. Assumes the shader is already bound and uniforms are set.
    void draw(glm::vec3 lightPosition, glm::mat4 viewMat, glm::mat4 projMat);

private:
    // Generate vertex and index data.
    void generateMesh(float lowerBound, float upperBound, float step);
    // Setup VAO and VBOs.
    void setupBuffers();
};

// -----------------------------
// Renderer Class Declaration
// Handles window/context initialization, shader and texture loading,
// callback registration, and the main render loop.
class Renderer
{
public:
    float screenWidth, screenHeight, stepSize, xMin, xMax;
    float cameraRadius, cameraTheta, cameraPhi;
    float tessellationOuter, tessellationInner;
    float texScale;
    std::string waterTexturePath, displacementTexturePath;
    float heightMultiplier;
    GLFWwindow *mainWindow;

    // Constructor with configuration parameters.
    Renderer(float scrWidth, float scrHeight, float stepSize, float xMin, float xMax,
             float cameraRadius, float cameraTheta, float cameraPhi,
             float tessOuter, float tessInner,
             float texScale,
             const char *waterTex, const char *dispTex,
             float heightMul);

    // Initializes GLFW, GLEW, window, and OpenGL state.
    void init();
    // Runs the main rendering loop.
    void run();
    // Main loop.
    void mainLoop();
    // Cleanup and terminate.
    void cleanup();
};

#endif // A6_WATER_HPP
