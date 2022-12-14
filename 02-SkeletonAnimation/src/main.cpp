#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>

//glfw include
#include <GLFW/glfw3.h>

// program include
#include "Headers/TimeManager.h"

// Shader include
#include "Headers/Shader.h"

// Model geometric includes
#include "Headers/Sphere.h"
#include "Headers/Cylinder.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

#include "Headers/AnimationUtils.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

GLFWwindow *window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

// Models complex instances
Box skyboxSphere;
Box boxCesped;
Model mVaquero;
Model mCyborg;
Model mGolem;
Model mayowModelAnimate;
Model mAmongUs;

GLuint textureCespedID;
GLuint skyboxTextureID;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

//std::string fileNames[6] = { "../Textures/miPrueba/posx.png",
//		"../Textures/miPrueba/negx.png",
//		"../Textures/miPrueba/posy.png",
//		"../Textures/miPrueba/negy.png",
//		"../Textures/miPrueba/posz.png",
//		"../Textures/miPrueba/negz.png" };

std::string fileNames[6] = { "../Textures/SkyBoxToon/Day_1/px.png",
		"../Textures/SkyBoxToon/Day_1/nx.png",
		"../Textures/SkyBoxToon/Day_1/py.png",
		"../Textures/SkyBoxToon/Day_1/ny.png",
		"../Textures/SkyBoxToon/Day_1/pz.png",
		"../Textures/SkyBoxToon/Day_1/nz.png" };

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

// Model matrix definitions
glm::mat4 matrixModelRock = glm::mat4(1.0);
glm::mat4 modelMatrixAircraft = glm::mat4(1.0);
glm::mat4 modelMatrixMayow = glm::mat4(1.0f);

//Matrices de modelos
glm::mat4 m4Vaquero = glm::mat4(1.0f);
glm::mat4 m4Golem = glm::mat4(1.0f);
glm::mat4 m4AmongUs = glm::mat4(1.0f);


int modelSelected = 0;
bool enableCountSelected = true;

// Variables to animations keyframes
bool saveFrame = false, availableSave = true;
std::ofstream myfile;
std::string fileName = "";
bool record = false;


//??ndices para la animaci??n
int iAnimGolem = 1;
int iAnimMay = 1;
int iAnimCyborg = 1;
int iAnimAmongUs = 1;

//vectores para la animaci??n
glm::vec3 vecMovMay = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovGolem = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovAmongUs = glm::vec3(0.0f, 0.0f, 0.0f);

double deltaTime;
double currTime, lastTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow *window, int key, int scancode, int action,int mode);
void mouseCallback(GLFWwindow *window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod);
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);

// Implementacion de todas las funciones.
void init(int width, int height, std::string strTitle, bool bFullScreen) {

	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if (bFullScreen)
		window = glfwCreateWindow(width, height, strTitle.c_str(),
				glfwGetPrimaryMonitor(), nullptr);
	else
		window = glfwCreateWindow(width, height, strTitle.c_str(), nullptr,
				nullptr);

	if (window == nullptr) {
		std::cerr
				<< "Error to create GLFW window, you can try download the last version of your video card that support OpenGL 3.3+"
				<< std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Init glew
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Failed to initialize glew" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicializaci??n de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);


	mVaquero.loadModel("../models/cowboy/Character Running.fbx");
	//mVaquero.loadModel("../models/mayow/personaje2.fbx");
	mVaquero.setShader(&shaderMulLighting);

	//Mayow
	mayowModelAnimate.loadModel("../models/mayow/personaje2.fbx");
	mayowModelAnimate.setShader(&shaderMulLighting);

	//Cyborg
	mCyborg.loadModel("../models/cyborg/intento_02.fbx");
	mCyborg.setShader(&shaderMulLighting);

	//Golem
	mGolem.loadModel("../models/golem/golem1.fbx");
	mGolem.setShader(&shaderMulLighting);

	//AmongUs
	mAmongUs.loadModel("../models/download/AmongUs/amongus_anim.fbx");
	mAmongUs.setShader(&shaderMulLighting);

	camera->setPosition(glm::vec3(0.0, 3.0, 4.0));

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP *bitmap;
	unsigned char *data;

	// Carga de texturas para el skybox
	Texture skyboxTexture = Texture("");
	glGenTextures(1, &skyboxTextureID);
	// Tipo de textura CUBE MAP
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	for (int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(types); i++) {
		skyboxTexture = Texture(fileNames[i]);
		FIBITMAP *bitmap = skyboxTexture.loadImage(true);
		unsigned char *data = skyboxTexture.convertToData(bitmap, imageWidth,
				imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		} else
			std::cout << "Failed to load texture" << std::endl;
		skyboxTexture.freeImage(bitmap);
	}

	// Definiendo la textura a utilizar
	Texture textureCesped("../Textures/cesped.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureCesped.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureCesped.convertToData(bitmap, imageWidth,
			imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureCespedID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureCespedID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Verifica si se pudo abrir la textura
	if (data) {
		// Transferis los datos de la imagen a memoria
		// Tipo de textura, Mipmaps, Formato interno de openGL, ancho, alto, Mipmaps,
		// Formato interno de la libreria de la imagen, el tipo de dato y al apuntador
		// a los datos
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
		GL_BGRA, GL_UNSIGNED_BYTE, data);
		// Generan los niveles del mipmap (OpenGL es el ecargado de realizarlos)
		glGenerateMipmap(GL_TEXTURE_2D);
	} else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);
}

