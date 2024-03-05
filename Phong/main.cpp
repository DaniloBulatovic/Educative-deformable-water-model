#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <thread>
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp"
#include "texture.hpp"
#include <random>
#include <list>

float Clamp(float x, float min, float max) {
    return x < min ? min : x > max ? max : x;
}

int WindowWidth = 800;
int WindowHeight = 800;
const float TargetFPS = 60.0f;
const std::string WindowTitle = "Simulacija talasa";

const float defaultAmplitude = 0.5f;
const float defaultFrequency = 5.0f;
const float defaultSpeed = 5.0f;
const float defaultSteepness = 0.4f;

const float smallAmplitude = defaultAmplitude / 5.0f;
const float bigAmplitude = defaultAmplitude * 2.0f;

struct Input {
    bool MoveLeft;
    bool MoveRight;
    bool MoveForward;
    bool MoveBackward;
    bool MoveUp;
    bool MoveDown;
    bool LookLeft;
    bool LookRight;
    bool LookUp;
    bool LookDown;
    bool RotateWaveLeft;
    bool RotateWaveRight;
};

struct EngineState {
    Input* mInput;
    Camera* mCamera;
    bool mPolygonLines = true;
    bool mWaves = true;
    bool mRain;
    bool mColors;
    float mAmplitude = defaultAmplitude;
    float mFrequency = defaultFrequency;
    float mSpeed = defaultSpeed;
    float mSteepness = defaultSteepness;
    float xMousePos;
    float yMousePos;
    float mDT;
    glm::vec2 waveDirection;
    int lastKey = 0;
    int lastNumKey = 0;
};

static void ErrorCallback(int error, const char* description) {
    std::cerr << "GLFW Error: " << description << std::endl;
}

static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    Input* UserInput = State->mInput;
    bool IsDown = action == GLFW_PRESS || action == GLFW_REPEAT;
    switch (key) {
        case GLFW_KEY_A: UserInput->MoveLeft = IsDown; break;
        case GLFW_KEY_D: UserInput->MoveRight = IsDown; break;
        case GLFW_KEY_W: UserInput->MoveForward = IsDown; break;
        case GLFW_KEY_S: UserInput->MoveBackward = IsDown; break;

        case GLFW_KEY_SPACE: UserInput->MoveUp = IsDown; break;
        case GLFW_KEY_LEFT_SHIFT: UserInput->MoveDown = IsDown; break;

        case GLFW_KEY_RIGHT: UserInput->LookLeft = IsDown; break;
        case GLFW_KEY_LEFT: UserInput->LookRight = IsDown; break;
        case GLFW_KEY_UP: UserInput->LookUp = IsDown; break;
        case GLFW_KEY_DOWN: UserInput->LookDown = IsDown; break;

        case GLFW_KEY_X: UserInput->RotateWaveLeft = IsDown; break;
        case GLFW_KEY_C: UserInput->RotateWaveRight = IsDown; break;

        case GLFW_KEY_KP_ADD:
            if (State->lastKey == GLFW_KEY_A) {
                State->mAmplitude += 0.01f;
            }
            else if (State->lastKey == GLFW_KEY_F) {
                State->mFrequency += 0.1f;
            }
            else if (State->lastKey == GLFW_KEY_S) {
                State->mSpeed += 0.1f;
            }
            else if (State->lastKey == GLFW_KEY_P) {
                State->mSteepness += 0.01f;
            }
            break;
        case GLFW_KEY_KP_SUBTRACT:
            if (State->lastKey == GLFW_KEY_A) {
                State->mAmplitude -= 0.01f;
            }
            else if (State->lastKey == GLFW_KEY_F) {
                State->mFrequency -= 0.1f;
            }
            else if (State->lastKey == GLFW_KEY_S) {
                State->mSpeed -= 0.1f;
            }
            else if (State->lastKey == GLFW_KEY_P) {
                State->mSteepness -= 0.01f;
            }
            break;
        case GLFW_KEY_1:
            State->mAmplitude = smallAmplitude;
            State->mFrequency = defaultFrequency;
            State->mSpeed = defaultSpeed;
            break;
        case GLFW_KEY_2:
            State->mAmplitude = defaultAmplitude;
            State->mFrequency = defaultFrequency;
            State->mSpeed = defaultSpeed;
            break;
        case GLFW_KEY_3:
            State->mAmplitude = bigAmplitude;
            State->mFrequency = defaultFrequency;
            State->mSpeed = defaultSpeed;
            break;
        case GLFW_KEY_ESCAPE: glfwSetWindowShouldClose(window, GLFW_TRUE); break;
    }
    if (key != GLFW_KEY_KP_ADD && key != GLFW_KEY_KP_SUBTRACT) {
        State->lastKey = key;
    }
    if (key == GLFW_KEY_1 || key == GLFW_KEY_2 || key == GLFW_KEY_3) {
        State->lastNumKey = key;
    }
}

