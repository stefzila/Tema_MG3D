#pragma once
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#define STB_IMAGE_IMPLEMENTATION

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <filesystem>
#include "main.h"
#include "Shader.h"
#include "camera.h"
#include <stb_image/stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
    cubeRotations.push_back(0.0f);
    cubeRotations.push_back(0.0f);
    cubeRotationDirection.push_back(glm::vec3(0, 1, 0));
    cubeRotationDirection.push_back(glm::vec3(0, 1, 0));
    cubeScale.push_back(glm::vec3(1, 1, 1));
    cubeScale.push_back(glm::vec3(1, 1, 1));


    //Initialize and open the main window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Tema_MG3D", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    //Initialized GLAD adn load all openGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    
    glEnable(GL_DEPTH_TEST);//enable Depth buffer

    Shader ourShader("VertexShader.vs", "FragmentShader.fs");

    //Vertex Buffer Object
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    
    //Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // texture attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    //textures
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    

    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width, height, nrChannels;
    unsigned char* data = stbi_load("C:/Users/sbota/Desktop/Faculate/MG3D/Proiect/Tema_MG3D/Tema_MG3D/Textures/wall.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    ourShader.use();
    ourShader.setInt("texture", 0);

    // Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable keyboard controls

    // Setup ImGui backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::SetNextWindowSize(ImVec2(300, 100));// set intial window size



    //keep running the window until it gets the command to stop/render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //Check if there is any key pressed at the start of the frame generation
        processInput(window);

        //rendering
        // Chanage color of the background
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//clear old color/depth

        //Initialize UI window using ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        

        ourShader.use();
        // create transformations

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        ourShader.setMat4("projection", projection); // note: currently we set the projection matrix each frame, but since the projection matrix rarely changes it's often best practice to set it outside the main loop only once.


        glm::mat4 view = camera.GetViewMatrix(); // make sure to initialize matrix to identity matrix first
        ourShader.setMat4("view", view);



        glBindVertexArray(VAO);
        
        //reference cube
        //model = glm::translate(model, cubePositions[0]);
        //model = glm::scale(model, glm::vec3(0.5, 0.5, 0.5));
        //ourShader.setMat4("model", model);

        //glDrawArrays(GL_TRIANGLES, 0, 36);

        //handle objects
        for (unsigned int i = 0; i < cubePositions.size() ; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(cubeRotations[i]), cubeRotationDirection[i]);
            if (i == 0)
                model = glm::scale(model, glm::vec3(0.2, 0.2, 0.2)); //reference object
            else
                model = glm::scale(model, cubeScale[i]);


            ourShader.setMat4("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        //UI section
        
        UIInteractionHandler();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        //Swap buffers and call events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //Shut down ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate(); //Clean up all the resources
    return 0;
}

//Function for checking inputs
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 2.5f * deltaTime;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
    {
        if (!FPScamera)
            FPScamera = true;

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);

        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);
        
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, deltaTime);
        
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, deltaTime);
    }
    else
    {
        if (FPScamera)
            FPScamera = false;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    }
    
    //object manipulation
    if (!FPScamera)
    {
        if (translateMode)
            TranslateSelectedObject(selectedObject, window);

        if (rotateMode)
            RotateSelectedObject(selectedObject, window);

        if (scaleMode)
            ScaleSelectedObject(selectedObject, window);
    }

    
}

void UIInteractionHandler()
{
    ImGui::Begin("UI Window");
    ImGui::Text("You typed:");
    if (ImGui::Button("Add Cube", ImVec2(200, 50))) 
    {
        cubePositions.push_back(glm::vec3(0.0f, 0.0f, 0.0f));
        cubeRotations.push_back(0.0f);
        cubeRotationDirection.push_back(glm::vec3(0, 1, 0));
        cubeScale.push_back(glm::vec3(1, 1, 1));
    }
    if (ImGui::Button("Press to toggle translate", ImVec2(200, 50))) {
        if (translateMode && !rotateMode && !scaleMode)
            translateMode = false;
        else if (!translateMode && !rotateMode && !scaleMode)
            translateMode = true;
    }

    ImGui::SameLine();
    ImGui::Text(translateMode ? "ENABLED" : "DISABLED");

    if (ImGui::Button("Press to toggle rotate", ImVec2(200, 50))) {
        if (rotateMode && !translateMode && !scaleMode)
            rotateMode = false;
        else if (!rotateMode && !translateMode && !scaleMode)
            rotateMode = true;
    }

    ImGui::SameLine();
    ImGui::Text(rotateMode ? "ENABLED" : "DISABLED");

    if (ImGui::Button("Press to toggle scale", ImVec2(200, 50))) {
        if (scaleMode && !translateMode && !rotateMode)
            scaleMode = false;
        else if (!scaleMode && !translateMode && !rotateMode)
            scaleMode = true;
    }
    
    ImGui::SameLine();
    ImGui::Text(scaleMode ? "ENABLED" : "DISABLED");

    ImGui::Text("Objects:");

    for (int i = 1; i < cubePositions.size(); i++)
    {
        if (ImGui::Button(std::to_string(i).c_str(), ImVec2(50, 50))) 
        {
            selectedObject = i;
        }
        
        if(i % 6 != 0)
            ImGui::SameLine();

    }

    ImGui::End();
}

void TranslateSelectedObject(int targetObject, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cubePositions[targetObject] += glm::vec3(1, 0, 0) * deltaTime; //move right
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cubePositions[targetObject] -= glm::vec3(1, 0, 0) * deltaTime; //move left
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cubePositions[targetObject] += glm::vec3(0, 0, 1) * deltaTime; //move back
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cubePositions[targetObject] -= glm::vec3(0, 0, 1) * deltaTime; //move forward
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cubePositions[targetObject] += glm::vec3(0, 1, 0) * deltaTime; //move up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cubePositions[targetObject] -= glm::vec3(0, 1, 0) * deltaTime; //move down
}

void RotateSelectedObject(int targetObject, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cubeRotationDirection[targetObject] = glm::vec3(0, 1, 0);
        cubeRotations[targetObject] += 20.0f * deltaTime; //rotate right
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cubeRotationDirection[targetObject] = glm::vec3(0, 1, 0);
        cubeRotations[targetObject] -= 20.0f * deltaTime; //rotate left
    }
}

void ScaleSelectedObject(int targetObject, GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) cubeScale[targetObject] += glm::vec3(1, 1, 1) * deltaTime; //scale Up
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) cubeScale[targetObject] -= glm::vec3(1, 1, 1) * deltaTime; //scale Down
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if(!FPScamera) return;
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;
    

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//funtion responsible for adjusting window to resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}