void destroy() {
	glfwDestroyWindow(window);
	glfwTerminate();
	// --------- IMPORTANTE ----------
	// Eliminar los shader y buffers creados.

	// Shaders Delete
	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCesped.destroy();

	// Custom objects animate
	mayowModelAnimate.destroy();
	mCyborg.destroy();
	mVaquero.destroy();
	mGolem.destroy();
	mAmongUs.destroy();

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);
}

void reshapeCallback(GLFWwindow *Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow *window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow *window, int button, int state, int mod) {
	if (state == GLFW_PRESS) {
		switch (button) {
		case GLFW_MOUSE_BUTTON_RIGHT:
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_LEFT:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
			std::cout << "lastMousePos.x:" << lastMousePosX << std::endl;
			std::cout << "lastMousePos.y:" << lastMousePosY << std::endl;
			break;
		}
	}
}

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera->moveFrontCamera(true, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera->moveFrontCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera->moveRightCamera(false, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera->moveRightCamera(true, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, offsetY, deltaTime);
	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS){
		enableCountSelected = false;
		modelSelected++;
		if(modelSelected > 2)
			modelSelected = 0;
		if(modelSelected == 0)
			fileName = "May";
		if(modelSelected == 1)
			fileName = "golem";
		if (modelSelected == 2)
			fileName = "AmongUs";
		std::cout << "modelSelected:" << modelSelected << std::endl;
	}
	else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		modelMatrixMayow = glm::rotate(modelMatrixMayow, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovMay = glm::vec3(0.0f, 0.0f, 1.0f);
		iAnimMay = 0;
	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		vecMovMay = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimMay = 1;
	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		modelMatrixMayow = glm::rotate(modelMatrixMayow, -0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovMay = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimMay = 0;

	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
		vecMovMay = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimMay = 1;

	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(0.0f, 0.0f, 0.02f));
		vecMovMay = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimMay = 0;

	}
	if (modelSelected == 0 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		vecMovMay = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimMay = 1;
	}

	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		m4Golem = glm::rotate(m4Golem, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovGolem = glm::vec3(0.0f, 0.0f, 1.0f);
		iAnimGolem = 0;
	}
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		vecMovGolem = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimGolem = 1;
	}
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		m4Golem = glm::rotate(m4Golem, -0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovGolem = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimGolem = 0;

	}
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
		vecMovGolem = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimGolem = 1;

	}
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		m4Golem = glm::translate(m4Golem, glm::vec3(0.0f, 0.0f, 0.02f));
		vecMovGolem = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimGolem = 0;

	}
	if (modelSelected == 1 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		vecMovGolem = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimGolem = 1;
	}


	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		m4AmongUs = glm::rotate(m4AmongUs, 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovAmongUs = glm::vec3(0.0f, 0.0f, 1.0f);
		iAnimAmongUs = 0;
	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_RELEASE) {
		vecMovAmongUs = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimAmongUs = 1;
	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		m4AmongUs = glm::rotate(m4AmongUs, -0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
		vecMovAmongUs = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimAmongUs = 0;

	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_RELEASE) {
		vecMovAmongUs = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimAmongUs = 1;

	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		m4AmongUs = glm::translate(m4AmongUs, glm::vec3(0.0f, 0.0f, 0.02f));
		vecMovAmongUs = glm::vec3(0.0f, 1.0f, 0.0f);
		iAnimAmongUs = 0;

	}
	if (modelSelected == 2 && glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE) {
		vecMovAmongUs = glm::vec3(0.0f, 0.0f, 0.0f);
		iAnimAmongUs = 1;
	}

	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	modelMatrixMayow = glm::translate(modelMatrixMayow, glm::vec3(13.0f, 0.05f, -5.0f));
	modelMatrixMayow = glm::rotate(modelMatrixMayow, glm::radians(-90.0f), glm::vec3(0, 1, 0));
	m4Golem = glm::translate(m4Golem, glm::vec3(4, 0, 4));//pos inicial
	m4AmongUs = glm::translate(m4AmongUs, glm::vec3(-4, 0, 4));//pos inicial

	lastTime = TimeManager::Instance().GetTime();

	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if(currTime - lastTime < 0.016666667){
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		// Variables donde se guardan las matrices de cada articulacion por 1 frame
		std::vector<float> matrixDartJoints;
		std::vector<glm::mat4> matrixDart;
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
				(float) screenWidth / (float) screenHeight, 0.01f, 100.0f);
		glm::mat4 view = camera->getViewMatrix();

		// Settea la matriz de vista y projection al shader con solo color
		shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shader.setMatrix4("view", 1, false, glm::value_ptr(view));

		// Settea la matriz de vista y projection al shader con skybox
		shaderSkybox.setMatrix4("projection", 1, false,
				glm::value_ptr(projection));
		shaderSkybox.setMatrix4("view", 1, false,
				glm::value_ptr(glm::mat4(glm::mat3(view))));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderMulLighting.setMatrix4("projection", 1, false,
					glm::value_ptr(projection));
		shaderMulLighting.setMatrix4("view", 1, false,
				glm::value_ptr(view));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.7, 0.7, 0.7)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.9, 0.9, 0.9)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-1.0, 0.0, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		shaderMulLighting.setInt("spotLightCount", 0);

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", 0);

		/*******************************************
		 * Cesped
		 *******************************************/
		glm::mat4 modelCesped = glm::mat4(1.0);
		modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
		modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
		// Se activa la textura del agua
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureCespedID);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(200, 200)));
		boxCesped.render(modelCesped);
		shaderMulLighting.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
		glBindTexture(GL_TEXTURE_2D, 0);

		/*******************************************
		 * Custom Anim objects obj
		 *******************************************/
		

		//Cyborg
		glm::mat4 modelMatrixCyborg = glm::mat4(1.0f);
		modelMatrixCyborg = glm::translate(modelMatrixCyborg, glm::vec3(4, 0, 0));
		modelMatrixCyborg = glm::scale(modelMatrixCyborg, glm::vec3(0.01f, 0.01f, 0.01f));
		mCyborg.setAnimationIndex(0);
		mCyborg.render(modelMatrixCyborg);

		//matriz cuerpo del vaquero
		glm::mat4 mcVaquero = glm::mat4(m4Vaquero);
		mcVaquero = glm::translate(mcVaquero, glm::vec3(0.0f, 0.0f, 0.0f));
		mcVaquero = glm::scale(mcVaquero, glm::vec3(0.005f, 0.005f, 0.005f));
		mVaquero.setAnimationIndex(0);
		mVaquero.render(mcVaquero);

		//mu??equita May
		glm::mat4 modelMatrixMayowBody = glm::mat4(modelMatrixMayow);
		modelMatrixMayowBody = glm::scale(modelMatrixMayowBody, glm::vec3(0.021, 0.021, 0.021));
		modelMatrixMayowBody = glm::translate(modelMatrixMayowBody, vecMovMay);
		mayowModelAnimate.setAnimationIndex(iAnimMay);
		mayowModelAnimate.render(modelMatrixMayowBody);

		//golem
		glm::mat4 m4GolemCorp = m4Golem;
		m4GolemCorp = glm::scale(m4GolemCorp, glm::vec3(0.005f, 0.005f, 0.005f));
		m4GolemCorp = glm::translate(m4GolemCorp, vecMovGolem);
		mGolem.setAnimationIndex(iAnimGolem);
		mGolem.render(m4GolemCorp);

		//AmongUs
		glm::mat4 m4AmongUsCorp = m4AmongUs;
		m4AmongUsCorp = glm::translate(m4AmongUsCorp, vecMovAmongUs);
		m4AmongUsCorp = glm::scale(m4AmongUsCorp, glm::vec3(0.01f, 0.01f, 0.01f));
		mAmongUs.setAnimationIndex(iAnimAmongUs);
		mAmongUs.render(m4AmongUsCorp);

		/*******************************************
		 * Skybox
		 *******************************************/
		GLint oldCullFaceMode;
		GLint oldDepthFuncMode;
		// deshabilita el modo del recorte de caras ocultas para ver las esfera desde adentro
		glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
		glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);
		shaderSkybox.setFloat("skybox", 0);
		glCullFace(GL_FRONT);
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		skyboxSphere.render();
		glCullFace(oldCullFaceMode);
		glDepthFunc(oldDepthFuncMode);

		glfwSwapBuffers(window);
	}
}

int main(int argc, char **argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}