static void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowWidth = width;
    WindowHeight = height;
    glViewport(0, 0, width, height);
}

static void RotateLeft(EngineState* state)
{
    //speed 0.01f
    float rotationSpeed = 1.0f;
    float rotationAmount = rotationSpeed * state->mDT;

    glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), -rotationAmount, glm::vec3(0.0f, 0.0f, 1.0f));

    state->waveDirection = glm::clamp(glm::vec2(rotationMatrix * glm::vec3(state->waveDirection, 0.0f)), glm::vec2(-1.0f), glm::vec2(1.0f));
    
    /*if (state->waveDirection.x < 0.99f && state->waveDirection.x > 0) {
        state->waveDirection.x = -0.99f;
    } else if (state->waveDirection.x > -0.99f && state->waveDirection.x < 0) {
        state->waveDirection.x = 0.99f;
    }

    if (state->waveDirection.y < 0.99f && state->waveDirection.y > 0) {
        state->waveDirection.y = -0.99f;
    } else if (state->waveDirection.y > -0.99f && state->waveDirection.y < 0) {
        state->waveDirection.y = 0.99f;
    }*/
}

static void RotateRight(EngineState* state)
{
    float rotationSpeed = 1.0f;
    float rotationAmount = rotationSpeed * state->mDT;

    glm::mat3 rotationMatrix = glm::rotate(glm::mat4(1.0f), rotationAmount, glm::vec3(0.0f, 0.0f, 1.0f));

    state->waveDirection = glm::clamp(glm::vec2(rotationMatrix * glm::vec3(state->waveDirection, 0.0f)), glm::vec2(-1.0f), glm::vec2(1.0f));

    /*if (state->waveDirection.x < 0.99f && state->waveDirection.x > 0) {
        state->waveDirection.x = -0.99f;
    } else if (state->waveDirection.x > -0.99f && state->waveDirection.x < 0) {
        state->waveDirection.x = 0.99f;
    }

    if (state->waveDirection.y < 0.99f && state->waveDirection.y > 0) {
        state->waveDirection.y = -0.99f;
    } else if (state->waveDirection.y > -0.99f && state->waveDirection.y < 0) {
        state->waveDirection.y = 0.99f;
    }*/
}

static void HandleInput(EngineState* state) {
    Input* UserInput = state->mInput;
    Camera* FPSCamera = state->mCamera;

    if (UserInput->MoveLeft) FPSCamera->Move(-1.0f, 0.0f, state->mDT);
    if (UserInput->MoveRight) FPSCamera->Move(1.0f, 0.0f, state->mDT);
    if (UserInput->MoveBackward) FPSCamera->Move(0.0f, -1.0f, state->mDT);
    if (UserInput->MoveForward) FPSCamera->Move(0.0f, 1.0f, state->mDT);

    if (UserInput->MoveUp) FPSCamera->Move(1.0f, state->mDT);
    if (UserInput->MoveDown) FPSCamera->Move(-1.0f, state->mDT);

    if (UserInput->LookLeft) FPSCamera->Rotate(1.0f, 0.0f, state->mDT);
    if (UserInput->LookRight) FPSCamera->Rotate(-1.0f, 0.0f, state->mDT);
    if (UserInput->LookDown) FPSCamera->Rotate(0.0f, -1.0f, state->mDT);
    if (UserInput->LookUp) FPSCamera->Rotate(0.0f, 1.0f, state->mDT);

    if (UserInput->RotateWaveLeft) RotateLeft(state);
    if (UserInput->RotateWaveRight) RotateRight(state);
}

void HandleKeyToggle(GLFWwindow* window, bool* result, bool* keyPressed, int key) {
    bool keyCurrentlyPressed = glfwGetKey(window, key) == GLFW_PRESS;
    if (!*keyPressed && keyCurrentlyPressed) {
        *result = !*result;
    }
    *keyPressed = keyCurrentlyPressed;
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    EngineState* State = (EngineState*)glfwGetWindowUserPointer(window);
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
    {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        int screenWidth, screenHeight;
        glfwGetWindowSize(window, &screenWidth, &screenHeight);

        State->xMousePos = xpos;
        State->yMousePos = ypos;
    }
    else {
        State->xMousePos = 0;
        State->yMousePos = 0;
    }
}

