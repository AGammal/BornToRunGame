// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>//rotation
#include <glm/gtx/norm.hpp>//rotation

using namespace glm;
using namespace std;

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>


const int MAX = 100;

// Read our .obj file
vector<vec3> vertices[MAX], normals[MAX];
vector<vec2> uvs[MAX];
GLuint Texture[MAX], vertexbuffer[MAX], normalbuffer[MAX], uvbuffer[MAX];
GLuint TextureID;

// Store Models names in array of strings to use in loading Models and textures
const int NumOfModels = 13;
string ModelsName[NumOfModels] = {"Trump", "Road", "Road", "Road", "ClearNight", "Snowman", "Presents"
                                  , "SnowEnv", "Road", "Road", "Road", "SnowEnv", "SnowEnv"};

// handles for our buffers
GLuint vertexPosition_modelspaceID ,vertexUVID ,vertexNormal_modelspaceID;

// handles for our "MVP" uniform
GLuint MatrixID, ViewMatrixID, ModelMatrixID;

// MVP View and Projection matrices
mat4 ProjectionMatrix, ViewMatrix;

// Model Translation, Rotation and Scaling Matrices
mat4 ModelMatrix[MAX];
mat4 RotationMatrix[MAX], TranslationMatrix[MAX], ScalingMatrix[MAX];

void Initialize()
{
    for(int i = 0; i < NumOfModels; i++)
    {
        RotationMatrix[i] = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians
        TranslationMatrix[i] = translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
        ScalingMatrix[i] = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
        ModelMatrix[i] = mat4(1.0);
    }
    // Trump
    TranslationMatrix[0] = translate(mat4(), vec3(3.0f, 0.0f, -2.0f));
    ModelMatrix[0] = TranslationMatrix[0];

    // Mid Road
    ScalingMatrix[1] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[1] = translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
    ModelMatrix[1] = TranslationMatrix[1]*ScalingMatrix[1];

    // Right Road
    ScalingMatrix[2] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[2] = translate(mat4(), vec3(-3.0f, 0.0f, 0.0f));
    ModelMatrix[2] = TranslationMatrix[2]* RotationMatrix[2]*ScalingMatrix[2];

    // Left Road
    ScalingMatrix[3] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[3] = translate(mat4(), vec3(3.0f, 0.0f, 0.0f));
    ModelMatrix[3] = TranslationMatrix[3]* RotationMatrix[3]*ScalingMatrix[3];

    // Front Mid Road
    ScalingMatrix[8] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[8] = translate(mat4(), vec3(0.0f, 0.0f, 13.0f));
    ModelMatrix[8] = TranslationMatrix[8]*ScalingMatrix[8];

    // Front Right Road
    ScalingMatrix[9] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[9] = translate(mat4(), vec3(-3.0f, 0.0f, 13.0f));
    ModelMatrix[9] = TranslationMatrix[9]* RotationMatrix[9]*ScalingMatrix[9];

    // Front Left Road
    ScalingMatrix[10] = scale(mat4(), vec3(1.0f, 1.0f, 4.0f));
    TranslationMatrix[10] = translate(mat4(), vec3(3.0f, 0.0f, 13.0f));
    ModelMatrix[10] = TranslationMatrix[10]* RotationMatrix[10]*ScalingMatrix[10];

    // Snowman
    ScalingMatrix[5] = scale(mat4(), vec3(1.0f, 1.0f, 1.4f));
    RotationMatrix[5] = eulerAngleYXZ(0.0f, -1.57f, -1.4f);//yaw, pitch and roll. Measured in radians;;
    TranslationMatrix[5] = translate(mat4(), vec3(0.0f, 0.0f, 0.0f));
    ModelMatrix[5] = TranslationMatrix[5]* RotationMatrix[5]*ScalingMatrix[5];

    // Presents
    RotationMatrix[6] = eulerAngleYXZ(0.0f, -1.57f, 0.0f);//yaw, pitch and roll. Measured in radians;
    TranslationMatrix[6] = translate(mat4(), vec3(-3.0f, 0.5f, 0.0f));
    ModelMatrix[6] = TranslationMatrix[6]* RotationMatrix[6]*ScalingMatrix[6];

    // Skybox
    RotationMatrix[4] = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians
    //TranslationMatrix[4] = translate(mat4(), vec3(0.0f, -10.0f, 0.0f));
    ScalingMatrix[4] = scale(mat4(), vec3(90.0f, 90.0f, 90.0f));
    ModelMatrix[4] = TranslationMatrix[4]*RotationMatrix[4]*ScalingMatrix[4];

    // Left SnowGround
    ScalingMatrix[7] = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
    RotationMatrix[7] = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians;;
    TranslationMatrix[7] = translate(mat4(), vec3(36.0f, -2.0f, 0.0f));
    ModelMatrix[7] = TranslationMatrix[7]* RotationMatrix[7]*ScalingMatrix[7];

    // Right SnowGround
    ScalingMatrix[11] = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
    RotationMatrix[11] = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians;;
    TranslationMatrix[11] = translate(mat4(), vec3(-36.0f, -2.0f, 0.0f));
    ModelMatrix[11] = TranslationMatrix[11]* RotationMatrix[11]*ScalingMatrix[11];

    // Far SnowGround
    ScalingMatrix[12] = scale(mat4(), vec3(1.0f, 1.0f, 1.0f));
    RotationMatrix[12] = eulerAngleYXZ(0.0f, 0.0f, 0.0f);//yaw, pitch and roll. Measured in radians;;
    TranslationMatrix[12] = translate(mat4(), vec3(0.0f, -2.0f, 60.0f));
    ModelMatrix[12] = TranslationMatrix[12]* RotationMatrix[12]*ScalingMatrix[12];
}

