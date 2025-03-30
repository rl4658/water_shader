// A6-Water.cpp
// ----------------------------------------------------
// Include the combined header
#include "A6-Water.hpp"

// ----------------------------------------------------
// STB Image Implementation (for texture loading)
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// ----------------------------------------------------
// Texture Class Implementation
GLuint Texture::loadImage(const std::string &filepath, bool generateMipmaps)
{
    int width, height, channels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(filepath.c_str(), &width, &height, &channels, 0);
    if (!data)
    {
        std::cerr << "stb_image failed to load: " << filepath << std::endl;
        return 0;
    }
    GLenum format = GL_RGB;
    if (channels == 1)
        format = GL_RED;
    else if (channels == 3)
        format = GL_RGB;
    else if (channels == 4)
        format = GL_RGBA;
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    if (generateMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    stbi_image_free(data);
    return textureID;
}

// ----------------------------------------------------
// Shader Class Implementation
Shader::Shader() : shaderProgram(0) {}
Shader::~Shader()
{
    if (shaderProgram != 0)
        glDeleteProgram(shaderProgram);
}

bool Shader::load(const std::string &vertexPath,
                  const std::string &tcsPath,
                  const std::string &tesPath,
                  const std::string &geomPath,
                  const std::string &fragPath)
{
    GLuint vs = compile(vertexPath, GL_VERTEX_SHADER);
    GLuint tcs = compile(tcsPath, GL_TESS_CONTROL_SHADER);
    GLuint tes = compile(tesPath, GL_TESS_EVALUATION_SHADER);
    GLuint gs = compile(geomPath, GL_GEOMETRY_SHADER);
    GLuint fs = compile(fragPath, GL_FRAGMENT_SHADER);
    if (!vs || !tcs || !tes || !gs || !fs)
        return false;

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vs);
    glAttachShader(shaderProgram, tcs);
    glAttachShader(shaderProgram, tes);
    glAttachShader(shaderProgram, gs);
    glAttachShader(shaderProgram, fs);
    glLinkProgram(shaderProgram);

    GLint success;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader linking failed:\n"
                  << infoLog << std::endl;
        return false;
    }
    glDeleteShader(vs);
    glDeleteShader(tcs);
    glDeleteShader(tes);
    glDeleteShader(gs);
    glDeleteShader(fs);
    return true;
}

void Shader::use() const { glUseProgram(shaderProgram); }
GLuint Shader::getShaderProgram() const { return shaderProgram; }
void Shader::setMat4(const std::string &name, const glm::mat4 &matrix) const
{
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, GL_FALSE, &matrix[0][0]);
}
void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const
{
    glUniform3fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}
void Shader::setVec4(const std::string &name, const glm::vec4 &vec) const
{
    glUniform4fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}
void Shader::setFloat(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setInt(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(shaderProgram, name.c_str()), value);
}
void Shader::setVec2(const std::string &name, const glm::vec2 &vec) const
{
    glUniform2fv(glGetUniformLocation(shaderProgram, name.c_str()), 1, &vec[0]);
}

GLuint Shader::compile(const std::string &path, GLenum type)
{
    std::string code = readShaderFile(path);
    if (code.empty())
        return 0;
    const char *src = code.c_str();
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation error in " << path << ":\n"
                  << infoLog << std::endl;
        return 0;
    }
    return shader;
}

std::string Shader::readShaderFile(const std::string &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Could not open shader file: " << path << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// ----------------------------------------------------
// Camera Class Implementation
Camera::Camera(float rad, float degTheta, float degPhi)
    : radius(rad), angleTheta(degTheta), anglePhi(degPhi), targetPosition(glm::vec3(0.0f)), upVector(glm::vec3(0.0f, 1.0f, 0.0f))
{
    updateCameraPosition();
}
void Camera::processMouseMovement(float deltaX, float deltaY)
{
    angleTheta += deltaX * 0.2f;
    anglePhi -= deltaY * 0.2f;
    anglePhi = glm::clamp(anglePhi, -89.0f, 89.0f);
    updateCameraPosition();
}
void Camera::processKeyboardInput(bool zoomIn)
{
    radius += zoomIn ? -0.2f : 0.2f;
    radius = std::max(radius, 1.0f);
    updateCameraPosition();
}
glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(camPosition, targetPosition, upVector);
}
glm::vec3 Camera::getPosition() const { return camPosition; }
void Camera::updateCameraPosition()
{
    float thetaRad = glm::radians(angleTheta);
    float phiRad = glm::radians(anglePhi);
    camPosition.x = radius * cos(phiRad) * cos(thetaRad);
    camPosition.y = radius * sin(phiRad);
    camPosition.z = radius * cos(phiRad) * sin(thetaRad);
}