glm::vec3* getNearestVertex(glm::vec2 mouseClick, int screenWidth, int screenHeight, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, std::vector<float> vertices)
{
    float ndcX = (2.0 * mouseClick.x) / screenWidth - 1.0;
    float ndcY = 1.0 - (2.0 * mouseClick.y) / screenHeight;

    // Calculate inverse of combined projection and view matrices
    glm::mat4 inversePVMatrix = glm::inverse(projectionMatrix * viewMatrix);

    // Calculate ray in clip space
    glm::vec4 rayClip = glm::vec4(ndcX, ndcY, 1.0f, 1.0f);

    // Transform ray to world space
    glm::vec4 rayWorld = inversePVMatrix * rayClip;
    rayWorld /= rayWorld.w;

    // Origin of the ray is the camera position
    glm::vec3 rayOrigin = glm::inverse(viewMatrix)[3];

    // Normalize ray direction
    glm::vec3 rayDirection = glm::normalize(glm::vec3(rayWorld) - rayOrigin);

    float minDistance = std::numeric_limits<float>::max();
    glm::vec3 nearestVertex{};

    // Iterate through all vertices and find the nearest one
    for (int i = 0; i < vertices.size(); i += 5)
    {
        // Get vertex position
        glm::vec3 vertexPos(vertices[i], vertices[i + 1], vertices[i + 2]);

        // Calculate distance from the ray to the vertex
        glm::vec3 rayToVertex = vertexPos - rayOrigin;
        float distance = glm::length(glm::cross(rayToVertex, rayDirection));

        // Check if the distance is smaller than the current nearest distance
        if (distance < minDistance)
        {
            minDistance = distance;
            nearestVertex = glm::vec3(vertexPos.x, vertexPos.y, vertexPos.z);
        }
    }
    return &nearestVertex;
}

