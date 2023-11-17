
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Shader.h"

#include <vector>

#include "BasicMesh.h"
#include "basicCubeMesh.h"
#include "basicCylinderMesh.h" 
#include "basicConeMesh.h"
#include "basicSphereMesh.h"

#include "Camera.h"
#include "model.h"
#include <filesystem>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int useWireframe = 0;
int displayGrayscale = 0;

// camera - give starting point
Camera camera(glm::vec3(10.0f, 20.0f, 10.0f),  // 67.0f, 627.5f, 169.9f pretty top down angle
    glm::vec3(0.0f, 1.0f, 0.0f),
    -128.1f, -42.4f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int constant_i = 0;

glm::vec3 camerapositions[]{     // array holding default values of camera positions to be moved when arrow key's are pressed
    glm::vec3{10.0f,300.0f,10.0f},
    glm::vec3{-10.0f,20.0f,-10.0f},
    glm::vec3{0.0f,100.0f,0.0f}
};

int main()
{
#pragma region GLFW_INIT_&_SETUP

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "GameEngine", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);//****** changes

    glViewport(0, 0, 800, 600);

#pragma endregion

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //instantiate shader
    Shader myShader("resources/shaders/basic.shader.vert", "resources/shaders/basic.shader.frag");
    Shader heightShader("resources/shaders/heightmap.shader.vert", "resources/shaders/heightmap.shader.frag");

    //load texture
#pragma region TEXTURE
    
    unsigned int texture1;
     unsigned int textureT2;
     unsigned int textureT3;
    std::filesystem::path imagePath = "resources/textures/blackBoard.png";// resources/textures/blackBoard.png // for the White Chessboard cube texture
    std::filesystem::path imagePathT2 = "resources/textures/WhiteBoardErrors.png";// resources/textures/whiteBoard.png // for Black chessboard Cube texture 
    std::filesystem::path imagePathT3 = "resources/textures/StoneFloor.png"; // border Texture

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    //set the texture Wrap parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set Texture Wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem:: getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
    unsigned char* data = stbi_load( imagePath.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // fixed the ditortion for some images

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //// texture 2
    //// ---------
    glGenTextures(1, &textureT2);
    glBindTexture(GL_TEXTURE_2D, textureT2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePathT2.generic_string().c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    //// texture 3
    //// ---------
    glGenTextures(1, &textureT3);
    glBindTexture(GL_TEXTURE_2D, textureT3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image, create texture and generate mipmaps
    int width3, height3, nrChannels3;
    // load image, create texture and generate mipmaps
    data = stbi_load(imagePathT3.generic_string().c_str(), &width3, &height3, &nrChannels3, 0);
    if (data)
    {

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width3, height3, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    

    myShader.setInt("texture1", 0);
    myShader.setInt("textureT2", 1);
    myShader.setInt("textureT3", 2);
#pragma endregion

    std::filesystem::path castlemodel = "resources/Models/rook.obj";
    std::filesystem::path knightmodel = "resources/Models/knight.obj";
    std::filesystem::path bishopmodel = "resources/Models/bishop.obj";

    // load and create a texture
    // -------------------------
    
    unsigned int texture2;
    std::filesystem::path imagePath2 = "resources/heightmaps/HeightmapLow.png";   // "resources/heightmaps/iceland_heightmap.png"

    // load image, create texture and generate mipmaps
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    stbi_set_flip_vertically_on_load(true);
    int width2, height2, nrChannels2;
    unsigned char* data2 = stbi_load( imagePath2.generic_string().c_str(), &width2, &height2, &nrChannels2, 0);
    if (data2)
    {
        std::cout << "Loaded heightmap of size " << height2 << " x " << width2 << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    // texture Heightmap
    unsigned int textureheightcolor;
    std::filesystem::path imagePathheightcolor = "resources/heightmaps/Heightmapcolor3.png";
    // ---------
    glGenTextures(1, &textureheightcolor);
    glBindTexture(GL_TEXTURE_2D, textureheightcolor);
    //set the texture Wrap parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set Texture Wrap
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //load image, create texture and generate mipmaps
    int widthHC, heightHC, nrChannelsHC;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem:: getPath(...) is part of the GitHub repository so we can find files on any IDE/platform;
    unsigned char* dataHC = stbi_load(imagePathheightcolor.generic_string().c_str(), &widthHC, &heightHC, &nrChannelsHC, 0);
    if (dataHC)
    {
        //glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // fixed the ditortion for some images

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthHC, heightHC, 0, GL_RGBA, GL_UNSIGNED_BYTE, dataHC);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(dataHC);


    //after setup
    //define model properties

            std:: vector<basicCubeVertex> vertices = {
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},

            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},

            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},

            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},

            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(0.5f, -0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-0.5f, -0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},

            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(0.5f,  0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(0.5f,  0.5f,  0.5f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f,  0.5f), glm::vec2(0.0f, 0.0f)},
            {glm::vec3(-0.5f,  0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
    };
    basicCubeMesh myCube(vertices);

    // world space positions of our cubes    // ( left/right, up/down,front/back)
    glm::vec3 cubePositions[] = {
        glm::vec3(0.0f,  13.0f,  0.0f), //1
        glm::vec3(1.0f,  13.0f,  0.0f), //2
        glm::vec3(2.0f,  13.1f,  0.0f), //3
        glm::vec3(3.0f,  13.2f,  0.0f), //4
        glm::vec3(4.0f,  13.3f,  0.0f), //5
        glm::vec3(-1.0f,  13.1f,  0.0f), //6
        glm::vec3(-2.0f,  13.2f,  0.0f), //7
        glm::vec3(-3.0f,  13.3f,  0.0f), //8
        glm::vec3(1.0f,  13.0f,  1.0f), //1
        glm::vec3(0.0f,  13.3f,  1.0f), //2
        glm::vec3(3.0f,  13.2f,  1.0f), //3
        glm::vec3(2.0f,  13.1f,  1.0f), //4
        glm::vec3(-1.0f,  13.2f,  1.0f), //5
        glm::vec3(4.0f,  13.1f,  1.0f), //6
        glm::vec3(-3.0f,  13.2f,  1.0f), //7
        glm::vec3(-2.0f,  13.1f,  1.0f), //8
        glm::vec3(0.0f,  13.4f,  2.0f), //1
        glm::vec3(1.0f,  13.2f,  2.0f), //2
        glm::vec3(2.0f,  13.1f,  2.0f), //3
        glm::vec3(3.0f,  13.3f,  2.0f), //4
        glm::vec3(4.0f,  13.0f,  2.0f), //5
        glm::vec3(-1.0f,  13.1f,  2.0f), //6
        glm::vec3(-2.0f,  13.4f,  2.0f), //7
        glm::vec3(-3.0f,  13.2f,  2.0f), //8
        glm::vec3(1.0f,  13.4f,  3.0f), //1
        glm::vec3(0.0f,  13.2f,  3.0f), //2
        glm::vec3(3.0f,  13.1f,  3.0f), //3
        glm::vec3(2.0f,  13.2f,  3.0f), //4
        glm::vec3(-1.0f,  13.2f,  3.0f), //5
        glm::vec3(4.0f,  13.3f,  3.0f), //6
        glm::vec3(-3.0f,  13.4f,  3.0f), //7
        glm::vec3(-2.0f,  13.1f,  3.0f), //8
        glm::vec3(0.0f,  13.3f,  4.0f), //1
        glm::vec3(1.0f,  13.2f,  4.0f), //2
        glm::vec3(2.0f,  13.1f,  4.0f), //3
        glm::vec3(3.0f,  13.1f,  4.0f), //4
        glm::vec3(4.0f,  13.0f,  4.0f), //5
        glm::vec3(-1.0f,  13.4f,  4.0f), //6
        glm::vec3(-2.0f,  13.0f,  4.0f), //7
        glm::vec3(-3.0f,  13.3f,  4.0f), //8
        glm::vec3(1.0f,  13.2f,  -1.0f), //1
        glm::vec3(0.0f,  13.0f,  -1.0f), //2
        glm::vec3(3.0f,  13.1f,  -1.0f), //3
        glm::vec3(2.0f,  13.0f,  -1.0f), //4
        glm::vec3(-1.0f,  13.2f,  -1.0f), //5
        glm::vec3(4.0f,  13.2f,  -1.0f), //6
        glm::vec3(-3.0f,  13.0f,  -1.0f), //7
        glm::vec3(-2.0f,  13.1f,  -1.0f), //8
        glm::vec3(0.0f,  13.4f,  -2.0f), //1
        glm::vec3(1.0f,  13.2f,  -2.0f), //2
        glm::vec3(2.0f,  13.0f,  -2.0f), //3
        glm::vec3(3.0f,  13.1f,  -2.0f), //4
        glm::vec3(4.0f,  13.4f,  -2.0f), //5
        glm::vec3(-1.0f,  13.4f,  -2.0f), //6
        glm::vec3(-2.0f,  13.1f,  -2.0f), //7
        glm::vec3(-3.0f,  13.2f,  -2.0f), //8
        glm::vec3(1.0f,  13.1f,  -3.0f), //1
        glm::vec3(0.0f,  13.3f,  -3.0f), //2
        glm::vec3(3.0f,  13.0f,  -3.0f), //3
        glm::vec3(2.0f,  13.3f,  -3.0f), //4
        glm::vec3(-1.0f,  13.0f,  -3.0f), //5
        glm::vec3(4.0f,  13.3f,  -3.0f), //6
        glm::vec3(-3.0f,  13.1f,  -3.0f), //7
        glm::vec3(-2.0f,  13.2f,  -3.0f) //8
        
    };

    glm::vec3 cubePositions2[] = {
       glm::vec3(4.5f, 12.5f,  4.5f), //1
       glm::vec3(3.5f, 12.5f,  4.5f), //2
       glm::vec3(2.5f, 12.5f,  4.5f), //3
       glm::vec3(1.5f, 12.5f,  4.5f), //4
       glm::vec3(0.5f, 12.5f,  4.5f), //5
       glm::vec3(-0.5f, 12.5f,  4.5f), //6
       glm::vec3(-1.5f, 12.5f,  4.5f), //7
       glm::vec3(-2.5f, 12.5f,  4.5f), //8
       glm::vec3(-3.5f, 12.5f,  4.5f), //9
       glm::vec3(4.5f, 12.5f,  3.5f), //1
       glm::vec3(4.5f, 12.5f,  2.5f), //2
       glm::vec3(4.5f, 12.5f,  1.5f), //3
       glm::vec3(4.5f, 12.5f,  0.5f), //4
       glm::vec3(4.5f, 12.5f,  -0.5f), //5
       glm::vec3(4.5f, 12.5f,  -1.5f), //6
       glm::vec3(4.5f, 12.5f,  -2.5f), //7
       glm::vec3(4.5f, 12.5f,  -3.5f), //8
      glm::vec3(4.5f, 12.5f,  -3.5f), //1
       glm::vec3(3.5f, 12.5f,  -3.5f), //2
       glm::vec3(2.5f, 12.5f,  -3.5f), //3
       glm::vec3(1.5f, 12.5f,  -3.5f), //4
       glm::vec3(0.5f, 12.5f,  -3.5f), //5
       glm::vec3(-0.5f, 12.5f,  -3.5f), //6
       glm::vec3(-1.5f, 12.5f,  -3.5f), //7
       glm::vec3(-2.5f, 12.5f,  -3.5f), //8
       glm::vec3(-3.5f, 12.5f,  -3.5f),
       glm::vec3(-3.5f, 12.5f,  3.5f), //1
       glm::vec3(-3.5f, 12.5f,  2.5f), //2
       glm::vec3(-3.5f, 12.5f,  1.5f), //3
       glm::vec3(-3.5f, 12.5f,  0.5f), //4
       glm::vec3(-3.5f, 12.5f,  -0.5f), //5
       glm::vec3(-3.5f, 12.5f,  -1.5f), //6
       glm::vec3(-3.5f, 12.5f,  -2.5f), //7
       glm::vec3(-3.5f, 12.5f,  -3.5f) //8
        
    };
    
    glm::vec3 spherePawnPositions[] = {
       glm::vec3(4.0f, 15.0f,  3.0f), //1
       glm::vec3(3.0f, 15.0f,  3.0f), //2
       glm::vec3(2.0f, 15.0f,  3.0f), //3
       glm::vec3(1.0f, 15.0f,  3.0f), //4
       glm::vec3(0.0f, 15.0f,  3.0f), //5
       glm::vec3(-1.0f, 15.0f,  3.0f), //6
       glm::vec3(-2.0f, 15.0f,  3.0f), //7
       glm::vec3(-3.0f, 15.0f,  3.0f), //8

       glm::vec3(4.0f, 15.0f,  -2.0f), //1
       glm::vec3(3.0f, 15.0f,  -2.0f), //2
       glm::vec3(2.0f, 15.0f,  -2.0f), //3
       glm::vec3(1.0f, 15.0f,  -2.0f), //4
       glm::vec3(0.0f, 15.0f,  -2.0f), //5
       glm::vec3(-1.0f, 15.0f,  -2.0f), //6
       glm::vec3(-2.0f, 15.0f,  -2.0f), //7
       glm::vec3(-3.0f, 15.0f,  -2.0f)  //8
    };
    glm::vec3 sphereKnightPositions[] = {
       glm::vec3(3.0f, 15.0f,  4.0f), //1
       glm::vec3(-2.0f, 15.0f,  4.0f), //2
       glm::vec3(3.0f, 15.0f,  -3.0f),//3
       glm::vec3(-2.0f, 15.0f,  -3.0f) //4
    };
    glm::vec3 sphereBishopPositions[]{
          glm::vec3(2.0f, 16.0f, 4.0f),//1
        glm::vec3(-1.0f, 16.0f, 4.0f),//2
        glm::vec3(2.0f, 16.0f, -3.0f),//3
        glm::vec3(-1.0f, 16.0f, -3.0f)//4
    };



    glm::vec3 cylinderCastlePositions[] = {
       glm::vec3(4.0f, 14.0f,  4.0f), //1
       glm::vec3(-3.0f, 14.0f,  4.0f), //2
       glm::vec3(4.0f, 14.0f,  -3.0f),//3
       glm::vec3(-3.0f, 14.0f,  -3.0f) //4
    };
    glm::vec3 cylinderKnightPositions[] = {
       glm::vec3(3.0f, 14.0f,  4.0f), //1
       glm::vec3(-2.0f, 14.0f,  4.0f), //2
       glm::vec3(3.0f, 14.0f,  -3.0f),//3
       glm::vec3(-2.0f, 14.0f,  -3.0f) //4
    };
    glm::vec3 cylinderBishopPosition[] = {
         glm::vec3(2.0f, 14.0f, 4.0f),//1
        glm::vec3(-1.0f, 14.0f, 4.0f),//2
        glm::vec3(2.0f, 14.0f, -3.0f),//3
        glm::vec3(-1.0f, 14.0f, -3.0f)//4
    };
    


    glm::vec3 conePawnPositions[] = {
       glm::vec3(4.0f, 14.5f,  3.0f), //1
       glm::vec3(3.0f, 14.5f,  3.0f), //2
       glm::vec3(2.0f, 14.5f,  3.0f), //3
       glm::vec3(1.0f, 14.5f,  3.0f), //4
       glm::vec3(0.0f, 14.5f,  3.0f), //5
       glm::vec3(-1.0f, 14.5f,  3.0f), //6
       glm::vec3(-2.0f, 14.5f,  3.0f), //7
       glm::vec3(-3.0f, 14.5f,  3.0f), //8

       glm::vec3(4.0f, 14.5f,  -2.0f), //1
       glm::vec3(3.0f, 14.5f,  -2.0f), //2
       glm::vec3(2.0f, 14.5f,  -2.0f), //3
       glm::vec3(1.0f, 14.5f,  -2.0f), //4
       glm::vec3(0.0f, 14.5f,  -2.0f), //5
       glm::vec3(-1.0f, 14.5f,  -2.0f), //6
       glm::vec3(-2.0f, 14.5f,  -2.0f), //7
       glm::vec3(-3.0f, 14.5f,  -2.0f)  //8
    }; 
    glm::vec3 coneKnightPositions[] = {
       glm::vec3(3.0f, 15.0f,  3.5f), //1
       glm::vec3(-2.0f, 15.0f,  3.5f), //2
       glm::vec3(3.0f, 15.0f,  -3.5f),//3
       glm::vec3(-2.0f, 15.0f,  -3.5f) //4
    };
    glm::vec3 coneBishopPositions[] = {
         glm::vec3(2.0f, 15.2f, 4.0f),//1
        glm::vec3(-1.0f, 15.2f, 4.0f),//2
        glm::vec3(2.0f, 15.2f, -3.0f),//3
        glm::vec3(-1.0f,15.2f,-3.0f)//4

    };
    glm::vec3 coneQueen1Positions[] = {

                glm::vec3(1.0f, 14.0f, 4.0f),//1
        glm::vec3(1.0f, 14.0f, -3.0f),//2

    };
    glm::vec3 coneQueen2Positions[] = {
                glm::vec3(1.0f, 15.0f, 4.0f),//1
        glm::vec3(1.0f, 15.0f, -3.0f)//2
    };

    glm::vec3 coneKingPositions[] = {
                glm::vec3(0.0f, 14.0f, 4.0f),//1
        glm::vec3(0.0f, 14.0f, -3.0f),//2

                glm::vec3(0.0f, 15.0f, 4.0f),//3
        glm::vec3(0.0f, 15.0f, -3.0f)//4
    };

    glm::vec3 cubeCastlePositions[] = {
       glm::vec3(4.0f, 15.0f,  4.0f), //1
       glm::vec3(-3.0f, 15.0f,  4.0f), //2
       glm::vec3(4.0f, 15.0f,  -3.0f),//3
       glm::vec3(-3.0f, 15.0f,  -3.0f) //4
    };




    #pragma region Vertex data
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    std::vector<float> heightvertices;
    float yScale = 64.0f / 256.0f, yShift = 16.0f;
    int rez = 1;
    unsigned bytePerPixel = nrChannels2;
    for (int i = 0; i < height2; i++)
    {
        for (int j = 0; j < width2; j++)
        {
            unsigned char* pixelOffset = data2 + (j + width2 * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            // vertex
            heightvertices.push_back(-height2 / 2.0f + height2 * i / (float)height2);   // vx
            heightvertices.push_back((int)y * yScale - yShift);   // vy
            heightvertices.push_back(-width2 / 2.0f + width2 * j / (float)width2);   // vz
        }
    }
    std::cout << "Loaded " << heightvertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data2);

    std::vector<unsigned> indices;
    for (unsigned i = 0; i < height2 - 1; i += rez)
    {
        for (unsigned j = 0; j < width2; j += rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width2 * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    const int numStrips = (height2 - 1) / rez;
    const int numTrisPerStrip = (width2 / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    // first, configure the cube's VAO (and terrainVBO + terrainIBO)
    unsigned int terrainVAO, terrainVBO, terrainIBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, heightvertices.size() * sizeof(float), &heightvertices[0], GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);

#pragma endregion

    //cylinder model properties Castle
    float cylinderRadius = 0.3f;
    float cylinderHeight = 1.5f;
    int cylinderSides = 16; // Adjust the number of sides as needed

    basicCylinderMesh myCylinder(cylinderRadius, cylinderHeight, cylinderSides);

    //cylinder model properties Knight
    float cylinderRadius2 = 0.3f;
    float cylinderHeight2 = 1.5f;
    int cylinderSides2 = 16; // Adjust the number of sides as needed

    basicCylinderMesh myKnightCylinder(cylinderRadius2, cylinderHeight2, cylinderSides2);

    // cone model properties
    float coneRadius = 0.5f;
    float coneHeight = 1.0f;
    int coneSides = 16; // Adjust the number of sides as needed

    basicConeMesh myCone(coneRadius, coneHeight, coneSides);

    // cone model properties Knight
    float coneRadius2 = 0.5f;
    float coneHeight2 = 1.0f;
    int coneSides2 = 16; // Adjust the number of sides as needed

    basicConeMesh myKnightCone(coneRadius2, coneHeight2, coneSides2);

    //sphere model
    float sphereRadius = 0.5f;
    int sphereSegments = 32; // Adjust the number of segments as needed

    basicSphereMesh mySphere(sphereRadius, sphereSegments);

    //sphere model knight
    float sphereRadius2 = 0.5f;
    int sphereSegments2 = 32; // Adjust the number of segments as needed

    basicSphereMesh myKnightSphere(sphereRadius2, sphereSegments2);
    
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //        std::cout << deltaTime << "ms (" << 1.0f / deltaTime << " FPS)" << std::endl;

        //input
        processInput(window);

        //processing
        glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//******* changes);

        //glBindTexture(GL_TEXTURE_2D, texture1);
        
        #pragma region Heightmap
        // be sure to activate shader when setting uniforms/drawing objects
       // myShader.use();
        heightShader.use();

        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        heightShader.setMat4("projection", projection);
        heightShader.setMat4("view", view);

        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        heightShader.setMat4("model", model);

        // render the cube
        glBindVertexArray(terrainVAO);
        //        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        for (unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,   // primitive type
                numTrisPerStrip + 2,   // number of indices to render
                GL_UNSIGNED_INT,     // index data type
                (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip)); // offset to starting index
            glBindTexture(GL_TEXTURE_2D, textureheightcolor);
        }
#pragma endregion

        #pragma region Chessboard
        ////=============================================
        ////cube 
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F,50.0F,0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale
        
        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);
        //myCube.Draw(myShader);

        for (unsigned int i = 0; i < 64; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i % 2 == 0)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCube.Draw(myShader);
        }


        ////=============================================
        ////cube 2
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);
        //myCube.Draw(myShader);

        for (unsigned int i = 0; i < 34; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions2[i]);
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i % 2 == 0)
                glBindTexture(GL_TEXTURE_2D, textureT3);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT3);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCube.Draw(myShader);
        }