void LoadTexturesAndModels()
{
    for(int i = 0; i < NumOfModels; i++)
    {
        // Load the texture
        string modelnamebmp = ModelsName[i]+".bmp";
        string modelnameobj = ModelsName[i]+".obj";
        Texture[i] = loadBMP_custom(modelnamebmp.c_str());

        // Read our .obj file
        bool res = loadOBJ(modelnameobj.c_str(), vertices[i], uvs[i], normals[i]);

        // Load it into a VBO
        // GLuint vertexbuffer;
        glGenBuffers(1, &vertexbuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, vertices[i].size() * sizeof(vec3), &vertices[i][0], GL_STATIC_DRAW);

        // GLuint uvbuffer;
        glGenBuffers(1, &uvbuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, uvs[i].size() * sizeof(vec2), &uvs[i][0], GL_STATIC_DRAW);

        // GLuint normalbuffer;
        glGenBuffers(1, &normalbuffer[i]);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
        glBufferData(GL_ARRAY_BUFFER, normals[i].size() * sizeof(vec3), &normals[i][0], GL_STATIC_DRAW);
    }
}



void LoadTexturesOnModels()
{
    for(int i = 0; i < NumOfModels; i ++)
    {
        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, Texture[i]);
        // Set our "myTextureSampler" sampler to user Texture Unit 0
        glUniform1i(TextureID, 0);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(vertexPosition_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[i]);
        glVertexAttribPointer(
                vertexPosition_modelspaceID,  // The attribute we want to configure
                3,                            // size
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                0,                            // stride
                (void*)0                      // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(vertexUVID);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer[i]);
        glVertexAttribPointer(
                vertexUVID,                   // The attribute we want to configure
                2,                            // size : U+V => 2
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                0,                            // stride
                (void*)0                      // array buffer offset
        );

        // 3rd attribute buffer : normals
        glEnableVertexAttribArray(vertexNormal_modelspaceID);
        glBindBuffer(GL_ARRAY_BUFFER, normalbuffer[i]);
        glVertexAttribPointer(
                vertexNormal_modelspaceID,    // The attribute we want to configure
                3,                            // size
                GL_FLOAT,                     // type
                GL_FALSE,                     // normalized?
                0,                            // stride
                (void*)0                      // array buffer offset
        );

        // Compute MVP matrix for each model
        mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix[i];

        // Send our transformation to the currently bound shader,
        // in the "MVP" uniform
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[i][0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

        // Draw the triangles !
        glDrawArrays(GL_TRIANGLES, 0, vertices[i].size() );
    }
}

int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}

	glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 2);
	glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 1);

	// Open a window and create its OpenGL context
	if( !glfwOpenWindow( 1024, 768, 0,0,0,0, 32,0, GLFW_WINDOW ) )
	{
		fprintf( stderr, "Failed to open GLFW window.\n" );
		glfwTerminate();
		return -1;
	}
        // Initialize Models Matrices
        Initialize();

	// Initialize GLEW
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

        glfwSetWindowTitle( "Game" );

	// Ensure we can capture the escape key being pressed below
	glfwEnable( GLFW_STICKY_KEYS );
	glfwSetMousePos(1024/2, 768/2);

	// Dark blue background
        glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	// Cull triangles which normal is not towards the camera
	glEnable(GL_CULL_FACE);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "StandardShading.vertexshader", "StandardShading.fragmentshader" );

	// Get a handle for our "MVP" uniform
        MatrixID = glGetUniformLocation(programID, "MVP");
        ViewMatrixID = glGetUniformLocation(programID, "V");
        ModelMatrixID = glGetUniformLocation(programID, "M");

	// Get a handle for our buffers
        vertexPosition_modelspaceID = glGetAttribLocation(programID, "vertexPosition_modelspace");
        vertexUVID = glGetAttribLocation(programID, "vertexUV");
        vertexNormal_modelspaceID = glGetAttribLocation(programID, "vertexNormal_modelspace");

	// Load the texture
        LoadTexturesAndModels();

	// Get a handle for our "myTextureSampler" uniform
        TextureID  = glGetUniformLocation(programID, "myTextureSampler");

        // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
        ProjectionMatrix = perspective(90.0f, 4.0f / 3.0f, 0.1f, 100.0f);
        // Camera matrix
        ViewMatrix = lookAt(
                                           vec3(0,2,-5),
                                           vec3(0,0,0), // and looks at the origin
                                           vec3(0,1,0)  // Head is up (set to 0,-1,0 to look upside-down)
        );

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
        GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
        GLuint LightPower = glGetUniformLocation(programID, "LightPower");
        GLuint RID = glGetUniformLocation(programID, "R");
        GLuint GID = glGetUniformLocation(programID, "G");
        GLuint BID = glGetUniformLocation(programID, "B");
        float LightPowerID = 900.0f;
        float R = 0.3f;
        float G = 0.3f;
        float B = 0.3f;
        do{


		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                if (glfwGetKey( 'W' ) == GLFW_PRESS){
                        LightPowerID += 5.0f;
                }
                if (glfwGetKey( 'E' ) == GLFW_PRESS){
                        LightPowerID -= 5.0f;
                }
                if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS){
                        R +=0.1f;
                        if(R > 1.0f)R = 1.0f;
                }
                if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS){
                        R -=0.1f;
                        if(R < 0.0f)R = 0.0f;
                }
                if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS){
                        G +=0.1f;
                        if(G > 1.0f)G = 1.0f;
                }
                if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS){
                        G-= 0.1f;
                        if(G < 0.0f)G = 0.0f;
                }

                glm::vec3 lightPos = glm::vec3(0,40,10);
                glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
                glUniform1f(LightPower, LightPowerID);
                glUniform1f(RID, R);
                glUniform1f(GID, G);
                glUniform1f(BID, B);
		// Use our shader
		glUseProgram(programID);

                LoadTexturesOnModels();

		glDisableVertexAttribArray(vertexPosition_modelspaceID);
		glDisableVertexAttribArray(vertexUVID);
		glDisableVertexAttribArray(vertexNormal_modelspaceID);

		// Swap buffers
		glfwSwapBuffers();

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey( GLFW_KEY_ESC ) != GLFW_PRESS &&
		   glfwGetWindowParam( GLFW_OPENED ) );

	// Cleanup VBO and shader
        for(int i = 0; i < NumOfModels; i ++)
        {
            glDeleteBuffers(1, &vertexbuffer[i]);
            glDeleteBuffers(1, &uvbuffer[i]);
            glDeleteBuffers(1, &normalbuffer[i]);
            glDeleteTextures(1, &Texture[i]);
        }

        glDeleteProgram(programID);
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

