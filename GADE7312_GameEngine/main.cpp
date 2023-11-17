
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <map>
#include <string>

#include "Shader.h"

#include <vector>

#include "BasicMesh.h"
#include "basicCubeMesh.h"
#include "basicCylinderMesh.h" 
#include "basicConeMesh.h"
#include "basicSphereMesh.h"

#include "Camera.h"
#include "AnimationController.cpp"

#include "model.h"

#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <ft2build.h>
#include FT_FREETYPE_H

//using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int modifiers);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
//void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
int useWireframe = 0;
int displayGrayscale = 0;

///// Holds all state information relevant to a character as loaded using FreeType
//struct Character {
//    unsigned int TextureID; // ID handle of the glyph texture
//    glm::ivec2   Size;      // Size of glyph
//    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
//    unsigned int Advance;   // Horizontal offset to advance to next glyph
//};
//
//std::map<GLchar, Character> Characters;
//unsigned int VAO, VBO;

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
// lighting
glm::vec3 lightPos(0.0f, 20.0f, -7.0f);

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
    Shader lightingShader("resources/shaders/lighting.shader.vert", "resources/shaders/lighting.shader.frag");
    Shader lightCubeShader("resources/shaders/lightSource.shader.vert", "resources/shaders/lightSource.shader.frag");
    Shader skyboxShader("resources/shaders/skybox.shader.vert", "resources/shaders/skybox.shader.frag");
    Shader textShader("resources/shaders/text.shader.vert", "resources/shaders/text.shader.frag");
    Shader importShader("resources/shaders/importBasic.shader.vert", "resources/shaders/importsBasic.shader.vert");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    textShader.use();
    glUniformMatrix4fv(glGetUniformLocation(textShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));


    
    #pragma region FreetypeFontText
    //// FreeType
    //// --------
    //FT_Library ft;
    //// All functions return a value different than 0 whenever an error occurred
    //if (FT_Init_FreeType(&ft))
    //{
    //    std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    //    return -1;
    //}

    //// find path to font
    //std::string font_name = ("resources/fonts/Antonio-Bold.ttf");
    //if (font_name.empty())
    //{
    //    std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
    //    return -1;
    //}
    //// load font as face
    //FT_Face face;
    //if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
    //    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
    //    return -1;
    //}
    //else {
    //    // set size to load glyphs as
    //    FT_Set_Pixel_Sizes(face, 0, 48);

    //    // disable byte-alignment restriction
    //    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //    // load first 128 characters of ASCII set
    //    for (unsigned char c = 0; c < 128; c++)
    //    {
    //        // Load character glyph 
    //        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
    //        {
    //            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
    //            continue;
    //        }
    //        // generate texture
    //        unsigned int texture;
    //        glGenTextures(1, &texture);
    //        glBindTexture(GL_TEXTURE_2D, texture);
    //        glTexImage2D(
    //            GL_TEXTURE_2D,
    //            0,
    //            GL_RED,
    //            face->glyph->bitmap.width,
    //            face->glyph->bitmap.rows,
    //            0,
    //            GL_RED,
    //            GL_UNSIGNED_BYTE,
    //            face->glyph->bitmap.buffer
    //        );
    //        // set texture options
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //        // now store character for later use
    //        Character character = {
    //            texture,
    //            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
    //            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
    //            static_cast<unsigned int>(face->glyph->advance.x)
    //        };
    //        Characters.insert(std::pair<char, Character>(c, character));
    //    }
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}
    //// destroy FreeType once we're finished
    //FT_Done_Face(face);
    //FT_Done_FreeType(ft);
    //// configure VAO/VBO for texture quads
    //// -----------------------------------
    //glGenVertexArrays(1, &VAO);
    //glGenBuffers(1, &VBO);
    //glBindVertexArray(VAO);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    //glEnableVertexAttribArray(0);
    //glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    //glBindBuffer(GL_ARRAY_BUFFER, 0);
    //glBindVertexArray(0);

#pragma endregion

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

    const char* modelpath1 = "resources/Models/rook.obj";
    const char* modelpath2 = "resources/Models/knight.obj";
    const char* modelpath3 = "resources/Models/bishop.obj";
    
    Model castlemodel(modelpath1);
    Model knightmodel(modelpath2);
    Model bishopmodel(modelpath3);

    #pragma region Heightmap & texture
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

