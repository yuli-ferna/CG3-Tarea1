#define NVBOS 5
#define PI 3.1416
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtx\euler_angles.hpp> // glm::yawPitchRoll

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <stb_image.h>
#include "userInterface.h"
#include "model.h"
#include "camera.h"

#include "Shader.h"
#include <vector>
#include <map>

#define px(x) x  
using namespace std;
// Window current width
unsigned int windowWidth = 800;
// Window current height
unsigned int windowHeight = 600;
// Window title
const char* windowTitle = "Yuliana Fernandez";
// Window pointer
GLFWwindow* window;
// Shader object
Shader* shader, * shaderFBO;

int modeImg = 0;
int modeImgAnt = 0;
// 0 = Normal
// 1 = Negative
int nImg = 0;
std::vector<float> kernel;
float sigma = 1.4;
//Textures
unsigned int textureID;
unsigned int textureID1, textureActual, texture, textMat;
unsigned int cubemapTexture;
std::vector<unsigned int> textures;
std::vector<unsigned int> images;

unsigned int planeVAO, planeVBO;

//MVP Matrix
glm::mat4 Model;
glm::mat4 View;
glm::mat4 Proj;

//Camera
camera* Camara = new camera();
float speedMouse = Camara->getSpeedMouse();
float currentTime, deltaTime;
float lastTime = glfwGetTime();


//Models
model* object;
std::vector< model* > modelsObj;
//tweakBar

userInterface* Interface;
userData dataTweak;
//Framebuffer
//Transformar al espacio de luz
float near_plane, far_plane;
unsigned int textureFBO;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;



float LoG(float x, float y, float sigma)
{
	float sigma4 = -(1 / (PI * sigma * sigma * sigma * sigma));
	float sigma2 = -((x * x + y * y) / (2 * sigma * sigma));
	float expon = exp(sigma2);
	return sigma4 * (1 - sigma2) * expon;
}
void resetKernet()
{
	kernel.clear();
	kernel.assign(49, 0.0f);
}
void LoGKernel(float x, float y, float sigma)
{
	//init
	resetKernet();
	int minX, maxX;
	int minY, maxY;
	int mayor = x < y ? y : x;

	minX = (-x / 2);
	minX =minX + (int(x) % 2 == 0 ? 1 : 0);
	maxX = (x / 2);

	minY = (-y / 2);
	minY = minY + (int(y) % 2 == 0 ? 1 : 0);
	maxY = (y / 2);

	//calculate
	int k = 0;
	for (int i = maxX; i >= minX; i--)
	{
		for (int j = maxY; j >= minY; j--)
		{
			kernel[k] = LoG(i, j, sigma);
			cout << kernel[k] << ' ';
		
			k++;
		}
	}
	cout << endl;
}	


/* *
 * Handles the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void resize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeight = height;
	// Sets the OpenGL viewport size and position
	glViewport(0, 0, windowWidth, windowHeight);
	Interface->reshape();

}

/**
 * Calback the window resize
 * @param{GLFWwindow} window pointer
 * @param{int} new width of the window
 * @param{int} new height of the window
 * */
void onResizeWindow(GLFWwindow* window, int width, int height) {
	TwWindowSize(width, height);
}

/**
 * Calback key press
 * */
