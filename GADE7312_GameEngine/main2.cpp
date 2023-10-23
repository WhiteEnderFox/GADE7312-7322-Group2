//#define STB_IMAGE_IMPLEMENTATION
//#include "stb_image.h"
//#include <glad/glad.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include "shader.h"
//#include <vector>
//#include "basicMesh.h"
//#include <filesystem>
//#include <glm/gtc/matrix_transform.hpp>
//#include<glm/gtc/type_ptr.hpp>
//#include "basicCubeMesh.h"
//#include "camera.h"
//
//#include "basicCylinderMesh.h" 
//#include "basicConeMesh.h"
//#include "basicSphereMesh.h"
//
//
////using namespace std;
//ggggg
//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
//void processInput(GLFWwindow* window);
//
//unsigned int loadTexture(const char* path);
//
//// settings
//const unsigned int SCR_WIDTH = 800;
//const unsigned int SCR_HEIGHT = 600;
//
//// camera
//Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
//float lastX = SCR_WIDTH / 2.0f;
//float lastY = SCR_HEIGHT / 2.0f;
//bool firstMouse = true;
//
//// timing
//float deltaTime = 0.0f;	// time between current frame and last frame
//float lastFrame = 0.0f;
//
//// lighting
//glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
//
//int main()
//{
//#pragma region GLFW_INIT_&_SETUP
//    glfwInit();
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//
//
//    GLFWwindow* window = glfwCreateWindow(800, 600, "MyGameEngine", NULL, NULL);
//    if (window == NULL)
//    {
//        std::cout << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//    glfwSetCursorPosCallback(window, mouse_callback);
//    glfwSetScrollCallback(window, scroll_callback);
//
//    // tell GLFW to capture our mouse
//    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//
//    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//    {
//        std::cout << "Failed to initialize GLAD" << std::endl;
//        return -1;
//    }
//
//
//    // configure global opengl state
//    // -----------------------------
//    glEnable(GL_DEPTH_TEST);//****** changes
//
//    glViewport(0, 0, 800, 600);
//
//#pragma endregion
//
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//    //instantiate shader 
//    Shader myShader("resources/shaders/basic.shader.vert", "resources/shaders/basic.shader.frag");
//    Shader myLight("resources/shaders/lighting.shader.vert", "resources/shaders/lighting.shader.frag");
//    Shader myLightSource("resources/shaders/lightSource.shader.vert", "resources/shaders/lightSource.shader.frag");
//
//    //load texture
//#pragma region TEXTURE
//
//    unsigned int texture1;
//    //unsigned int diffuseMap;
//    std::filesystem::path imagePath = "resources/textures/garfield.png";
//    //std::filesystem::path diffusePath = "resources/textures/container2.png";
//
//    unsigned int diffuseMap = loadTexture("resources/textures/container2.png");
//    unsigned int specularMap = loadTexture("resources/textures/container2_specular.png");
//
//    //texture 1
//
//    glGenTextures(1, &texture1);
//    glBindTexture(GL_TEXTURE_2D, texture1); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//    // set the texture wrapping parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    // set texture filtering parameters
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    // load image, create texture and generate mipmaps
//    int width, height, nrChannels;
//    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//    unsigned char* data = stbi_load(imagePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
//    if (data)
//    {
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//    }
//    else
//    {
//        std::cout << "Failed to load texture" << std::endl;
//    }
//    stbi_image_free(data);
//
//
//    //diffuseMap
//    //glGenTextures(1, &diffuseMap);
//    //glBindTexture(GL_TEXTURE_2D, diffuseMap); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
//    //// set the texture wrapping parameters
//    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
//    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    //// set texture filtering parameters
//    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    //int width2, height2, nrChannels2;
//    //// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
//    //unsigned char* data2 = stbi_load(diffusePath.generic_string().c_str(), &width2, &height2, &nrChannels2, 0);
//    //if (data2)
//    //{
//    //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width2, height2, 0, GL_RGBA, GL_UNSIGNED_BYTE, data2);
//    //    glGenerateMipmap(GL_TEXTURE_2D);
//    //}
//    //else
//    //{
//    //    std::cout << "Failed to load texture" << std::endl;
//    //}
//    //stbi_image_free(data2);
//#pragma endregion
//
//
//    //after setup 
//    //define model properties
//     
//    //std::vector<basicLightVertex>vertices = {
//
//    //    /*{ glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //    { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //    { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //    { glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//
//    //    { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //    { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //    { glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //    { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
//    //    };*/
//
//    //    // Front face
//    //   { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//
//    //   // Back face
//    //   { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//
//    //   // Left face
//    //   { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//    //   
//    //   // Right face
//    //   { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//
//    //   //Top face
//    //   { glm::vec3(0.5f, -0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(0.5f, -0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
//
//    //   //Bottom face
//    //   { glm::vec3(0.5f, 0.5f, -0.5f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
//    //   { glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
//    //   { glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) }
//    //};
//
//    //std::vector<unsigned int>indices = {
//    //    //0, 1, 2, 2, 3, 0,    // Front Face
//    //    //4, 7, 6, 6, 5, 4,    // Back Face
//    //    //0, 4, 5, 5, 1, 0,    // Left Face
//    //    //3, 2, 6, 6, 7, 3,    // Right Face
//    //    //1, 5, 6, 6, 2, 1,    // Top Face
//    //    //4, 0, 3, 3, 7, 4     // Bottom Face
//    //    // 
//    //   // Front face
//    //        0, 1, 2, 2, 3, 0,
//
//    //        // Back face
//    //        4, 5, 6, 6, 7, 4,
//
//    //        // Left face
//    //        8, 9, 10, 10, 11, 8,
//
//    //        // Right face
//    //        12, 13, 14, 14, 15, 12,
//
//    //        16, 17, 18, 18, 19, 16,
//    //        // Top Face
//    //    20, 21, 22, 22, 23, 20    // Bottom Face
//
//    //};
//    //basicLightMesh myCube(vertices, indices);
//   
//
//    myLight.use();
//    myLight.setInt("material.diffuse", 0);
//    myLight.setInt("material.specular", 1);
//
//    //BaseLight myLighter(glm::vec3(1.2f, 1.0f, 2.0f), glm::vec3(0.2f, 0.2f, 0.2f), glm::vec3(0.8f, 0.8f, 0.8f), glm::vec3(1.0f, 1.0f, 1.0f));
//
//    //cylinder model properties
//    float cylinderRadius = 0.5f;
//    float cylinderHeight = 1.0f;
//    int cylinderSides = 16; // Adjust the number of sides as needed
//
//    basicCylinderMesh myCylinder(cylinderRadius, cylinderHeight, cylinderSides);
//
//    // cone model properties
//    float coneRadius = 0.5f;
//    float coneHeight = 1.0f;
//    int coneSides = 16; // Adjust the number of sides as needed
//
//    basicConeMesh myCone(coneRadius, coneHeight, coneSides);
//
//    //sphere model
//    float sphereRadius = 0.5f;
//    int sphereSegments = 32; // Adjust the number of segments as needed
//
//    basicSphereMesh mySphere(sphereRadius, sphereSegments);
//
//    while (!glfwWindowShouldClose(window))
//    {
//        // per-frame time logic
//        // --------------------
//        float currentFrame = static_cast<float>(glfwGetTime());
//        deltaTime = currentFrame - lastFrame;
//        lastFrame = currentFrame;
//
//        //inputs
//        processInput(window);
//
//        //processing
//        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//******* changes
//
//        glBindTexture(GL_TEXTURE_2D, texture1);
//        //glBindTexture(GL_TEXTURE_2D, diffuseMap);
//
//        //define vector  and initialize it to an identity matrix  
//        // be sure to activate shader when setting uniforms/drawing objects
//        // 
//       
//        //myLight.use();
//        //
//        ////myLight.setVec3("light.diffuse", glm::vec3(0.9f, 0.0f, 0.0f));
//        //myLight.setVec3("viewPos", camera.Position);
//
//        ////light properties
//        //myLight.setLight("light",myLighter);
//        //
//        //
//        //// material properties
//        ////myLight.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
//        //myLight.setFloat("material.shininess", 1.0f);
//
//
//        //// view/projection transformations
//        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        //glm::mat4 view = camera.GetViewMatrix();
//        //myLight.setMat4("projection", projection);
//        //myLight.setMat4("view", view);
//
//        //// world transformation
//        //glm::mat4 model = glm::mat4(1.0f);
//        //myLight.setMat4("model", model);
//
//        //// bind diffuse map
//        //glActiveTexture(GL_TEXTURE0);
//        //glBindTexture(GL_TEXTURE_2D, diffuseMap);
//        //// bind specular map
//        //glActiveTexture(GL_TEXTURE1);
//        //glBindTexture(GL_TEXTURE_2D, specularMap);
//        //
//        ////render the cube
//        //myCube.Draw(myLight);
//
//        //// also draw the lamp object
//        //myLightSource.use();
//        //myLightSource.setMat4("projection", projection);
//        //myLightSource.setMat4("view", view);
//        //model = glm::mat4(1.0f);
//        //model = glm::translate(model, lightPos);
//        //model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
//        //myLightSource.setMat4("model", model);
//        //myCube.Draw(myLightSource);
//
//        //render cylinder
//        //// view/projection transformations
//        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        //glm::mat4 view = camera.GetViewMatrix();
//        //myShader.setMat4("projection", projection);
//        //myShader.setMat4("view", view);
//
//        //// world transformation
//        //glm::mat4 model = glm::mat4(1.0f);
//        //myShader.setMat4("model", model);
//        //myCylinder.Draw(myShader);
//        ////std::cout << "cylinder failed to load "  << std::endl;
//
//        //render cone
//        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        //glm::mat4 view = camera.GetViewMatrix();
//        //myShader.setMat4("projection", projection);
//        //myShader.setMat4("view", view);
//
//        //// world transformation
//        //glm::mat4 model = glm::mat4(1.0f);
//        //myShader.setMat4("model", model);
//        //myCone.Draw(myShader);
//
//        //render sphere
//        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
//        glm::mat4 view = camera.GetViewMatrix();
//        myShader.setMat4("projection", projection);
//        myShader.setMat4("view", view);
//
//        // world transformation
//        glm::mat4 model = glm::mat4(1.0f);
//        myShader.setMat4("model", model);
//        mySphere.Draw(myShader);
//
//
//
//        //math
//
//        //frame buffers
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glfwTerminate();
//    return 0;
//
//    //return 0;
//}
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//}
//
//void processInput(GLFWwindow* window)
//{
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//
//    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
//        camera.ProcessKeyboard(FORWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
//        camera.ProcessKeyboard(BACKWARD, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
//        camera.ProcessKeyboard(LEFT, deltaTime);
//    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
//        camera.ProcessKeyboard(RIGHT, deltaTime);
//    //all input managing things
//}
//
//void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
//{
//    float xpos = static_cast<float>(xposIn);
//    float ypos = static_cast<float>(yposIn);
//
//    if (firstMouse)
//    {
//        lastX = xpos;
//        lastY = ypos;
//        firstMouse = false;
//    }
//
//    float xoffset = xpos - lastX;
//    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
//
//    lastX = xpos;
//    lastY = ypos;
//
//    camera.ProcessMouseMovement(xoffset, yoffset);
//}
//
//// glfw: whenever the mouse scroll wheel scrolls, this callback is called
//// ----------------------------------------------------------------------
//void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
//{
//    camera.ProcessMouseScroll(static_cast<float>(yoffset));
//}
//
//// utility function for loading a 2D texture from file
//// ---------------------------------------------------
//unsigned int loadTexture(char const* path)
//{
//    unsigned int textureID;
//    glGenTextures(1, &textureID);
//
//    int width, height, nrComponents;
//    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
//    if (data)
//    {
//        GLenum format;
//        if (nrComponents == 1)
//            format = GL_RED;
//        else if (nrComponents == 3)
//            format = GL_RGB;
//        else if (nrComponents == 4)
//            format = GL_RGBA;
//
//        glBindTexture(GL_TEXTURE_2D, textureID);
//        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
//        glGenerateMipmap(GL_TEXTURE_2D);
//
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//        stbi_image_free(data);
//    }
//    else
//    {
//        std::cout << "Texture failed to load at path: " << path << std::endl;
//        stbi_image_free(data);
//    }
//
//    return textureID;
//}