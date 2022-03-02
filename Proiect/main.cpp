#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp" //core glm functionality
#include "glm/gtc/matrix_transform.hpp" //glm extension for generating common transformation matrices
#include "glm/gtc/matrix_inverse.hpp" //glm extension for computing inverse matrices
#include "glm/gtc/type_ptr.hpp" //glm extension for accessing the internal data structure of glm types

#include "Window.h"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Model3D.hpp"
#include "SkyBox.hpp"

#include <iostream>

// window
gps::Window myWindow;

// matrices
glm::mat4 model;
glm::mat4 view;
glm::mat4 projection;
glm::mat3 normalMatrix;
glm::mat4 lightRotation;

// light parameters
glm::vec3 lightDir;
glm::vec3 lightColor = glm::vec3(0.1f, 0.1f, 0.1f);
bool light = false;

// shader uniform locations
GLuint modelLoc;
GLuint viewLoc;
GLuint projectionLoc;
GLuint normalMatrixLoc;
GLuint lightDirLoc;
GLuint lightColorLoc;
GLuint p_lightsLoc;
GLuint fogDensityLoc;

// camera
gps::Camera myCamera(
    glm::vec3(80.6804f, 21.1539f, 20.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));

GLuint cameraPositionLoc;
GLfloat cameraSpeed = 0.5f;
GLfloat angle;


float lastX = 320;
float lastY = 240;

float yaw = -180;
float pitch = -30;

bool firstMouse = true;

GLboolean pressedKeys[1024];

// models
gps::Model3D Scena;
gps::Model3D Rata;
gps::Model3D Ratusca1;
gps::Model3D Ratusca2;
gps::Model3D Ratusca3;
gps::Model3D Ratusca4;
gps::Model3D Ratusca5;
gps::Model3D Ratusca6;

//Skybox
gps::SkyBox mySkyBox;
std::vector<const GLchar*> faces;

//Shadows
GLuint shadowMapFBO;
GLuint depthMapTexture;
bool showDepthMap;
const unsigned int SHADOW_WIDTH = 2048;
const unsigned int SHADOW_HEIGHT = 2048;

//lights
glm::vec3 p_lights[] = { glm::vec3(24.0028f, 3.21297f, 37.2272f), glm::vec3(22.4004f, 2.10168f, 43.2224f), glm::vec3(40.1134f, 3.56777f, 6.3404f), glm::vec3(18.6961f, 3.56777f, 6.3404f),
                    glm::vec3(-20.0929f, 3.56777f, 4.77834f), glm::vec3(-31.8584f, 3.56777f, 5.09765f), glm::vec3(-0.194596f, 23.3889f, 21.3202f) };

// shaders
gps::Shader myBasicShader;
gps::Shader skyboxShader;
gps::Shader depthMapShader;
gps:: Shader screenQuadShader;

//animation
bool time_to_animate = false;
bool in_animation = false;
float time_of_animation = 0.0f;
float viteza_miscarii = 500.0f; //20.of
float animation_directionz = -0.6f;
float animation_directionx = 0.0f;
bool la_explorat = false;
bool prima_intoarcere = false;
bool catre_a_doua_intoarcere = false;
bool a_doua_intoarcere = false;
bool catre_casa = false;
bool a_treia_intoarcere = false;
bool catre_a_patra_intoarcere = false;
bool a_patra_intoarcere = false;

double timp_trecut;
float rotationangle = 0.0f;

//presentation animation
bool presenting = false;
bool first_m = false;
bool second_m = false;
bool third_m = false;

//fog
glm::vec3 fogDensity = glm::vec3(0.009f, 0.009f, 0.009f);