void onKeyPress(GLFWwindow* window, int key, int scancode, int action, int mods) {
	/*if (action == GLFW_PRESS)
	{
		TwKeyPressed(key, TW_KMOD_NONE);
		return;
	}*/
	//Important!
	if (TwEventKeyGLFW(key, action))
		return;

	if (action == GLFW_PRESS)
	{
		glm::vec2 k = Interface->kernel;
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_LEFT:
			nImg--;
			if (nImg < 0)
			{
				nImg = images.size() - 1;
			}
			textureActual = images[nImg];
			break;
		case GLFW_KEY_RIGHT:
			nImg++;
			if (nImg >= images.size())
			{
				nImg = 0;
			}
			textureActual = images[nImg];
			break;
		case GLFW_KEY_1:
			modeImg = 1;
			Interface->setFilter(1);
			modeImgAnt = modeImg;

			break;
		case GLFW_KEY_0:
			modeImg = 0;
			Interface->setFilter(0);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_2:
			modeImg = 2;
			Interface->setFilter(2);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_3:
			modeImg = 3;
			Interface->setFilter(3);
			modeImgAnt = modeImg;

			break;
		case GLFW_KEY_4:
			modeImg = 4;
			//sobelKernel(k.x, k.y);
			Interface->setFilter(4);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_5:
			modeImg = 5;
			//prewittKernel(k.x, k.y);
			Interface->setFilter(5);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_6:
			modeImg = 6;
			//robertsKernel(k.x, k.y);
			Interface->setFilter(6);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_7:
			modeImg = 7;
			//promKernel(k.x, k.y);
			Interface->setFilter(7);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_8:
			modeImg = 8;
			//medianaKernel(k.x, k.y);
			Interface->setFilter(8);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_9:
			modeImg = 9;
			LoGKernel(k.x, k.y, sigma);
			Interface->setFilter(9);
			modeImgAnt = modeImg;
			break;
		case GLFW_KEY_C:
			//Changes camera mode
			if (!Camara->getCameraMode())
			{
				//cameraMode = true;
				//Interface->show();

			}
			else {
				//cameraMode = false;
				//Interface->hide();

			}
			Camara->changeCameraMode();
			//cameraMode = !cameraMode;
			break;

		}
	}

}

void onMouseButton(GLFWwindow* window, int button, int action, int mods)
{
	auto a = action == GLFW_PRESS ? TW_MOUSE_PRESSED : TW_MOUSE_RELEASED;
	auto b = TW_MOUSE_LEFT;

	TwMouseButton(a, b);
}

void onMouseMotion(GLFWwindow* window, double xpos, double ypos)
{
	TwMouseMotion(px(static_cast<int>(xpos)), px(static_cast<int>(ypos)));
	if (Camara->getCameraMode()) {

		glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
		GLfloat xoffset = ((windowWidth / 2.0) - xpos) * speedMouse * deltaTime;
		GLfloat yoffset = ((windowHeight / 2.0) - ypos) * speedMouse * deltaTime;
		Camara->updateInputMouse(xoffset, yoffset);

		/*yaw += xoffset;
		pitch += yoffset;
		std::cout << "main:\n" << yaw << ' ' << pitch << std::endl;*/
	}
}
void onCharacter(GLFWwindow* window, unsigned int codepoint) {
	TwKeyPressed(codepoint, TW_KMOD_NONE);

}

void initUserInterfaceValues()
{
	dataTweak.hist = false;
	dataTweak.info = false;
	dataTweak.kernel = glm::ivec2(3);
	dataTweak.type = 0;
	Interface->setData(dataTweak);
	Interface->setFilter(dataTweak.type);

}
/**
 * initialize the user interface
 * @returns{bool} true if everything goes ok
* */
bool initUserInterface()
{
	if (!TwInit(TW_OPENGL_CORE, NULL))
		return false;

	Interface = new userInterface();
	TwWindowSize(windowHeight, windowHeight);
	//initUserInterfaceValues();
	return true;
}
/**
 * Initialize the glfw library
 * @returns{bool} true if everything goes ok
 * */
bool initWindow()
{
	// Initialize glfw
	glfwInit();
	// Sets the Opegl context to Opengl 3.0
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	// Creates the window
	window = glfwCreateWindow(windowWidth, windowHeight, windowTitle, NULL, NULL);

	// The window couldn't be created
	if (!window)
	{
		std::cout << "Failed to create the glfw window" << std::endl;
		glfwTerminate(); // Stops the glfw program
		return false;
	}

	// Creates the glfwContext, this has to be made before calling initGlad()
	glfwMakeContextCurrent(window);

	// Window resize callback
	glfwSetFramebufferSizeCallback(window, resize);

	glfwSetCursorPosCallback(window, onMouseMotion);
	glfwSetMouseButtonCallback(window, onMouseButton);
	glfwSetKeyCallback(window, onKeyPress);
	glfwSetCharCallback(window, onCharacter);
	glfwSetWindowSizeCallback(window, onResizeWindow);

	return true;
}