int main() {
    GLFWwindow* Window = 0;
    if (!glfwInit()) {
        std::cerr << "Failed to init glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    Window = glfwCreateWindow(WindowWidth, WindowHeight, WindowTitle.c_str(), 0, 0);
    if (!Window) {
        std::cerr << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);

    GLenum GlewError = glewInit();
    if (GlewError != GLEW_OK) {
        std::cerr << "Failed to init glew: " << glewGetErrorString(GlewError) << std::endl;
        glfwTerminate();
        return -1;
    }

    EngineState State = { 0 };
    Camera FPSCamera;
    Input UserInput = { 0 };
    State.mCamera = &FPSCamera;
    State.mInput = &UserInput;
    State.waveDirection = glm::vec2(1.0f, -0.2f);
    glfwSetWindowUserPointer(Window, &State);

    glfwSetErrorCallback(ErrorCallback);
    glfwSetFramebufferSizeCallback(Window, FramebufferSizeCallback);
    glfwSetKeyCallback(Window, KeyCallback);
    glfwSetMouseButtonCallback(Window, MouseButtonCallback);

    glViewport(0.0f, 0.0f, WindowWidth, WindowHeight);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //glClearColor(0.1, 0.1, 0.3, 1.0);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    unsigned WaterDiffuseTexture = Texture::LoadImageToTexture("res/water_diffuse.jpg");
    unsigned WaterSpecularTexture = Texture::LoadImageToTexture("res/water_specular.jpg");
    unsigned WaterDUDVTexture = Texture::LoadImageToTexture("res/waterDUDV.png");

    int resolution = 100;
    std::vector<float> vertices;

    for (int z = 0; z < resolution; z++) {
        for (int x = 0; x < resolution; x++) {
            vertices.push_back(((float(x) / (resolution - 1)) * 2 - 1) * (resolution / 2.0f));
            vertices.push_back(0.0f);
            vertices.push_back(((float(z) / (resolution - 1)) * 2 - 1) * (resolution / 2.0f));
            vertices.push_back((float(x) / (resolution - 1)));
            vertices.push_back((float(z) / (resolution - 1)));
        }
    }

    std::vector<unsigned int> indices;

    for (int x = 0; x < resolution - 1; x++) {
        for (int z = 0; z < resolution - 1; z++) {
            int i0 = x * resolution + z;
            int i1 = i0 + 1;
            int i2 = i0 + resolution;
            int i3 = i2 + 1;

            if ((z + x) % 2) {
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i1);
                indices.push_back(i1); indices.push_back(i2); indices.push_back(i3);
            }
            else {
                indices.push_back(i0); indices.push_back(i2); indices.push_back(i3);
                indices.push_back(i0); indices.push_back(i3); indices.push_back(i1);
            }
        }
    }

    unsigned PlaneVAO;
    glGenVertexArrays(1, &PlaneVAO);
    glBindVertexArray(PlaneVAO);

    unsigned PlaneVBO;
    glGenBuffers(1, &PlaneVBO);
    glBindBuffer(GL_ARRAY_BUFFER, PlaneVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    unsigned PlaneEBO;
    glGenBuffers(1, &PlaneEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PlaneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    Shader RippleShader("shaders/ripple.vert", "shaders/ripple.frag");
    glUseProgram(RippleShader.GetId());
    RippleShader.SetUniform3f("uDirLight.Direction", glm::vec3(0.0f, -1.0f, 0.0f));
    RippleShader.SetUniform3f("uDirLight.Ka", glm::vec3(0.1f));
    RippleShader.SetUniform3f("uDirLight.Kd", glm::vec3(0.4f));
    RippleShader.SetUniform3f("uDirLight.Ks", glm::vec3(1.0f));
    RippleShader.SetUniform1i("uMaterial.Kd", 0);
    RippleShader.SetUniform1i("uMaterial.Ks", 1);
    RippleShader.SetUniform1f("uMaterial.Shininess", 1024.0f);
    glUseProgram(0);

    glm::mat4 ModelMatrix(1.0f);
    glm::mat4 Projection = glm::perspective(glm::radians(90.0f), (float)WindowWidth / WindowHeight, 0.1f, 500.0f);
    glm::mat4 View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
    
    bool keyMPressed = false;
    bool keyRPressed = false;
    bool keyTPressed = false;
    bool keyBPressed = false;
    int screenWidth, screenHeight;
    int MAX_CENTERS = 100;
    double noRainProbability = 0.7;
    float moveFactor = 0;
    std::list<glm::vec3> centerPositions{};
    std::list<float> timersList{};
    std::list<float> amplitudesList{};
    float initTimer = 0.1f;
    centerPositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    timersList.push_back(initTimer);
    amplitudesList.push_back(defaultAmplitude);
    glm::vec3 nearestVertex = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 nearestCenter = glm::vec3(0.0f, 0.0f, 0.0f);
    float Angle = 0.0f;
    float Distance = 5.0f;
    float TargetFrameTime = 1.0f / TargetFPS;
    float StartTime = glfwGetTime();
    float EndTime = glfwGetTime();

    Shader* CurrentShader = &RippleShader;
    while (!glfwWindowShouldClose(Window)) {
        glfwPollEvents();
        HandleInput(&State);

        HandleKeyToggle(Window, &State.mPolygonLines, &keyMPressed, GLFW_KEY_M);
        HandleKeyToggle(Window, &State.mWaves, &keyTPressed, GLFW_KEY_T);
        HandleKeyToggle(Window, &State.mColors, &keyBPressed, GLFW_KEY_B);

        bool keyRCurrentlyPressed = glfwGetKey(Window, GLFW_KEY_R) == GLFW_PRESS;
        if (!keyRPressed && keyRCurrentlyPressed) {
            State.mRain = !State.mRain;
            if (!State.mRain) {
                centerPositions.clear();
                timersList.clear();
                amplitudesList.clear();
            }
        }
        keyRPressed = keyRCurrentlyPressed;

        if (State.mPolygonLines)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Projection = glm::perspective(45.0f, WindowWidth / (float)WindowHeight, 0.1f, 500.0f);
        View = glm::lookAt(FPSCamera.GetPosition(), FPSCamera.GetTarget(), FPSCamera.GetUp());
        StartTime = glfwGetTime();
        glUseProgram(CurrentShader->GetId());
        CurrentShader->SetProjection(Projection);
        CurrentShader->SetView(View);
        CurrentShader->SetUniform3f("uViewPos", FPSCamera.GetPosition());
        glfwGetWindowSize(Window, &screenWidth, &screenHeight);
        
        if (State.xMousePos != 0 && State.yMousePos != 0) {
            nearestVertex = *getNearestVertex(glm::vec2(State.xMousePos, State.yMousePos), screenWidth, screenHeight, Projection, View, vertices);
            if (centerPositions.size() > MAX_CENTERS){
                centerPositions.pop_front();
                timersList.pop_front();
                amplitudesList.pop_front();
            }
            if (std::find(centerPositions.begin(), centerPositions.end(), nearestVertex) == centerPositions.end()) {
                centerPositions.push_back(nearestVertex);
                timersList.push_back(initTimer);
                amplitudesList.push_back(State.mAmplitude);
            }
        }

        if (State.mWaves) {
            CurrentShader->SetUniform1i("waves", 1);
        }
        else {
            CurrentShader->SetUniform1i("waves", 0);
        }

        if (State.mColors) {
            CurrentShader->SetUniform1i("colors", 1);
        }
        else {
            CurrentShader->SetUniform1i("colors", 0);
        }

        if (State.mRain) {
            std::vector<glm::vec3> tempVertices;
            for (int i = 0; i < vertices.size(); i += 5)
            {
                glm::vec3 vertexPosition(vertices[i], vertices[i + 1], vertices[i + 2]);
                tempVertices.push_back(vertexPosition);
            }
            std::random_device random_device;
            std::mt19937 engine{random_device()};
            std::uniform_int_distribution<int> dist(0, tempVertices.size() - 1);

            std::uniform_real_distribution<> probDist(0.0, 1.0);
            double randomValue = probDist(engine);

            if (randomValue > noRainProbability){
                glm::vec3 random_element = tempVertices[dist(engine)];
                if (centerPositions.size() >= MAX_CENTERS){
                    centerPositions.pop_front();
                    timersList.pop_front();
                    amplitudesList.pop_front();
                }
                centerPositions.push_back(random_element);
                timersList.push_back(initTimer);
                amplitudesList.push_back(State.mAmplitude);
            }
        }

        std::vector<glm::vec3> centers{};
        std::vector<float> timers{};
        std::vector<float> amplitudes{};
        if (!centerPositions.empty()){
            centers = std::vector<glm::vec3> (centerPositions.begin(), centerPositions.end());
            timers = std::vector<float>(timersList.begin(), timersList.end());
            amplitudes = std::vector<float>(amplitudesList.begin(), amplitudesList.end());
        }

        // Mesh plane
        glUseProgram(CurrentShader->GetId());
        CurrentShader->SetView(View);
        ModelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
        CurrentShader->SetModel(ModelMatrix);
        CurrentShader->SetUniform1f("time", glfwGetTime());
        CurrentShader->SetUniform1i("maxCenters", centers.size());
        if (centers.size())
            CurrentShader->SetUniform3fv("centers", centers);
        if (timers.size())
            CurrentShader->SetUniform1fv("timers", timers);
        if (amplitudes.size())
            CurrentShader->SetUniform1fv("amplitudes", amplitudes);
        CurrentShader->SetUniform2f("direction", State.waveDirection);
        CurrentShader->SetUniform1f("amplitude", State.mAmplitude);
        CurrentShader->SetUniform1f("frequency", State.mFrequency);
        CurrentShader->SetUniform1f("steepness", State.mSteepness);
        CurrentShader->SetUniform1f("speed", State.mSpeed);

        //moveFactor += 0.00001f * StartTime;
        if (moveFactor > 1.0f) {
            moveFactor = 0.0f;
        }
        CurrentShader->SetUniform1f("moveFactor", moveFactor);
        glBindVertexArray(PlaneVAO);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, WaterDiffuseTexture);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, WaterSpecularTexture);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, WaterDUDVTexture);
        glUniform1i(glGetUniformLocation(CurrentShader->GetId(), "dudvMap"), 2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, PlaneEBO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, (void*)0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);

        glGetError();
        glfwSwapBuffers(Window);

        if (timersList.size()){
            int i;
            for (i = 0; i < timersList.size(); i++) {
                if (timers[i] + State.mDT <= 30){
                    timers[i] += State.mDT;
                }
            }
            timersList = std::list<float>(timers.begin(), timers.end());
        }

        EndTime = glfwGetTime();
        float WorkTime = EndTime - StartTime;
        if (WorkTime < TargetFrameTime) {
            int DeltaMS = (int)((TargetFrameTime - WorkTime) * 1000.0f);
            std::this_thread::sleep_for(std::chrono::milliseconds(DeltaMS));
            EndTime = glfwGetTime();
        }
        State.mDT = EndTime - StartTime;
    }

    glfwTerminate();
    return 0;
}