#pragma endregion

        #pragma region Pawn
        ////=============================================
        ////sphere pawns
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 16; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, spherePawnPositions[i]);
            model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 8)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            mySphere.Draw(myShader);
        }
        ////=============================================
       ////cone pawn
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 16; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, conePawnPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 8)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }

        #pragma endregion

        #pragma region Castle
        ////=============================================
        ////cylinder Castle
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cylinderCastlePositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(1.0f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCylinder.Draw(myShader);
        }
       
        ////=============================================
        ////cube Castle
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubeCastlePositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::scale(model, glm::vec3(0.7f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCube.Draw(myShader);
        }
#pragma endregion

        #pragma region Knight
        
        ////=============================================
        ////cylinder Knight
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cylinderKnightPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(1.0f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myKnightCylinder.Draw(myShader);
        }
        ////=============================================
        ////sphere Knight
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, sphereKnightPositions[i]);
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myKnightSphere.Draw(myShader);
        } 
        ////=============================================
       ////cone Knight
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKnightPositions[i]); 
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(0.8f, 0.8f, 0.8f));
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
                model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myKnightCone.Draw(myShader);
        }
#pragma endregion

        #pragma region Bishop
        ////cylinder bishops
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cylinderBishopPosition[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(1.0f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCylinder.Draw(myShader);
        }

        ////cone bishops
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneBishopPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }

        ////sphere bishops
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, sphereBishopPositions[i]);
            model = glm::scale(model, glm::vec3(0.6f, 0.6f, 0.6f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }

            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            mySphere.Draw(myShader);
        }