#pragma endregion

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

    #pragma region ChessboardPositions
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
#pragma endregion

    #pragma region ChessPiece Locations
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
    glm::vec3 sphereKingPositions[] = {
       glm::vec3(1.0f, 15.4f,  4.0f), //1
       glm::vec3(0.0f, 15.4f,  -3.0f), //2
    };

    glm::vec3 sphereKnightPositions[] = {
       glm::vec3(3.0f, 15.0f,  4.0f), //1
       glm::vec3(-2.0f, 15.0f,  4.0f), //2
       glm::vec3(3.0f, 15.0f,  -3.0f),//3
       glm::vec3(-2.0f, 15.0f,  -3.0f) //4
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
    glm::vec3 cylinderBishopPositions[] = {
       glm::vec3(2.0f, 14.0f,  4.0f), //1
       glm::vec3(-1.0f, 14.0f,  4.0f), //2
       glm::vec3(2.0f, 14.0f,  -3.0f),//3
       glm::vec3(-1.0f, 14.0f,  -3.0f) //4
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
    };
    glm::vec3 coneKnight2Positions[] = {
       glm::vec3(3.0f, 15.0f,  -2.5f),//3
       glm::vec3(-2.0f, 15.0f,  -2.5f) //4
    };
    glm::vec3 coneBishopPositions[] = {
       glm::vec3(2.0f, 15.0f,  4.0f), //1
       glm::vec3(-1.0f, 15.0f,  4.0f), //2
       glm::vec3(2.0f, 15.0f,  -3.0f),//3
       glm::vec3(-1.0f, 15.0f,  -3.0f) //4
    };
    glm::vec3 coneQueenPositions[] = {
       glm::vec3(0.0f, 14.5f,  4.0f), //1
       glm::vec3(1.0f, 14.5f,  -3.0f), //2
    };
    glm::vec3 coneQueen2Positions[] = {
       glm::vec3(0.0f, 15.1f,  4.0f), //1
       glm::vec3(1.0f, 15.1f,  -3.0f), //2
    };
    glm::vec3 coneKingPositions[] = {
       glm::vec3(1.0f, 14.5f,  4.0f), //1
       glm::vec3(0.0f, 14.5f,  -3.0f), //2
    };
    glm::vec3 coneKing2Positions[] = {
       glm::vec3(1.0f, 15.1f,  4.0f), //1
       glm::vec3(0.0f, 15.1f,  -3.0f), //2
    };

    glm::vec3 cubeCastlePositions[] = {
       glm::vec3(4.0f, 14.8f,  4.0f), //1
       glm::vec3(-3.0f, 14.8f,  4.0f), //2
       glm::vec3(4.0f, 14.8f,  -3.0f),//3
       glm::vec3(-3.0f, 14.8f,  -3.0f) //4
    };
    glm::vec3 cubeBishopPositions[] = {
       glm::vec3(2.0f, 15.1f,  4.0f), //1
       glm::vec3(-1.0f, 15.1f,  4.0f), //2
       glm::vec3(2.0f, 15.1f,  -3.0f),//3
       glm::vec3(-1.0f, 15.1f,  -3.0f) //4
    };
#pragma endregion

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

   #pragma region Primitives Chesspieces
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

    //cylinder model properties Knight
    float cylinderRadius3 = 0.3f;
    float cylinderHeight3 = 1.5f;
    int cylinderSides3 = 16; // Adjust the number of sides as needed

    basicCylinderMesh myBishopCylinder(cylinderRadius3, cylinderHeight3, cylinderSides3);

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

    // cone model properties Knight
    float coneRadius3 = 0.5f;
    float coneHeight3 = 1.0f;
    int coneSides3 = 16; // Adjust the number of sides as needed

    basicConeMesh myBishopCone(coneRadius3, coneHeight3, coneSides3);

   
    //sphere model
    float sphereRadius3 = 0.5f;
    int sphereSegments3 = 32; // Adjust the number of segments as needed

    basicSphereMesh mySphere(sphereRadius3, sphereSegments3);

    //sphere model knight
    float sphereRadius2 = 0.5f;
    int sphereSegments2 = 32; // Adjust the number of segments as needed

    basicSphereMesh myKnightSphere(sphereRadius2, sphereSegments2);

#pragma endregion
    

    #pragma region skybox

    //Skybox
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces;
    {
        std::filesystem::path skypathright = "resources/textures/skybox/right.jpg";
        std::filesystem::path skypathleft = "resources/textures/skybox/left.jpg";
        std::filesystem::path skypathtop = "resources/textures/skybox/top.jpg";
        std::filesystem::path skypathbottom = "resources/textures/skybox/bottom.jpg";
        std::filesystem::path skypathfront = "resources/textures/skybox/front.jpg";
        std::filesystem::path skypathback = "resources/textures/skybox/back.jpg";
        
        //std::cout << "faces textures ---";
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //skyboxShader.use();
    skyboxShader.setInt("skybox", 0);
    skyboxShader.use();

#pragma endregion

    // load textures (we now use a utility function to keep the code more organized)
    // -----------------------------------------------------------------------------
    unsigned int diffuseMap = loadTexture ("resources/textures/blackBoard.png");

    // shader configuration
    // --------------------
    lightingShader.use();
    lightingShader.setInt("material.diffuse", 0);

   
    AnimationController anim;

    //FPS Times
    double prevTime = 0.0;
    double crntTime = 0.0;
    double timeDiff;
    unsigned int counter = 0;

    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        
        // FPS counter in window title
        crntTime = glfwGetTime();
        timeDiff = crntTime - prevTime;
        counter++;
        if (timeDiff >= 1.0 / 30.0)
        {
            std::string FPS = std::to_string((1.0 / timeDiff) * counter);
            std::string ms = std::to_string((timeDiff / counter) * 1000);
            std::string newTitle = "GameEngine - " + FPS + "FPS / " + ms + "ms";
            glfwSetWindowTitle(window, newTitle.c_str());
            prevTime = crntTime;
            counter = 0;
        }


        anim.update(deltaTime);

        //        std::cout << deltaTime << "ms (" << 1.0f / deltaTime << " FPS)" << std::endl;

        //input
        processInput(window);

        //processing
        glClearColor(0.5f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//******* changes);

        //glBindTexture(GL_TEXTURE_2D, texture1);

        //Lighting
        // don't forget to use the corresponding shader program first (to set the uniform)
        lightingShader.use();
        lightingShader.setVec3("light.position", lightPos);
        lightingShader.setVec3("viewPos", camera.Position);


        // light properties
        lightingShader.setVec3("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
        lightingShader.setVec3("light.diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
        lightingShader.setVec3("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

        // material properties
        lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
        lightingShader.setFloat("material.shininess", 64.0f);
        
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

        // view/projection transformations
        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);

        // world transformation
        lightingShader.setMat4("model", model);

        // bind diffuse map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);


        #pragma region Chessboard
        ////=============================================
        ////cube 
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F,50.0F,0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale
        
        myShader.use();

        /*myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);*/

        lightingShader.setMat4("projection", projection);
        lightingShader.setMat4("view", view);
        
        lightingShader.setMat4("model", model);
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
        //model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
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

            //Animations here
            anim.applyBouncingAnimation(model, 0.1f, 0.5f);
            anim.applyBreathingAnimation(model, 0.5f, 1.0f);
            anim.applyRotationAnimation(model, 10.0f, glm::vec3(0.0f, 0.0f, 1.0f));

            
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

            //Animations here
            anim.applyRotationAnimation(model,10.0f, glm::vec3(0.0f,0.0f,1.0f));
            

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

            //Animations here
            anim.applyRotationAnimation(model, 10.0f, glm::vec3(0.0f, 0.0f, 2.0f));

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
            model = glm::scale(model, glm::vec3(0.7f,0.7f,0.5f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 10.0f, glm::vec3(0.0f, 0.0f, 2.0f));

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

            //Animations here
            anim.applyRotationAnimation(model, 15.0f, glm::vec3(0.0f, 0.0f, 1.0f));

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

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKnightPositions[i]); 
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 15.0f, glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
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
        ////=============================================
       ////cone Knight 2
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKnight2Positions[i]);
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
            model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
                glBindTexture(GL_TEXTURE_2D, textureT2);
            


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myKnightCone.Draw(myShader);
        }
#pragma endregion

        #pragma region Bishop
        ////=============================================
        ////cylinder Bishop
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
            model = glm::translate(model, cylinderBishopPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 7.0f, glm::vec3(0.0f, 0.0f, 1.0f));

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
            myBishopCylinder.Draw(myShader);
        }
        ////=============================================
       ////cone Bishop
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

            //Animations here
            anim.applyRotationAnimation(model, 7.0f, glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, glm::vec3(0.7f));
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
            myBishopCone.Draw(myShader);
        }
        ////=============================================
       ////cube Bishop
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
            model = glm::translate(model, cubeBishopPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 7.0f, glm::vec3(1.0f, 0.0f, 0.0f));

            model = glm::scale(model, glm::vec3(0.7f,0.2f,0.2f));


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

        #pragma region Queen
        ////=============================================
       ////cone Queen
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneQueenPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, glm::vec3(1.0f,1.0f,1.5f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 1)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }
        ////=============================================
       ////cone Queen2
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneQueen2Positions[i]);
            model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 100.0f, glm::vec3(0.0f, 0.0f, 1.0f));

            model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 1)
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
        ////=============================================
       ////cone King
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKingPositions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.5f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 1)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }
        ////=============================================
       ////cone King2
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, coneKing2Positions[i]);
            model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            float angle = 20.0f * i;
            model = glm::scale(model, glm::vec3(0.7f, 0.7f, 0.7f));
            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 1)
                glBindTexture(GL_TEXTURE_2D, texture1);
            else
            {
                glBindTexture(GL_TEXTURE_2D, textureT2);
            }


            myShader.setMat4("model", model);;

            //glDrawArrays(GL_TRIANGLES, 0, 36);
            myCone.Draw(myShader);
        }
        ////=============================================
       ////sphere King
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::translate(model, glm::vec3(0.0F, 50.0F, 0.0F));
        //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
        //model = glm::scale(model, glm::vec3(500.0f)); // Change Cube Scale

        myShader.use();

        myShader.setMat4("model", model);
        myShader.setMat4("view", view);
        myShader.setMat4("projection", projection);

        for (unsigned int i = 0; i < 2; i++)
        {
            // calculate the model matrix for each object and pass it to shader before drawing
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, sphereKingPositions[i]);
            model = glm::scale(model, glm::vec3(0.4f));
            float angle = 20.0f * i;

            //Animations here
            anim.applyRotationAnimation(model, 10.0f, glm::vec3(0.0f, 1.0f, 0.0f));
            anim.applyBouncingAnimation(model, 0.5f, 1.0f);

            //if (i % 3 == 0)  // every 3rd iteration (including the first) we set the angle using GLFW's time function.  ... Use this for as if (i % 2 ==0) for colour of cubes.
            //angle = glfwGetTime() * 25.0f; // Rotation of cubes overtime
            //model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            if (i < 1)
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

        #pragma region  lightingCube
        
        // also draw the lamp object
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
        lightCubeShader.setMat4("model", model);

        //glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.2f));
        lightCubeShader.setMat4("model", model);
        myCube.Draw(lightCubeShader);

