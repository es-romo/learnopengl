#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>
#include <camera.h>
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

struct {
    float delta = 0.0f;
    float last = 0.0f;
    float current = 0.0f;
} gameTime;

struct {
    int height = 600;
    int width = 800;
} windowDimensions;

struct {
    float lastx = (float) windowDimensions.width / 2;
    float lasty = (float)windowDimensions.height / 2;
    float x = (float) windowDimensions.width / 2;
    float y = (float) windowDimensions.height / 2;
    float sensitivity = 0.1f;
} mouse;


bool firstMouse = true;
Camera camera(glm::vec3(-3.0f, 0.0f, 0.0f));

int main()
{
    if (!glfwInit()) {
        std::cout << "ERROR::GLFW::INIT_FAILED\n" << stderr << std::endl;
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create glfw window
    GLFWwindow* window = glfwCreateWindow(windowDimensions.width, windowDimensions.height, "Jefe", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR::GLFW::WINDOW::CREA TION_FAILED" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // Load OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "ERROR::GLAD::INITALIZATION_FAILED" << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "INFO::GL_MAX_VERTEX_ATTRIBS::" << nrAttributes << std::endl;

    // Backface culling
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    //glFrontFace(GL_CW);
    glEnable(GL_DEPTH_TEST);

    // Set up shaders
    //Shader colorShader("shaders/color.vert", "shaders/color.frag");
    //Shader uniformShader("shaders/color.vert", "shaders/uniform.frag");
    //Shader textureShader("shaders/texture.vert", "shaders/texture.frag");
    Shader lightSourceShader("shaders/light_source.vert", "shaders/light_source.frag");
    Shader lightTargetShader("shaders/light_target.vert", "shaders/light_target.frag");
    //Texture containerTex("resources/container.jpg", false, false);
    //Texture guyTex("resources/awesomeface.png", true, true);
    Texture diffMap("resources/container2.png", false);
    Texture specMap("resources/container2_specular.png", false);
    Texture emissionMap("resources/emission2.png", false);

    // First rectangle vertex data
    float firstRectangleVertices[] = {
         0.5f,  0.5f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
    };

    unsigned int firstRectangleIndices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
    };

    // Second rectangle vertex data
    //float secondRectangleVertices[] = {
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    //     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    //    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    //    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    //};

    float cubeVertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
    };

    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    // Vertex attribute object
    // Vertex bufffer object
    // Element buffer object
    unsigned int VAOs[2], VBOs[2], EBOs[2];
    glGenVertexArrays(2, VAOs);
    glGenBuffers(2, VBOs);
    glGenBuffers(2, EBOs);


    // First rectangle
    // 1. bind Vertex Array Object
    glBindVertexArray(VAOs[0]);

    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(firstRectangleVertices), firstRectangleVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(firstRectangleIndices), firstRectangleIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Second rectangle
    // 1. bind Vertex Array Object
    glBindVertexArray(VAOs[1]);

    // 2. copy our vertices array in a vertex buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 3. copy our index array in a element buffer for OpenGL to use
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(secondRectangleIndices), secondRectangleIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //textureShader.use();
    //textureShader.setInt("tex1", 0);
    //textureShader.setInt("tex2", 1);
    //containerTex.activate(0);
    //guyTex.activate(1);

    diffMap.activate(0);
    specMap.activate(1);
    emissionMap.activate(2);

    glfwSetTime(0);

    while (!glfwWindowShouldClose(window))
    {
        gameTime.current = glfwGetTime();
        gameTime.delta = gameTime.current - gameTime.last;

        processInput(window);

        // Background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAOs[1]);

        glm::mat4 view = camera.getViewMatrix();

        glm::mat4 projection = glm::perspective(glm::radians(camera.fov), (float)windowDimensions.width / (float)windowDimensions.height, 0.1f, 100.0f);

        //glm::mat4 projection = glm::ortho(0.0f, 3.0f, 2.0f, 0.0f, .01f, 100.0f);

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::rotate(model2, glm::radians(90 * gameTime.current),glm::vec3(0.0f, 1.0f, 0.0));
        model2 = glm::translate(model2, glm::vec3(1.5f,0.0f,0.0f));
        model2 = glm::rotate(model2, glm::radians(60 * gameTime.current), glm::vec3(0.0f, 1.0, 0.0));
        model2 = glm::scale(model2, glm::vec3(.2f, .2f, .2f));

        glm::vec4 lightPos = model2 * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        lightSourceShader.use();

        glm::mat4 modelx = glm::mat4(1.0f);
        modelx = glm::rotate(modelx, glm::radians(90 * gameTime.current), glm::vec3(0.0f, 1.0f, 0.0));
        modelx = glm::translate(modelx, glm::vec3(5.0f, 1.5f, -1.0f));
        modelx = glm::scale(modelx, glm::vec3(.2f, .2f, .2f));

        glm::vec4 lightVector = modelx * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

        lightSourceShader.setMat4fv("model", modelx);

        //lightSourceShader.setMat4fv("model", model2);
        lightSourceShader.setMat4fv("view", view);
        lightSourceShader.setMat4fv("projection", projection);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightTargetShader.use();
        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-15 * gameTime.current), glm::vec3(0.0f, 1.0f, 0.0));

        //lightTargetShader.setVec4f("light.vector", lightPos);
        //lightTargetShader.setVec4f("light.vector", glm::vec4(-camera.position, 0.0f));
        lightTargetShader.setVec4f("light.vector", lightVector);
        lightTargetShader.setVec3f("light.ambient", 0.2f, 0.2f, 0.2f);
        lightTargetShader.setVec3f("light.diffuse", 0.5f, 0.5f, 0.5f); // darken diffuse light a bit
        lightTargetShader.setVec3f("light.specular", 1.0f, 1.0f, 1.0f);
        lightTargetShader.setVec3f("viewPos", camera.position);
        
        lightTargetShader.setFloat("light.constant", 1.0f);
        lightTargetShader.setFloat("light.linear", 0.027f);
        lightTargetShader.setFloat("light.quadratic", 0.0028f);
        
        lightTargetShader.setInt("material.diffuse", 0);
        lightTargetShader.setInt("material.specular", 1);
        lightTargetShader.setInt("material.emission", 2);
        lightTargetShader.setFloat("material.shininess", 32.0f);

        lightTargetShader.setMat4fv("view", view);
        lightTargetShader.setMat4fv("projection", projection);

        for (int i = 0; i < 10; i++)
        {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);  
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            lightTargetShader.setMat4fv("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
        gameTime.last = gameTime.current;
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);

    //uniformShader.del();
    //colorShader.del();
    //textureShader.del();
    lightSourceShader.del();
    lightTargetShader.del();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowDimensions.width = width;
    windowDimensions.height = height;
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse)
    {
        mouse.lastx = xpos;
        mouse.lasty = ypos;
        firstMouse = false;
    }

    mouse.x = xpos;
    mouse.y = ypos;

    float yaw = (mouse.x - mouse.lastx) * mouse.sensitivity;
    float pitch = (mouse.lasty - mouse.y) * mouse.sensitivity;

    mouse.lastx = mouse.x;
    mouse.lasty = mouse.y;

    camera.processLook(yaw, pitch);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processZoom(-(float)yoffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processMovement(Camera::Action::FORWARD, gameTime.delta);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processMovement(Camera::Action::LEFT, gameTime.delta);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processMovement(Camera::Action::BACKWARD, gameTime.delta);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processMovement(Camera::Action::RIGHT, gameTime.delta);

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processMovement(Camera::Action::WORLD_UP, gameTime.delta);

    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.processMovement(Camera::Action::WORLD_DOWN, gameTime.delta);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_Q && action == GLFW_PRESS)
    {
        if (camera.mode == CAMERA_MODE_FPS) camera.mode = CAMERA_MODE_DRONE;
        else if (camera.mode == CAMERA_MODE_DRONE) camera.mode = CAMERA_MODE_FPS;
    }
    if (key == GLFW_KEY_LEFT_SHIFT) {
        if (action == GLFW_PRESS) {
            camera.speed = CAMERA_BOOST_SPEED;
        }
        if (action == GLFW_RELEASE) {
            camera.speed = CAMERA_SPEED;
        }
    }
}