void animation(double timed) {
    if (time_to_animate) {
        if (!in_animation) {
            in_animation = true;
            la_explorat = true;
        }
        else {
            if (la_explorat) {
                if (animation_directionz <= -18.0f) {
                    la_explorat = false;
                    prima_intoarcere = true;
                    animation_directionx = 2.0f;
                }
                animation_directionz -= viteza_miscarii * timed;
                model = glm::translate(model, glm::vec3(0.0f, 0.0f, animation_directionz));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            }
            else if (prima_intoarcere) {
                //rotationangle += 1.0f;
                //model = glm::translate(model, glm::vec3(0.0f, 0.0f, -animation_directionz));
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                //model = glm::translate(model, glm::vec3(0.0f, 0.0f, animation_directionz));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                //if (rotationangle >= 45) {
                    prima_intoarcere = false;
                    catre_a_doua_intoarcere = true;
                //}
                

            }
            else if (catre_a_doua_intoarcere) {
                if (animation_directionx <= 0) {
                    catre_a_doua_intoarcere = false;
                    a_doua_intoarcere = true;
                }
                animation_directionx -= viteza_miscarii * timed;
                model = glm::translate(model, glm::vec3(animation_directionx, 0.0f, animation_directionz));
                //model = glm::rotate(model, glm::radians(rotationangle), glm::vec3(0, 1, 0));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            }
            else if (a_doua_intoarcere) {
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                a_doua_intoarcere = false;
                catre_casa = true;
            }
            else if (catre_casa) {
                if (animation_directionz >= -0.6f) {
                    catre_casa = false;
                    a_treia_intoarcere = true;
                }
                animation_directionz += viteza_miscarii * timed;
                model = glm::translate(model, glm::vec3(animation_directionx, 0.0f, animation_directionz));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));

            }
            else if (a_treia_intoarcere) {
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                a_treia_intoarcere = false;
                catre_a_patra_intoarcere = true;
            }
            else if (catre_a_patra_intoarcere) {
                if (animation_directionx >= 1.0f) {
                    catre_a_patra_intoarcere = false;
                    a_patra_intoarcere = true;
                }
                animation_directionx += viteza_miscarii * timed;
                model = glm::translate(model, glm::vec3(animation_directionx, 0.0f, animation_directionz));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            }
            else if (a_patra_intoarcere) {
                model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));
                glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
                a_patra_intoarcere = false;
                in_animation = false;
                animation_directionx = 0.0f;
                animation_directionz = -0.6f;
                rotationangle = 0.0f;
            }
        }

        Rata.Draw(myBasicShader);
        Ratusca1.Draw(myBasicShader);
        Ratusca2.Draw(myBasicShader);
        Ratusca3.Draw(myBasicShader);
        Ratusca4.Draw(myBasicShader);
        Ratusca5.Draw(myBasicShader);
        Ratusca6.Draw(myBasicShader);

    }
}

void presenting_animation(double timed) {
    
   if(first_m){
        if (myCamera.getCameraPosition().x <= -40.0f) {
            first_m = false;
            second_m = true;
        }
        if (pitch <= -5)
            pitch += 0.1f;
        myCamera.move(gps::MOVE_FORWARD, cameraSpeed * 0.5);
   }
   else if (second_m) {
       if (yaw >= -360)
           yaw -= 2.0f;
       else {
           presenting = false;
           second_m = false;
       }
   }

    view = myCamera.getViewMatrix();
    myBasicShader.useShaderProgram();
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
}

GLenum glCheckError_(const char *file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR) {
		std::string error;
		switch (errorCode) {
            case GL_INVALID_ENUM:
                error = "INVALID_ENUM";
                break;
            case GL_INVALID_VALUE:
                error = "INVALID_VALUE";
                break;
            case GL_INVALID_OPERATION:
                error = "INVALID_OPERATION";
                break;
            case GL_STACK_OVERFLOW:
                error = "STACK_OVERFLOW";
                break;
            case GL_STACK_UNDERFLOW:
                error = "STACK_UNDERFLOW";
                break;
            case GL_OUT_OF_MEMORY:
                error = "OUT_OF_MEMORY";
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                error = "INVALID_FRAMEBUFFER_OPERATION";
                break;
        }
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}
#define glCheckError() glCheckError_(__FILE__, __LINE__)

