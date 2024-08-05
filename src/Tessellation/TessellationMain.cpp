#include "Tessellation.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Utility/GL/VertexArray/VertexArray.hpp"
#include "Utility/GL/Buffer/Buffer.hpp"
#include "Utility/GL/ShaderProgram/ShaderProgram.hpp"
#include "Utility/GL/Texture/Texture.hpp"

#include "Camera/CameraController/CameraController.hpp"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>

void Tessellation::init() {
    if (!glfwInit()) {
        std::cout << "Failed to init GLFW!\n";
        std::exit(1);
    }

    window = glfwCreateWindow(1024, 768, "Window", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to load OpenGL/GLAD!\n";
        std::exit(1);
    }

    glClearColor(.25f, .5f, .75f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    const char* vendor = (char*)glGetString(GL_VENDOR); // Returns the vendor
    const char* renderer = (char*)glGetString(GL_RENDERER); // Returns a hint to the model

    printf("Vendor: %s\nRenderer: %s\n", vendor, renderer);
}

std::unique_ptr<ShaderProgram> loadTessShader(std::string basePath) {
    Shader vertexShader(GL_VERTEX_SHADER);
    vertexShader.addSourceCodeFromPath(basePath + "vertex.glsl");
    vertexShader.compile();

    if (!vertexShader.isShaderCompiled()) {
        std::string vertexShaderLog = vertexShader.getInfoLog();
        std::cout << vertexShaderLog << '\n';
        std::exit(1);
    }

    Shader tcsShader(GL_TESS_CONTROL_SHADER);
    tcsShader.addSourceCodeFromPath(basePath + "tcs.glsl");
    tcsShader.compile();

    if (!tcsShader.isShaderCompiled()) {
        std::string tcsShaderLog = tcsShader.getInfoLog();
        std::cout << tcsShaderLog << '\n';
        std::exit(1);
    }

    Shader tesShader(GL_TESS_EVALUATION_SHADER);
    tesShader.addSourceCodeFromPath(basePath + "tes.glsl");
    tesShader.compile();

    if (!tesShader.isShaderCompiled()) {
        std::string tesShaderLog = tesShader.getInfoLog();
        std::cout << tesShaderLog << '\n';
        std::exit(1);
    }

    Shader fragmentShader(GL_FRAGMENT_SHADER);
    fragmentShader.addSourceCodeFromPath(basePath + "fragment.glsl");
    fragmentShader.compile();

    if (!fragmentShader.isShaderCompiled()) {
        std::string fragmentShaderLog = fragmentShader.getInfoLog();
        std::cout << fragmentShaderLog << '\n';
        std::exit(1);
    }

    // Assemble shaders together
    auto program = std::make_unique<ShaderProgram>();

    program->addShader(std::move(vertexShader));
    program->addShader(std::move(tcsShader));
    program->addShader(std::move(tesShader));
    program->addShader(std::move(fragmentShader));

    program->linkProgram();

    if (!program->isProgramLinked()) {
        std::string programLog = program->getInfoLog();
        std::cout << programLog << '\n';
        std::exit(1);
    }

    program->getShaderList().clear();

    return program;
}

glm::vec3 faceVertexData[] = {
    glm::vec3( 1.0f,-1.0f, -1.0f),
	glm::vec3(-1.0f,-1.0f, -1.0f),
	glm::vec3( 1.0f, 1.0f, -1.0f),
	glm::vec3(-1.0f, 1.0f, -1.0f),
    glm::vec3(-1.0f,-1.0f, 1.0f),
	glm::vec3( 1.0f,-1.0f, 1.0f),
	glm::vec3(-1.0f, 1.0f, 1.0f),
	glm::vec3( 1.0f, 1.0f, 1.0f),
    glm::vec3(-1.0f, -1.0f,-1.0f),
    glm::vec3(-1.0f, -1.0f, 1.0f),
    glm::vec3(-1.0f,  1.0f,-1.0f),
    glm::vec3(-1.0f,  1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f, 1.0f),
    glm::vec3(1.0f, -1.0f,-1.0f),
    glm::vec3(1.0f,  1.0f, 1.0f),
    glm::vec3(1.0f,  1.0f,-1.0f),
    glm::vec3( 1.0f, 1.f,-1.0f),
    glm::vec3(-1.0f, 1.f,-1.0f),
    glm::vec3( 1.0f, 1.f, 1.0f),
    glm::vec3(-1.0f, 1.f, 1.0f),
    glm::vec3(-1.0f,-1.f,-1.0f),
    glm::vec3( 1.0f,-1.f,-1.0f),
    glm::vec3(-1.0f,-1.f, 1.0f),
    glm::vec3( 1.0f,-1.f, 1.0f),
};

void Tessellation::loop() {
    VertexArray vao;
    vao.bindVertexArray();

    Buffer<glm::vec3> faceVertexBuffer;
    faceVertexBuffer.bufferData(faceVertexData);

    Buffer<glm::vec3> spherePositionBuffer;
    Buffer<float>     sphereScaleBuffer;

    std::vector<glm::vec3> spherePositionData;
    std::vector<float>     sphereScaleData;

    spherePositionData.push_back(glm::vec3(10, 0, 10));
    sphereScaleData.push_back(1);

    spherePositionData.push_back(glm::vec3(0, 0, 10));
    sphereScaleData.push_back(1);

    spherePositionData.push_back(glm::vec3(10, 0, 20));
    sphereScaleData.push_back(1);

    spherePositionBuffer.bufferData(spherePositionData);
    sphereScaleBuffer.bufferData(sphereScaleData);

    auto myTessShader = loadTessShader("./shader/sphere/");
    myTessShader->use();
    GLuint MVPuniform = myTessShader->getUniformLocation("MVP");
    GLuint originUniform = myTessShader->getUniformLocation("origin");
    GLuint scaleUniform = myTessShader->getUniformLocation("scale");
    CameraController cameraController(glm::vec3(-5, 0, -5));

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, faceVertexBuffer.getBufferId());
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, spherePositionBuffer.getBufferId());
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, sphereScaleBuffer.getBufferId());
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, nullptr);

    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

    glPatchParameteri(GL_PATCH_VERTICES, 4);

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    double lastFrameStartTime = glfwGetTime();

    float aspectRatio;
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) !=  GLFW_PRESS) {
        double frameStartTime = glfwGetTime();

        // DeltaT calc
        double deltaTime = frameStartTime - lastFrameStartTime;

        glfwPollEvents();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        int windowWidth, windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);

        if (windowWidth != 0 && windowHeight != 0) {
            aspectRatio = (float)windowWidth / (float)windowHeight;
        }

        cameraController.step(window, deltaTime);

        glm::mat4 viewMatrix = cameraController.getViewMatrix();
        glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), aspectRatio, 0.1f, 1000.f);
        glm::mat4 viewProjection = projectionMatrix * viewMatrix;

        glm::vec3 origin(5, 0, 5);
        float scale = 15;

        glUniform3fv(originUniform, 1, &origin[0]);
        glUniform1fv(scaleUniform, 1, &scale);
        glUniformMatrix4fv(MVPuniform, 1, GL_FALSE, &viewProjection[0][0]);

        glDrawArraysInstanced(GL_PATCHES, 0, 24, spherePositionData.size());

        // Frame limiter
        const double frameEndTime = glfwGetTime();
        const double frameTimeMS = (frameEndTime - frameStartTime) * 1e6;
        const double requiredFrameTimeMS = 8333.33;
        const double sleepTime = requiredFrameTimeMS - frameTimeMS;

        if (sleepTime > 0) std::this_thread::sleep_for(std::chrono::microseconds((std::int64_t)sleepTime));

        // Update dt calculation
        lastFrameStartTime = frameStartTime;

        glfwSwapBuffers(window);
        
        // Make viewport always the same size as window
        glViewport(0, 0, windowWidth, windowHeight);
    }
}

void Tessellation::cleanup() {
    glfwDestroyWindow(window);

    glfwTerminate();
}