#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <texture.h>
#include <iostream>


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

float mixValue = 0.2f;

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
    GLFWwindow* window = glfwCreateWindow(800, 600, "Jefe", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "ERROR::GLFW::WINDOW::CREATION_FAILED" << std::endl;
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

    int nrAttributes;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
    std::cout << "INFO::GL_MAX_VERTEX_ATTRIBS::" << nrAttributes << std::endl;

    // Backface culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    // Set up shaders
    Shader colorShader("shaders/color.vert", "shaders/color.frag");
    Shader uniformShader("shaders/color.vert", "shaders/uniform.frag");
    Shader textureShader("shaders/color.vert", "shaders/texture.frag");

    Texture containerTex("resources/container.jpg", false, false);
    Texture guyTex("resources/awesomeface.png", true, true);

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
    float secondRectangleVertices[] = {
         0.5f,  0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
         0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, // top left 
    };

    unsigned int secondRectangleIndices[] = {
        0, 1, 3,   // first triangle
        1, 2, 3    // second triangle
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
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
    glBufferData(GL_ARRAY_BUFFER, sizeof(secondRectangleVertices), secondRectangleVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 3. copy our index array in a element buffer for OpenGL to use
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(secondRectangleIndices), secondRectangleIndices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    textureShader.use();
    textureShader.setInt("tex1", 0);
    textureShader.setInt("tex2", 1);

    float timeValue, greenValue;
    int vertexColorLocation;
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Orange rect
        //uniformShader.use();
        //glBindVertexArray(VAOs[0]);
        //
        //timeValue = glfwGetTime();
        //greenValue = sin(timeValue) / 2.0f + 0.5f;
        //
        //uniformShader.setVec4f("color", 0.0f, greenValue, 0.0f, 1.0f);

        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[0]);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 transform(1.0f);
        transform = glm::translate(transform, glm::vec3(0.5f, -0.5f, 0.0f));
        transform = glm::rotate(transform, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
        textureShader.setMat4fv("transform", &transform);

        // Yellow rect
        textureShader.setFloat("mixValue", mixValue);
        containerTex.activate(0);
        guyTex.activate(1);
        glBindVertexArray(VAOs[1]);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBOs[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glm::mat4 trans2(1.0f);
        trans2 = glm::translate(trans2, glm::vec3(-0.5f, 0.5f, 0.0f));
        float scale = 0.5f * (float)glm::sin(4 * glfwGetTime()) + 1.0f;
        trans2 = glm::scale(trans2, glm::vec3(scale, scale, 1.0));
        textureShader.setMat4fv("transform", &trans2);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(2, VAOs);
    glDeleteBuffers(2, VBOs);
    glDeleteBuffers(2, EBOs);

    uniformShader.del();
    colorShader.del();

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        if (mixValue < 1.0f)
            mixValue += 0.0001f;

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        if (mixValue > 0.0f)
            mixValue -= 0.0001f;
        
}