#pragma endregion

        //import models
        importShader.use();

    #pragma region ModelCastle

        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        //importShader.setMat4("model", model);
        //castlemodel.Draw(castlemodel);

#pragma endregion

    #pragma region Modelbishop

        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(5.0f, 5.0f, 5.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        //importShader.setMat4("model", model);
        //bishopmodel.Draw(bishopmodel);

#pragma endregion

    #pragma region Modelknight

        //glm::mat4 model = glm::mat4(1.0f);
        //model = glm::translate(model, glm::vec3(10.0f, 10.0f, 10.0f)); // translate it down so it's at the center of the scene
        //model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        //importShader.setMat4("model", model);
        //knightmodel.Draw(knightmodel);


#pragma endregion

        #pragma region Skybox
        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default

#pragma endregion

        #pragma region TextRender
        //// Text
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);

        //RenderText(textShader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
        //RenderText(textShader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
#pragma endregion


        //math
        
        //frame buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &skyboxVBO);
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(false);
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            GLenum format;
            if (nrChannels == 1)
                format = GL_RED;
            else if (nrChannels == 3)
                format = GL_RGB;
            else if (nrChannels == 4)
                format = GL_RGBA;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

//// render line of text
//// -------------------
//void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
//{
//    // activate corresponding render state	
//    shader.use();
//    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
//    glActiveTexture(GL_TEXTURE0);
//    glBindVertexArray(VAO);
//
//    // iterate through all characters
//    std::string::const_iterator c;
//    for (c = text.begin(); c != text.end(); c++)
//    {
//        Character ch = Characters[*c];
//
//        float xpos = x + ch.Bearing.x * scale;
//        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;
//
//        float w = ch.Size.x * scale;
//        float h = ch.Size.y * scale;
//        // update VBO for each character
//        float vertices[6][4] = {
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos,     ypos,       0.0f, 1.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//
//            { xpos,     ypos + h,   0.0f, 0.0f },
//            { xpos + w, ypos,       1.0f, 1.0f },
//            { xpos + w, ypos + h,   1.0f, 0.0f }
//        };
//        // render glyph texture over quad
//        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
//        // update content of VBO memory
//        glBindBuffer(GL_ARRAY_BUFFER, VBO);
//        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData
//
//        glBindBuffer(GL_ARRAY_BUFFER, 0);
//        // render quad
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
//        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
//    }
//    glBindVertexArray(0);
//    glBindTexture(GL_TEXTURE_2D, 0);
//}