// ----------------------------------------------------
// Callbacks Implementation
namespace Callbacks
{
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        static double lastX = -1, lastY = -1;
        if (lastX < 0 && lastY < 0)
        {
            lastX = xpos;
            lastY = ypos;
            return;
        }
        float deltaX = static_cast<float>(xpos - lastX);
        float deltaY = static_cast<float>(ypos - lastY);
        static bool leftMousePressed = false;
        if (leftMousePressed)
            camera->processMouseMovement(deltaX, -deltaY);
        lastX = xpos;
        lastY = ypos;
    }
    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        static bool leftMousePressed = false;
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            leftMousePressed = (action == GLFW_PRESS);
        }
    }
    void scrollCallback(GLFWwindow *window, double xoffset, double yoffset)
    {
        Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        camera->processKeyboardInput(yoffset > 0);
    }
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Camera *camera = static_cast<Camera *>(glfwGetWindowUserPointer(window));
        if (key == GLFW_KEY_UP && action != GLFW_RELEASE)
            camera->processKeyboardInput(true);
        else if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE)
            camera->processKeyboardInput(false);
        if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE)
            camera->processMouseMovement(5.0f, 0.0f);
        else if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE)
            camera->processMouseMovement(-5.0f, 0.0f);
    }
    void initializeCallbacks(GLFWwindow *window, Camera *camera)
    {
        glfwSetWindowUserPointer(window, camera);
        glfwSetCursorPosCallback(window, cursorPosCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetKeyCallback(window, keyCallback);
    }
}

// ----------------------------------------------------
// PlaneMesh Class Implementation
PlaneMesh::PlaneMesh(float lowerBound, float upperBound, float step)
    : lowerBound(lowerBound), upperBound(upperBound)
{
    generateMesh(lowerBound, upperBound, step);
    vertexCount = vertices.size() / 3;
    indexCount = indexData.size();
    setupBuffers();
}
void PlaneMesh::generateMesh(float lowerBound, float upperBound, float step)
{
    vertices.clear();
    normalVectors.clear();
    indexData.clear();
    float x = lowerBound, y = 0;
    // First column of vertices (x = lowerBound)
    for (float z = lowerBound; z <= upperBound; z += step)
    {
        vertices.insert(vertices.end(), {x, y, z});
        normalVectors.insert(normalVectors.end(), {0, 1, 0});
    }
    // Remaining columns
    for (x = lowerBound + step; x <= upperBound; x += step)
    {
        for (float z = lowerBound; z <= upperBound; z += step)
        {
            vertices.insert(vertices.end(), {x, y, z});
            normalVectors.insert(normalVectors.end(), {0, 1, 0});
        }
    }
    int nCols = static_cast<int>((upperBound - lowerBound) / step) + 1;
    for (int i = 0; i < nCols - 1; ++i)
    {
        for (int j = 0; j < nCols - 1; ++j)
        {
            indexData.insert(indexData.end(), {i * nCols + j,
                                               i * nCols + j + 1,
                                               (i + 1) * nCols + j + 1,
                                               (i + 1) * nCols + j});
        }
    }
}
void PlaneMesh::setupBuffers()
{
    glGenVertexArrays(1, &vertexArrayObj);
    glGenBuffers(1, &vertexBufferObj);
    glGenBuffers(1, &normalBufferObj);
    glGenBuffers(1, &elementBufferObj);
    glBindVertexArray(vertexArrayObj);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObj);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, normalBufferObj);
    glBufferData(GL_ARRAY_BUFFER, normalVectors.size() * sizeof(float), normalVectors.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObj);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexData.size() * sizeof(int), indexData.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}