#pragma endregion

        #pragma region Queen

        ////cone queen
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneQueen1Positions[i]);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }

        ////cone queen
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneQueen2Positions[i]);
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }


#pragma endregion

        #pragma region King

        ////cone queen
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 4; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKingPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 2)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }

#pragma endregion

    #pragma region ModelCastle

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        myShader.setMat4("model", model);
        castlemodel.Draw(castlemodel);

#pragma endregion

#pragma region Modelbishop

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(5.0f, 5.0f, 5.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        myShader.setMat4("model", model);
        bishopmodel.Draw(bishopmodel);

#pragma endregion

#pragma region Modelknight

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(10.0f, 10.0f, 10.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        myShader.setMat4("model", model);
        knightmodel.Draw(knightmodel);

#pragma endregion

        

        //math
        
        //frame buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;

}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
void processInput(GLFWwindow* window) // inputs manager
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        camera = camerapositions[--constant_i];

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        camera = camerapositions[++constant_i];

    if (constant_i < 0)
        constant_i = 2;

    if (constant_i > 2)
        constant_i = 0;

}
// glfw: whenever a key event occurs, this callback is called
// ---------------------------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers)
{
    if (action == GLFW_PRESS)
    {
        switch (key)
        {
        case GLFW_KEY_SPACE:
            useWireframe = 1 - useWireframe;
            break;
        case GLFW_KEY_G:
            displayGrayscale = 1 - displayGrayscale;
            break;
        default:
            break;
        }
    }
}
// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}