void windowResizeCallback(GLFWwindow* window, int width, int height) {
	fprintf(stdout, "Window resized! New width: %d , and height: %d\n", width, height);
}

void keyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

	if (key >= 0 && key < 1024) {
        if (action == GLFW_PRESS) {
            pressedKeys[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressedKeys[key] = false;
        }
    }
}

void initFaces()
{
    faces.push_back("skybox/right.png");
    faces.push_back("skybox/left.png");
    faces.push_back("skybox/top.png");
    faces.push_back("skybox/bottom.png");
    faces.push_back("skybox/back.png");
    faces.push_back("skybox/front.png");
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

}

void processMovement() {
	if (pressedKeys[GLFW_KEY_W]) {
		myCamera.move(gps::MOVE_FORWARD, cameraSpeed);
		//update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_S]) {
		myCamera.move(gps::MOVE_BACKWARD, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_A]) {
		myCamera.move(gps::MOVE_LEFT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

	if (pressedKeys[GLFW_KEY_D]) {
		myCamera.move(gps::MOVE_RIGHT, cameraSpeed);
        //update view matrix
        view = myCamera.getViewMatrix();
        myBasicShader.useShaderProgram();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
        // compute normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	}

    if (pressedKeys[GLFW_KEY_Q]) {
        angle -= 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    if (pressedKeys[GLFW_KEY_E]) {
        angle += 1.0f;
        // update model matrix for teapot
        model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0, 1, 0));
        // update normal matrix for teapot
        normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
    }

    //crestam intensitatea cetii
    if (pressedKeys[GLFW_KEY_I]) {
        fogDensity.x += 0.001;
        if (fogDensity.x < 0 || fogDensity.x > 1)
            fogDensity.x = 0.0f;
    }

    //scadem intensitatea cetii
    if (pressedKeys[GLFW_KEY_O]) {
        fogDensity.x -= 0.001;
        if (fogDensity.x < 0 || fogDensity.x > 1)
            fogDensity.x = 0.0f;
    }

    //pornmim/oprim animatia
    if (pressedKeys[GLFW_KEY_T]) {
        time_to_animate = true;
        //SoundEngine->play2D("audio/duck_sounds_curious_ducks_talking_7144026612371096135.mp3", true);
    }

    if (pressedKeys[GLFW_KEY_Y]) {
        time_to_animate = false;
    }

    // line view
    if (pressedKeys[GLFW_KEY_Z]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    // point view
    if (pressedKeys[GLFW_KEY_X]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    }

    // normal view
    if (pressedKeys[GLFW_KEY_C]) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    if (pressedKeys[GLFW_KEY_V]) {
        presenting = true;
        first_m = true;
        myCamera.setCameraPosition(glm::vec3(80.6804f, 21.1539f, 20.0f));

        yaw = -180;
        pitch = -20;
    }

    if (pressedKeys[GLFW_KEY_B]) {
        presenting = false;
        first_m = false;
        second_m = false;
        third_m = false;

        myCamera.setCameraPosition(glm::vec3(80.6804f, 21.1539f, 20.0f));

        yaw = -180;
        pitch = -20;
    }

    if (pressedKeys[GLFW_KEY_L]) {
        light = true;
    }

    if (pressedKeys[GLFW_KEY_K]) {
        light = false;
    }
}

void initOpenGLWindow() {
    myWindow.Create(1920, 1080, "OpenGL Project Core");
}

void setWindowCallbacks() {
	glfwSetWindowSizeCallback(myWindow.getWindow(), windowResizeCallback);
    glfwSetKeyCallback(myWindow.getWindow(), keyboardCallback);
    glfwSetCursorPosCallback(myWindow.getWindow(), mouseCallback);
    glfwSetInputMode(myWindow.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void initOpenGLState() {
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);
    glEnable(GL_FRAMEBUFFER_SRGB);
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glEnable(GL_CULL_FACE); // cull face
	glCullFace(GL_BACK); // cull back face
	glFrontFace(GL_CCW); // GL_CCW for counter clock-wise
}

void initModels() {
    Scena.LoadModel("models/Scena/ProiectPG.obj");
    Rata.LoadModel("models/Rata/Rata.obj");
    Ratusca1.LoadModel("models/Ratusca1/Ratusca1.obj");
    Ratusca2.LoadModel("models/Ratusca2/Ratusca2.obj");
    Ratusca3.LoadModel("models/Ratusca3/Ratusca3.obj");
    Ratusca4.LoadModel("models/Ratusca4/Ratusca4.obj");
    Ratusca5.LoadModel("models/Ratusca5/Ratusca5.obj");
    Ratusca6.LoadModel("models/Ratusca6/Ratusca6.obj");
}

void initShaders() {
	myBasicShader.loadShader("shaders/basic.vert", "shaders/basic.frag");
    screenQuadShader.loadShader("shaders/screenQuad.vert", "shaders/screenQuad.frag");
    screenQuadShader.useShaderProgram();
    depthMapShader.loadShader("shaders/depthMapShader.vert", "shaders/depthMapShader.frag");
    depthMapShader.useShaderProgram();
}

void initSkyBoxShader()
{
    mySkyBox.Load(faces);
    skyboxShader.loadShader("shaders/skyboxShader.vert", "shaders/skyboxShader.frag");
    skyboxShader.useShaderProgram();
    view = myCamera.getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "view"), 1, GL_FALSE,
        glm::value_ptr(view));

    projection = glm::perspective(glm::radians(45.0f), (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height, 0.1f, 1000.0f);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.shaderProgram, "projection"), 1, GL_FALSE,
        glm::value_ptr(projection));
}

void initUniforms() {
	myBasicShader.useShaderProgram();

    // create model matrix for teapot
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
	modelLoc = glGetUniformLocation(myBasicShader.shaderProgram, "model");

	// get view matrix for current camera
	view = myCamera.getViewMatrix();
	viewLoc = glGetUniformLocation(myBasicShader.shaderProgram, "view");
	// send view matrix to shader
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

    // compute normal matrix for teapot
    normalMatrix = glm::mat3(glm::inverseTranspose(view*model));
	normalMatrixLoc = glGetUniformLocation(myBasicShader.shaderProgram, "normalMatrix");

	// create projection matrix
	projection = glm::perspective(glm::radians(45.0f),
                               (float)myWindow.getWindowDimensions().width / (float)myWindow.getWindowDimensions().height,
                               0.1f, 150.0f);
	projectionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "projection");
	// send projection matrix to shader
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));	

	//set the light direction (direction towards the light)
	lightDir = glm::vec3(0.0f, 1.0f, 1.0f);
	lightDirLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightDir");
	// send light dir to shader
	glUniform3fv(lightDirLoc, 1, glm::value_ptr(lightDir));

    //set lights position
    p_lightsLoc = glGetUniformLocation(myBasicShader.shaderProgram, "p_lights");
    glUniform3fv(p_lightsLoc, 7, glm::value_ptr(p_lights[0]));

}

