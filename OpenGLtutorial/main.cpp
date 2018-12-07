#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>

//GLEW
//===============
#define GLEW_STATIC
#include <GL/glew.h>

//GLFW
//============
#include <GLFW/glfw3.h>

//Soil
//=============
#include "SOIL2/SOIL2.h"

//GLM
//============
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//Ohter includes
//==================
#include "Shader.h"
#include "Camera.h"

//global variables
//===================

//window size
GLint WIDTH = 1000, HEIGHT = 900;

//size of conways world
const int squares = 51;
bool ConwaysWorld[squares][squares];
//used for placing squares on screen
glm::vec2 squarePositions[squares][squares];

//global methods
//=================

//Checks for any inputs that happen in the game loop
void processInput(GLFWwindow *window);


//Helper methods
//===================

//Sets conways world with all false values
void InitializeConwaysWorld();
void GeneratePositionsAndConwaysWorld();
int GetColRow(int);

//namespaces
//====================
using namespace std;


//Main program
int main()
{
    //initilize glfw
    glfwInit();
    
    //Window settings
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_DECORATED, GL_TRUE);
    
    //create window
    GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, "wfewgfergewr", nullptr, nullptr);
    
    //screen width and height
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    if(nullptr == window)
    {
        std::cout << "Failed to create glfw window" << endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    
    if(GLEW_OK != glewInit())
    {
        cout << "Failed to intilize GLEW" << endl;
        return -1;
    }
    
    //define the viewport dimension
    glViewport(0, 0, screenWidth, screenHeight);
    
    //Build and compile shader program
    Shader MyShader("res/shaders/lighting.vs", "res/shaders/lighting.frag");
    
    
    float vertices[] = {
        //positions             //Color
        -0.5f, -0.5f, 0.0f,    1.0f, 1.0f, 1.0f,    //bottom left
        0.5f, -0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    //bottom right
        0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 1.0f,    //top right
        -0.5f, 0.5f, 0.0f,      1.0f, 1.0f, 1.0f    //top left
    };
    
    //Sets the world with false values
    InitializeConwaysWorld();
    
    //Generate random cells in conways world and set positions for the squares
    GeneratePositionsAndConwaysWorld();
    
    //EBO indices
    unsigned int indices[] = {  // note that we start from 0!
        0, 1, 2,    //first triangle
        3, 0, 2,     //2nd triangle
    };
    
    //create the vertex buffer, array and element buffer objects
    GLuint VBO, VAO, EBO;
    glGenVertexArrays( 1, &VAO);
    glGenBuffers( 1, &VBO );
    glGenBuffers(1, &EBO);
    
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
    glBindVertexArray( VAO );
    
    //bind buffer object and load the vertices to VAO
    glBindBuffer( GL_ARRAY_BUFFER, VBO );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );
    
    //bind the EBO and the data to the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * ) 0 );
    glEnableVertexAttribArray( 0 );
    // Color attribute
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
    glEnableVertexAttribArray( 1 );
    //Texture attribute
//    glVertexAttribPointer( 2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof( GLfloat ), ( GLvoid * )( 3 * sizeof( GLfloat ) ) );
//    glEnableVertexAttribArray( 2 );
    
    //Unbind VAO
    glBindVertexArray( 0 );
    
    //Use the shader program to set the view and perspective
    MyShader.Use();
    
    //initilize view and perspective
    glm::mat4 view(1);
    glm::mat4 perspective(1);
    
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -((float)squares+squares*2.0f)));
    perspective = glm::perspective(glm::radians(45.0f), (float)screenWidth/(float)screenHeight, 0.1f, (float)squares+squares*2);
    
    MyShader.setMatrix4fv("perspective", perspective);
    MyShader.setMatrix4fv("view", view);
    
    glEnable(GL_DEPTH_TEST);
    
    // Game loop
    while ( !glfwWindowShouldClose( window ) )
    {
        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents( );
        
        // Render
        // Clear the colorbuffer
        glClearColor( 0.2f, 0.2f, 0.2f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glBindVertexArray( VAO );
        
        for(int row = 1; row < squares-1; row++)
        {
            for(int col = 1; col < squares-1; col++)
            {
                glm::mat4 model(1);
                model = glm::translate(model, glm::vec3(squarePositions[row][col], 0.2f));
//                model = glm::rotate(model, (float)glfwGetTime()*-2.0f, glm::vec3(0.0f,0.0f,1.0f));
                MyShader.setMatrix4fv("model", model);
                
                glm::vec4 myColor(1.0f);

                int life = 0;
                life += ConwaysWorld[col][GetColRow(row-1)] ? 1 : 0;
                life += ConwaysWorld[col][GetColRow(row+1)] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col-1)][GetColRow(row-1)] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col-1)][GetColRow(row+1)] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col+1)][GetColRow(row+1)] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col+1)][GetColRow(row-1)] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col+1)][row] ? 1 : 0;
                life += ConwaysWorld[GetColRow(col-1)][row] ? 1 : 0;
                
                if(ConwaysWorld[col][row])
                {
                    if(life < 2 || life > 3)
                        ConwaysWorld[col][row] = false;
                }
                else
                {
                    ConwaysWorld[col][row] = life == 3 ? true : false;
                }
                
                if(ConwaysWorld[row][col])
                    myColor = ConwaysWorld[row][col] ? glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) : glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
                
                MyShader.setVec4("color", myColor);
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            }
        }
        
        // Swap the screen buffers
        glfwSwapBuffers( window );
        processInput(window);
    }
    
    // Properly de-allocate all resources once they've outlived their purpose
    glDeleteVertexArrays( 1, &VAO );
    glDeleteBuffers( 1, &VBO );
    glDeleteBuffers( 1, &EBO );
   
    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate( );
    
    return 0;
}

//Global Methods
//===================

//Checks for any inputs that happen in the game loop
void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ENTER))
        glfwSetWindowShouldClose(window, GL_TRUE);
}


//Helper Methods
//=====================

//Sets the world with false values
void InitializeConwaysWorld()
{
    for(int i = 0; i < squares; i++)
    {
        for(int j = 0; j < squares; j++)
        {
            ConwaysWorld[i][j] = false;
        }
    }
}

//Generate random cells in conways world and set positions for the squares
void GeneratePositionsAndConwaysWorld()
{
    //generate true random numbers
    srand((unsigned int)time(NULL));
    
    //determines space between squares in x direction
    float xPos = -(float)squares/2.0f;
    
    //Loop through matrix and set all positions for the squares and random values for conways world
    for(int row = 0; row < squares; row++)
    {
        float yPos = -(float)squares/2.0f;
        for(int col = 0; col < squares; col++)
        {
            if(col % 3 == 0 && row % 5 == 0)
            {
//                if(rand() % 2 == 0)
//                    ConwaysWorld[row][col] = false;
//                else
                    ConwaysWorld[row][col] = true;
            }
            squarePositions[row][col] = glm::vec2(xPos, yPos);
            yPos += 1.0f;
        }
        xPos += 1.0f;
    }
}

int GetColRow(int colRow)
{
    return colRow >= squares ? 0 : colRow < 0 ? squares-1 : colRow;
}