/**
 * Initialize the glad library
 * @returns{bool} true if everything goes ok
 * */
bool initGlad()
{
	// Initialize glad
	int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// If something went wrong during the glad initialization
	if (!status)
	{
		std::cout << status << std::endl;
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}
	return true;
}
/**
 * Initialize the opengl context
 * */
void initGL()
{
	// Enables the z-buffer test
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	//Blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// Sets the ViewPort
	glViewport(0, 0, windowWidth, windowHeight);
	// Sets the clear color
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
}

/*
** Init MVP Matrix
*/

void initMVP()
{
	Model = glm::mat4(1.0f);
	View = Camara->getView();
	Proj = glm::perspective(45.0f, 800.0f / 600.0f, 0.1f, 190.0f);

}

void buildModel(std::string path, std::vector< glm::vec3 > position, std::vector< model* >& arrayObj)
{
	std::vector< glm::vec3 > allVert;
	std::vector< glm::vec3 > Vert;
	std::vector< glm::vec3 > Normal;
	std::vector< glm::vec3 > Tangent;
	std::vector< glm::vec3 > Bitangent;
	std::vector< glm::vec2 > UV;
	//Positions

	model* obj = new model(position[0]);
	obj->loadObj(path, Vert, Normal, UV);
	obj->getTangentBitanget(Vert, UV, Normal, Tangent, Bitangent, allVert);

	//int nMod = 3;
	//Load 3 models
	unsigned int VAOForm[1], VBOForm[NVBOS], numVertex = Vert.size();
	// Creates on GPU the vertex array
	glGenVertexArrays(1, &VAOForm[0]);
	// Creates on GPU the vertex buffer obj
	glGenBuffers(NVBOS, VBOForm);
	// Binds the vertex array to set all the its properties
	glBindVertexArray(VAOForm[0]);

	//vexter position VBOForm
	// Sets the buffer geometry data
	glBindBuffer(GL_ARRAY_BUFFER, VBOForm[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Vert.size(), &Vert[0], GL_STATIC_DRAW);
	//vertex position position VAO
	// Sets the vertex attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//uv VBOForm
	// Sets the buffer geometry data
	glBindBuffer(GL_ARRAY_BUFFER, VBOForm[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * UV.size(), &UV[0], GL_STATIC_DRAW);

	//uv VAO
	// Sets the vertex attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	//normal VBOForm
	// Sets the buffer geometry data
	glBindBuffer(GL_ARRAY_BUFFER, VBOForm[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Normal.size(), &Normal[0], GL_STATIC_DRAW);

	//normal object.VAO
	// Sets the vertex attributes
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Tangent VBOForm
	// Sets the buffer geometry data
	glBindBuffer(GL_ARRAY_BUFFER, VBOForm[3]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Tangent.size(), &Tangent[0], GL_STATIC_DRAW);

	//Tangent object.VAO
	// Sets the vertex attributes
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//Bitangent VBOForm
	// Sets the buffer geometry data
	glBindBuffer(GL_ARRAY_BUFFER, VBOForm[4]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * Bitangent.size(), &Bitangent[0], GL_STATIC_DRAW);

	//Bitangent object.VAO
	// Sets the vertex attributes
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);


	//Se encolan los modelos que tienen el mismo obj
	model* obj1;
	for (size_t i = 0; i < position.size(); i++)
	{
		model* obj1 = new model(position[i]);
		//obj1->setPosition(pos[i]);
		obj1->VBO[0] = VBOForm[0];
		obj1->VBO[1] = VBOForm[1];
		obj1->VBO[2] = VBOForm[2];
		obj1->VBO[3] = VBOForm[3];
		obj1->VBO[4] = VBOForm[4];
		obj1->VAO[0] = VAOForm[0];
		obj1->numVertex = numVertex;
		//std::cout << position[i].x << ' ' << position[i].y << position[i].z << std::endl;
		obj1->setPosition(position[i]);
		arrayObj.push_back(obj1);
	}

	position.clear();
}

void buildPlane()
{
	float vertices[] = {
		//Vertices         //Coordenadas de textura 
		-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

	};

	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(planeVAO);

	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

}

/**
 * Builds all the geometry buffers and
 * loads them up into the GPU
 * (Builds a simple triangle)
 * */
void buildGeometry()
{
	std::vector< std::string > paths;
	std::vector < glm::vec3 > pos;
	//Positions
	pos.push_back(glm::vec3(0.0f, 5.0f, 0.0f));
	//Carga el mismo modelo en las distintas posiciones que tiene el arreglo
	buildModel(".\\assets\\models\\Crate.obj", pos, modelsObj);
	pos.clear();

	//Load skybox
	//buildSkyBox();
}
/**
 * Loads a texture into the GPU
 * @param{const char} path of the texture file
 * @returns{unsigned int} GPU texture index
 * */
unsigned int loadTexture(const char* path)
{
	unsigned int id;
	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Loads the texture
	int textureWidth, textureHeight, numberOfChannels;
	// Flips the texture when loads it because in opengl the texture coordinates are flipped
	stbi_set_flip_vertically_on_load(true);

	// Loads the texture file data
	unsigned char* data = stbi_load(path, &textureWidth, &textureHeight, &numberOfChannels, 0);
	if (data)
	{
		// Gets the texture channel format
		GLenum format1;
		GLenum format2;
		switch (numberOfChannels)
		{
		case 1:
			format1 = GL_RED;
			format2 = GL_RED;
			break;
		case 3:
			format1 = GL_SRGB;
			format2 = GL_RGB;
			break;
		case 4:
			format1 = GL_SRGB_ALPHA;
			format2 = GL_RGBA;
			break;

		}

		// Binds the texture
		glBindTexture(GL_TEXTURE_2D, id);
		// Creates the texture
		glTexImage2D(GL_TEXTURE_2D, 0, format1, textureWidth, textureHeight, 0, format2, GL_UNSIGNED_BYTE, data);
		// Creates the texture mipmaps
		glGenerateMipmap(GL_TEXTURE_2D);

		// Set the filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}
	else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}
	// We dont need the data texture anymore because is loaded on the GPU
	stbi_image_free(data);

	return id;
}

unsigned int loadTextureMatrix(std::vector<float>matrix)
{
	
	unsigned int id;

	// Creates the texture on GPU
	glGenTextures(1, &id);
	// Gets the texture channel format
	
	// Binds the texture
	glBindTexture(GL_TEXTURE_1D, id);
	// Creates the texture

	
	// Set the filtering parameters
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, 49, 0, GL_RED, GL_FLOAT, matrix.data());
	glBindTexture(GL_TEXTURE_1D, 0);

	/*else
	{
		std::cout << "ERROR:: Unable to load texture " << path << std::endl;
		glDeleteTextures(1, &id);
	}*/
	// We dont need the data texture anymore because is loaded on the GPU
	
	return id;
}

unsigned int loadCubemap(std::vector<std::string> faces)
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
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
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

void initTexture() {

	textureID1 = loadTexture("assets/textures/el-witcher.jpg");
	textureID = loadTexture("assets/textures/paisaje2.png");
	std::vector<float> pass;
	pass.assign(49, 0.5f);
	pass[0] = 0.0f;
	textMat = loadTextureMatrix(pass);
	resetKernet();
	kernel[0] = 1.0f;
	kernel[1] = 0.5f;
	kernel[2] = 0.0f;
	kernel[40] = 1.0f;
	glDeleteTextures(1, &textMat);

	textMat = loadTextureMatrix(kernel);

	textureActual = textureID1;
	images.push_back(textureID1);
	images.push_back(textureID);
	
}

void initFramebuffer() {

	//Creamos el objeto de framebuffer
	glGenFramebuffers(1, &textureFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
	//creamos una textura 2D que usaremos como buffer de profundidad del framebuffer

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	/*glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);*/
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

	//float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	//glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	////Con la textura de profundidad generada podemos adjuntarla como b�fer de profundidad del framebuffer
	//glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
	//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, texture, 0);
	//glDrawBuffer(GL_NONE);
	//glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}
/**
 * Initialize everything
 * @returns{bool} true if everything goes ok
 * */
bool init()
{
	// Initialize the window, the interface and the glad components
	if (!initWindow() || !initGlad() || !initUserInterface())
		return false;

	// Initialize the opengl context
	initGL();

	// Loads the shader
	shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
	shaderFBO = new Shader("assets/shaders/fbo.vert", "assets/shaders/fbo.frag");

	// Loads all the geometry into the GPU
	buildGeometry();
	buildPlane();

	//Framebuffer
	initFramebuffer();

	// Loads the texture into the GPU
	initTexture();

	//Initializate MVP values
	initMVP();

	
	//Init values of tweakbar
	initUserInterfaceValues();

	return true;
}
/**
 * Process the keyboard input
 * There are ways of implementing this function through callbacks provide by
 * the GLFW API, check the GLFW documentation to find more
 * @param{GLFWwindow} window pointer
 * */
void processKeyboardInput(GLFWwindow* window)
{
	// Checks if the escape key is pressed
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		// Tells glfw to close the window as soon as possible
		glfwSetWindowShouldClose(window, true);

	// Checks if the r key is pressed
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		// Reloads the shader
		delete shader;
		delete shaderFBO;

		shader = new Shader("assets/shaders/basic.vert", "assets/shaders/basic.frag");
		shaderFBO = new Shader("assets/shaders/fbo.vert", "assets/shaders/fbo.frag");

	}

	//Move camera
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		Camara->updateInputKeyboard('w');

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		Camara->updateInputKeyboard('s');

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		Camara->updateInputKeyboard('a');

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		Camara->updateInputKeyboard('d');

}

/*
* Update MVP
*/
void updateMVP(int i, glm::vec3 pos)
{
	Model = glm::mat4(1.0f);

	Model = glm::translate(glm::mat4(1.0f), pos);

	View = Camara->getView();
	//Proj = glm::perspective(45.0f, (float)windowHeight / (float)windowWidth, 0.1f, 100.0f);
}
void kernelCalc()
{
	glm::vec2 k = Interface->kernel;
	if (modeImg != modeImgAnt)
	{
		modeImgAnt = modeImg;
		switch (modeImg) {
		case 1:
			
	
			modeImgAnt = modeImg;

			break;
		case 0:
			
	
			modeImgAnt = modeImg;
			break;
		case 2:
			
	
			modeImgAnt = modeImg;
			break;
		case 3:
			
	
			modeImgAnt = modeImg;

			break;
		case 4:
			
			//sobelKernel(k.x, k.y);
	
			modeImgAnt = modeImg;
			break;
		case 5:
			
			//prewittKernel(k.x, k.y);
	
			modeImgAnt = modeImg;
			break;
		case 6:
			
			//robertsKernel(k.x, k.y);
	
			modeImgAnt = modeImg;
			break;
		case 7:
			
			//promKernel(k.x, k.y);
	
			modeImgAnt = modeImg;
			break;
		case 8:
			
			//medianaKernel(k.x, k.y);
	
			modeImgAnt = modeImg;
			break;
		case 9:
			
			LoGKernel(k.x, k.y, sigma);
	
			modeImgAnt = modeImg;
			break;
		}
		glDeleteTextures(1,&textMat);
		textMat = loadTextureMatrix(kernel);
	}
}

void renderPlane(Shader* shaderActual)
{
	glBindVertexArray(planeVAO);
	shaderActual->use();
	updateMVP(0, glm::vec3(0.0f, 0.0f, -1.0f));
	shaderActual->setMat4("Model", Model);
	shaderActual->setMat4("View", View);
	shaderActual->setMat4("Proj", Proj);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureActual);
	shaderActual->setInt("text", 0);
	//kernel calculate
	
	kernelCalc();
	/*glDeleteTextures(1, &textMat);
	textMat = loadTextureMatrix(kernel);
*/
	shaderActual->setInt("textMat", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, textMat);
	

	shaderActual->setInt("mode", modeImg);
	shaderActual->setVec2("kernel", dataTweak.kernel);
	
	
	//std::cout << dataTweak.kernel.x << " " << dataTweak.kernel.y << std::endl;
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

}

void updateUserInterface()
{
	dataTweak.type = Interface->getFilter();
	if (dataTweak.type == 0) modeImg = 0;
	else if (dataTweak.type == 1) modeImg = 1;
	else if (dataTweak.type == 2) modeImg = 2;
	else if (dataTweak.type == 3) modeImg = 3;
	else if (dataTweak.type == 4) modeImg = 4;
	else if (dataTweak.type == 5) modeImg = 5;
	else if (dataTweak.type == 6) modeImg = 6;
	else if (dataTweak.type == 7) modeImg = 7;
	else if (dataTweak.type == 8) modeImg = 8;
	else if (dataTweak.type == 9) modeImg = 9;
	else if (dataTweak.type == 10) modeImg = 10;
	
	dataTweak = Interface->getData();
}

void renderModels()
{
}
/**
 * Render Function
 * */
void render()
{
	glBindFramebuffer(GL_FRAMEBUFFER, textureFBO);
	glEnable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
	// Clears the color and depth buffers from the frame buffer
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/** Draws code goes here **/

	renderPlane(shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_DEPTH_TEST); // disable depth test so screen-space quad isn't discarded due to depth test.
	// clear all relevant buffers
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // set clear color to white (not really necessery actually, since we won't be able to see behind the quad anyways)
	glClear(GL_COLOR_BUFFER_BIT);
	renderPlane(shader);
	if (Interface->histograma)
	{
		//pintar quad
	glBindVertexArray(planeVAO);
	shaderFBO->use();
	shaderFBO->setInt("text", 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	shaderFBO->setInt("textMat", 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, textMat);
	//glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//renderQuad();
	}
	else {}

	//tweakbar
	TwDraw();

	//Update
	updateUserInterface();

	// Swap the buffer
	glfwSwapBuffers(window);
}
/*
* change speed
*/
void updateCameraSpeed()
{
	currentTime = glfwGetTime();
	deltaTime = currentTime - lastTime;
	lastTime = currentTime;

	Camara->setDeltatime(deltaTime);
}


/**
 * App main loop
 * */
void update()
{
	// Loop until something tells the window, that it has to be closed
	while (!glfwWindowShouldClose(window))
	{
		//Update camera speed
		updateCameraSpeed();
		// Checks for keyboard inputs
		processKeyboardInput(window);

		// Renders everything
		render();


		// Check and call events
		glfwPollEvents();
	}
}


/**
 * App starting point
 * @param{int} number of arguments
 * @param{char const *[]} running arguments
 * @returns{int} app exit code
 * */
int main(int argc, char const* argv[])
{
	// Initialize all the app components
	if (!init())
	{
		// Something went wrong
		std::cin.ignore();
		return -1;
	}

	std::cout << "=====================================================" << std::endl
		<< "        Press Escape to close the program            " << std::endl
		<< "=====================================================" << std::endl;

	// Starts the app main loop
	update();

	// Deletes the texture from the gpu
	glDeleteTextures(1, &textureID);
	glDeleteTextures(1, &texture);
	glDeleteTextures(1, &textMat);
	glDeleteTextures(1, &textureID1);
	glDeleteTextures(1, &textureActual);
	glDeleteTextures(1, &cubemapTexture);
	glDeleteFramebuffers(1, &textureFBO);

	for (size_t i = 0; i < modelsObj.size(); i++)
	{
		// Deletes the vertex array from the GPU
		glDeleteVertexArrays(1, &modelsObj[i]->VAO[0]);
		// Deletes the vertex object from the GPU
		glDeleteBuffers(1, &modelsObj[i]->VBO[0]);
		glDeleteBuffers(1, &modelsObj[i]->VBO[1]);
		glDeleteBuffers(1, &modelsObj[i]->VBO[2]);
		glDeleteBuffers(1, &modelsObj[i]->VBO[3]);
		glDeleteBuffers(1, &modelsObj[i]->VBO[4]);
	}
	glDeleteBuffers(1, &planeVAO);
	glDeleteBuffers(1, &planeVBO);
	// Destroy the shader
	
	delete Camara;
	delete object;
	delete Interface;
	//delete texture;
	//delete textureFBO;

	TwTerminate();

	glfwTerminate();

	return 0;
}