void initFBO() {
    //TODO - Create the FBO, the depth texture and attach the depth texture to the FBO
    glGenFramebuffers(1, &shadowMapFBO);
    glGenTextures(1, &depthMapTexture);
    glBindTexture(GL_TEXTURE_2D, depthMapTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapTexture, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 computeLightSpaceTrMatrix() {
    //TODO - Return the light-space transformation matrix
    glm::vec3 lightDirTr = glm::vec3(glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f)) * glm::vec4(lightDir, 1.0f));
    glm::mat4 lightView = glm::lookAt(lightDirTr, myCamera.getCameraTarget(), glm::vec3(0.0f, 1.0f, 0.0f));
    const GLfloat near_plane = -50.0f, far_plane = 300.0f;
    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, near_plane, far_plane);
    glm::mat4 lightSpaceTrMatrix = lightProjection * lightView;
    
    return lightSpaceTrMatrix;
}

void renderObjects(gps::Shader shader, bool depthPass) {
    // select active shader program
    shader.useShaderProgram();

    //send scene model matrix data to shader
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

    //send scene normal matrix data to shader
    if (!depthPass) {
        normalMatrix = glm::mat3(glm::inverseTranspose(view * model));
        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    }

    // draw objects
    Scena.Draw(shader);
    if (!time_to_animate) {
        Rata.Draw(myBasicShader);
        Ratusca1.Draw(myBasicShader);
        Ratusca2.Draw(myBasicShader);
        Ratusca3.Draw(myBasicShader);
        Ratusca4.Draw(myBasicShader);
        Ratusca5.Draw(myBasicShader);
        Ratusca6.Draw(myBasicShader);
    }
    //lightCube.Draw(shader);
}