void PlaneMesh::draw(glm::vec3 lightPos, glm::mat4 V, glm::mat4 P)
{
    glBindVertexArray(vertexArrayObj);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    glDrawElements(GL_PATCHES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// ----------------------------------------------------
// Renderer Class Implementation
Renderer::Renderer(float scrWidth, float scrHeight, float stepSize, float xMin, float xMax,
                   float cameraRadius, float cameraTheta, float cameraPhi,
                   float tessOuter, float tessInner,
                   float texScale,
                   const char *waterTex, const char *dispTex,
                   float heightMul)
    : screenWidth(scrWidth), screenHeight(scrHeight), stepSize(stepSize), xMin(xMin), xMax(xMax),
      cameraRadius(cameraRadius), cameraTheta(cameraTheta), cameraPhi(cameraPhi),
      tessellationOuter(tessOuter), tessellationInner(tessInner),
      texScale(texScale),
      waterTexturePath(waterTex), displacementTexturePath(dispTex),
      heightMultiplier(heightMul)
{
    init();
}
void Renderer::init()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        exit(EXIT_FAILURE);
    }
    mainWindow = glfwCreateWindow(screenWidth, screenHeight, "Assignment #6 - Water Surface Simulation", NULL, NULL);
    if (!mainWindow)
    {
        std::cerr << "Failed to open GLFW window\n";
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    // (Optional: center the window here)
    glfwMakeContextCurrent(mainWindow);
    glewExperimental = true;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW\n";
        exit(EXIT_FAILURE);
    }
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glClearColor(0.2f, 0.2f, 0.3f, 1.0f);
    glfwSetInputMode(mainWindow, GLFW_STICKY_KEYS, GL_TRUE);
}
void Renderer::run()
{
    mainLoop();
    cleanup();
}
void Renderer::mainLoop()
{
    Shader shader;
    // Adjust these shader paths as needed.
    bool success = shader.load("WaterShader.vertexshader",
                               "WaterShader.tcs",
                               "WaterShader.tes",
                               "WaterShader.geoshader",
                               "WaterShader.fragmentshader");
    if (!success)
    {
        std::cerr << "Shader program failed to load." << std::endl;
        return;
    }
    GLuint waterTex = Texture::loadImage(waterTexturePath);
    GLuint dispTex = Texture::loadImage(displacementTexturePath);
    Camera *camera = new Camera(cameraRadius, cameraTheta, cameraPhi);
    Callbacks::initializeCallbacks(mainWindow, camera);
    PlaneMesh plane(xMin, xMax, stepSize);
    glm::mat4 Projection = glm::perspective(glm::radians(45.0f), screenWidth / screenHeight, 0.1f, 100.0f);
    float timeValue = 0.0f;
    glm::vec3 lightPos(5.0f, 30.0f, 5.0f);
    glm::vec4 modelColor(1.0f, 1.0f, 1.0f, 1.0f);
    do
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::mat4 View = camera->getViewMatrix();
        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 MVP = Projection * View * Model;
        shader.use();
        shader.setMat4("MVP", MVP);
        shader.setMat4("Model", Model);
        shader.setVec3("LightPosition_worldspace", lightPos);
        shader.setVec3("EyePosition_worldspace", camera->getPosition());
        shader.setFloat("time", timeValue);
        shader.setVec4("modelcolor", modelColor);
        shader.setVec2("texOffset", glm::vec2(0.0f));
        shader.setFloat("texScale", texScale);
        shader.setFloat("outerTess", tessellationOuter);
        shader.setFloat("innerTess", tessellationInner);
        shader.setFloat("heightMult", heightMultiplier);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, waterTex);
        shader.setInt("waterTexture", 0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, dispTex);
        shader.setInt("displacementTexture", 1);
        plane.draw(lightPos, View, Projection);
        glfwSwapBuffers(mainWindow);
        glfwPollEvents();
        timeValue += 0.01f;
    } while (glfwGetKey(mainWindow, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
             !glfwWindowShouldClose(mainWindow));
    delete camera;
}
void Renderer::cleanup()
{
    glfwTerminate();
}

// ----------------------------------------------------
// Main function
int main()
{
    // Configurable parameters
    const float SCREEN_WIDTH = 1280.0f;
    const float SCREEN_HEIGHT = 720.0f;
    const float PLANE_STEPSIZE = 1.0f;
    const float PLANE_XMIN = -10.0f;
    const float PLANE_XMAX = 10.0f;
    const float CAMERA_RADIUS = 25.0f;
    const float CAMERA_THETA = 45.0f;
    const float CAMERA_PHI = 30.0f;
    const float OUTER_TESS_LEVEL = 64.0f;
    const float INNER_TESS_LEVEL = 64.0f;
    const float TEXTURE_SCALE = 10.0f;
    const char *DISP_TEXTURE_PATH = "Assets/displacement-map1.bmp";
    const char *WATER_TEXTURE_PATH = "Assets/water.bmp";
    const float HEIGHT_MULT = 0.15f;

    Renderer app(SCREEN_WIDTH, SCREEN_HEIGHT,
                 PLANE_STEPSIZE, PLANE_XMIN, PLANE_XMAX,
                 CAMERA_RADIUS, CAMERA_THETA, CAMERA_PHI,
                 OUTER_TESS_LEVEL, INNER_TESS_LEVEL,
                 TEXTURE_SCALE,
                 WATER_TEXTURE_PATH, DISP_TEXTURE_PATH,
                 HEIGHT_MULT);
    app.run();
    return 0;
}