void renderScene() {
    depthMapShader.useShaderProgram();
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(depthMapShader.shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    renderObjects(depthMapShader, true);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    if (showDepthMap) {
        //bind the depth map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(screenQuadShader.shaderProgram, "depthMap"), 0);

        glDisable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH_TEST);
    }
    else {
        
        //set light color
        if (light) {
            lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
        }
        else {
            lightColor = glm::vec3(0.01f, 0.01f, 0.01f);
        }
        lightColorLoc = glGetUniformLocation(myBasicShader.shaderProgram, "lightColor");

        // send light color to shader
        glUniform3fv(lightColorLoc, 1, glm::value_ptr(lightColor));

        //final scene rendering pass(with shadows)

        glViewport(0, 0, myWindow.getWindowDimensions().width, myWindow.getWindowDimensions().height);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myBasicShader.useShaderProgram();

        //render the scene
        view = myCamera.getViewMatrix();
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        lightRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniform3fv(lightDirLoc, 1, glm::value_ptr(glm::inverseTranspose(glm::mat3(view * lightRotation)) * lightDir));

        // bind the shadow map
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMapTexture);
        glUniform1i(glGetUniformLocation(myBasicShader.shaderProgram, "shadowMap"), 3);

        glUniformMatrix4fv(glGetUniformLocation(myBasicShader.shaderProgram, "lightSpaceTrMatrix"), 1, GL_FALSE, glm::value_ptr(computeLightSpaceTrMatrix()));

        //set camera position
        cameraPositionLoc = glGetUniformLocation(myBasicShader.shaderProgram, "cameraPosition");
        glUniform3fv(cameraPositionLoc, 1, glm::value_ptr(myCamera.getCameraPosition()));

        //set the fog density location
        fogDensityLoc = glGetUniformLocation(myBasicShader.shaderProgram, "fogDensity");
        glUniform3fv(fogDensityLoc, 1, glm::value_ptr(fogDensity));

        //render the objects

        renderObjects(myBasicShader, false);

        double timp_curent = glfwGetTime();
        animation(timp_curent - timp_trecut);
        if (presenting) {
            presenting_animation(timp_curent - timp_trecut);
        }
        //lightCube.Draw(lightShader);
        mySkyBox.Draw(skyboxShader, view, projection);
        myCamera.rotate(pitch, yaw);
    }
}

void cleanup() {
    myWindow.Delete();
    //cleanup code for your own data
}

int main(int argc, const char * argv[]) {

    try {
        initOpenGLWindow();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    initFaces();
    initSkyBoxShader();
    initOpenGLState();
	initModels();
	initShaders();
	initUniforms();
    initFBO();
    setWindowCallbacks();

	glCheckError();
	// application loop
	while (!glfwWindowShouldClose(myWindow.getWindow())) {
        processMovement();
        timp_trecut = glfwGetTime();
        renderScene();
		glfwPollEvents();
		glfwSwapBuffers(myWindow.getWindow());

		glCheckError();
	}

	cleanup();

    return EXIT_SUCCESS;
}
