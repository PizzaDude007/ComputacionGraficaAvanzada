#define _USE_MATH_DEFINES
#include <cmath>
//glew include
#include <GL/glew.h>

//std includes
#include <string>
#include <iostream>
#include <random>
#include <algorithm>
#include <ctime>
#include <iterator>
#include <iostream>

// contains new std::shuffle definition
#include <algorithm>
#include <random>

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
#include "Headers/ThirdPersonCamera.h"

//GLM include
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Headers/Texture.h"

// Include loader Model class
#include "Headers/Model.h"

// Include Terrain
#include "Headers/Terrain.h"

#include "Headers/AnimationUtils.h"

// Include Colision headers functions
#include "Headers/Colisiones.h"

// OpenAL include
#include <AL/alut.h>

//Shadowbox include
#include "Headers/ShadowBox.h"

#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))

int screenWidth;
int screenHeight;

const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;

GLFWwindow* window;

Shader shader;
//Shader con skybox
Shader shaderSkybox;
//Shader con multiples luces
Shader shaderMulLighting;
//Shader para el terreno
Shader shaderTerrain;
//Shader para las particulas de fountain
Shader shaderParticlesFountain;
//Shader para las particulas de fuego
Shader shaderParticlesFire;
//Shader para visualizar el buffer de profundidad
Shader shaderViewDepth;
//Shader para dibujar el buffer de profunidad
Shader shaderDepth;
//Shader para dibujar una textura
Shader shaderTexture;

//Shadow box object
ShadowBox* shadowBox;

std::shared_ptr<Camera> camera(new ThirdPersonCamera());
float distanceFromTarget = 7.0;

Sphere skyboxSphere(20, 20);
Box boxCollider;
Sphere sphereCollider(10, 10);
Box boxViewDepth;
Box boxLightViewBox;
Box boxIntro;
Model rayModel;


// Terrain model instance
Terrain terrain(-1, -1, 400, 16, "../Textures/heightmap.png");

GLuint textureCespedID, textureWallID, textureWindowID, textureHighwayID, textureLandingPadID, textureActivaID;
GLuint textureTerrainBackgroundID, textureTerrainRID, textureTerrainGID, textureTerrainBID, textureTerrainBlendMapID;
GLuint textureParticleFountainID, textureParticleFireID, texId;
GLuint skyboxTextureID;

bool iniciaPartida = false, presionaEnter = false, presionarOpcion = false, keyJumpStatus=false, presiona = false, suelta = false;

GLenum types[6] = {
GL_TEXTURE_CUBE_MAP_POSITIVE_X,
GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
GL_TEXTURE_CUBE_MAP_NEGATIVE_Z };

std::string fileNames[6] = { "../Textures/SkyBoxToon/Night_1/px.png",
		"../Textures/SkyBoxToon/Night_1/nx.png",
		"../Textures/SkyBoxToon/Night_1/py.png",
		"../Textures/SkyBoxToon/Night_1/ny.png",
		"../Textures/SkyBoxToon/Night_1/pz.png",
		"../Textures/SkyBoxToon/Night_1/nz.png" };

// Lamps positions
std::vector<glm::vec3> lamp1Position = { glm::vec3(-7.03, 0, -19.14), glm::vec3(
		24.41, 0, -34.57), glm::vec3(-10.15, 0, -54.10) };
std::vector<float> lamp1Orientation = { -17.0, -82.67, 23.70 };
std::vector<glm::vec3> lamp2Position = { glm::vec3(-36.52, 0, -23.24),
		glm::vec3(-52.73, 0, -3.90) };
std::vector<float> lamp2Orientation = { 21.37 + 90, -65.0 + 90 };

// Blending model unsorted
std::map<std::string, glm::vec3> blendingUnsorted = {
		{"fire", glm::vec3(0.8, 0.0, 0.3)},
};

// Jump variables
bool isJump = false;
float GRAVITY = 1.81;
double tmv = 0;
double startTimeJump = 0;

// Definition for the particle system
GLuint initVel, startTime;
GLuint VAOParticles;
GLuint nParticles = 8000;
double currTimeParticlesAnimation, lastTimeParticlesAnimation;

// Definition for the particle system fire
GLuint initVelFire, startTimeFire;
GLuint VAOParticlesFire;
GLuint nParticlesFire = 800;
GLuint posBuf[2], velBuf[2], age[2];
GLuint particleArray[2];
GLuint feedback[2];
GLuint drawBuf = 1;
float particleSize = 1.5, particleLifetime = 6.0;
double currTimeParticlesAnimationFire, lastTimeParticlesAnimationFire;

bool exitApp = false;
int lastMousePosX, offsetX = 0;
int lastMousePosY, offsetY = 0;

//Todos los nuevos modelos los podemos documentar como n1, n2... para saber dónde y cuándo se
//generaron sus matrices de movimiento, índices de anim etc.

//n1 Modelo gráfico
//n2 Matriz de movimiento del modelo
//n3 Vectores para la animación
//n4 Vector de escala, para escalar el tamaño del modelo
//n5 Indices para la animación
//n6 booleano objeto animado o inanimado
//n7 tipo de colisionador (OBB, AABB, SBB...)
//n8 tiempo de muerte
//n9 lista de modelos muertos

//n1 Modelo gráfico
std::vector<Model*> listaModelos;
Model mFlecha;
Model mArquero;
Model mTroll;
Model mTrollGrey;
Model mTrollOrange;
Model mOrkGreen;
Model mOrkOrange;
Model mOrkPink;
Model mShootingRange;
Model mFort;
Model mHouseDestroyed;
Model mDebris;

//n2 Matriz de movimiento del modelo
std::vector<glm::mat4*> listaMat4;
glm::mat4 m4Flecha = glm::mat4(1.0f);
glm::mat4 m4Arquero = glm::mat4(1.0f);
glm::mat4 m4Troll = glm::mat4(1.0f);
glm::mat4 m4TrollGrey = glm::mat4(1.0f);
glm::mat4 m4TrollOrange = glm::mat4(1.0f);
glm::mat4 m4OrkGreen = glm::mat4(1.0f);
glm::mat4 m4OrkOrange = glm::mat4(1.0f);
glm::mat4 m4OrkPink = glm::mat4(1.0f);
glm::mat4 m4ShootingRange = glm::mat4(1.0f);
glm::mat4 m4Fort = glm::mat4(1.0f);
glm::mat4 m4HouseDestroyed = glm::mat4(1.0f);
glm::mat4 m4Debris = glm::mat4(1.0f);

//n3 Vectores para la animación
std::vector<glm::vec3*> listaVecMov;
glm::vec3 vecMovFlecha = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovArquero = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovTroll = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovTrollGrey = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovTrollOrange = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovOrkGreen = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovOrkOrange = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 vecMovOrkPink = glm::vec3(0.0f, 0.0f, 0.0f);

//n4 Vector de escala, para escalar el tamaño del modelo
std::vector<glm::vec3*> listaEscalas;
glm::vec3 escalaFlecha = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 escalaArquero = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 escalaTroll = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 escalaTrollGrey = escalaTroll;
glm::vec3 escalaTrollOrange = escalaTroll;
glm::vec3 escalaOrkGreen = escalaTroll;
glm::vec3 escalaOrkOrange = escalaTroll;
glm::vec3 escalaOrkPink = escalaTroll;
glm::vec3 escalaShootingRange = glm::vec3(0.01f, 0.01f, 0.01f);
glm::vec3 escalaFort = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 escalaHouseDestroyed = glm::vec3(0.01f, 0.01f, 0.01f);
glm::vec3 escalaDebris = glm::vec3(0.01f, 0.01f, 0.01f);

const int JUGADOR = 0;
const int ENEMIGO = 1;
const int OBJETO = 2;
const int PROYECTIL = 3;

const int ANIM_TROLL_CORRE = 18;
const int ANIM_TROLL_MUERE = 0;
const int ANIM_TROLL_GOLPE = 1;

const bool VIVO = true;
const bool MUERTO = false;

glm::vec3 SIN_MOV = glm::vec3(0.0f, 0.0f, 0.0f);

//n5 Índices para la animación
std::vector<int> listaIndiceAnim;
int iAnimArquero = 0;
int iAnimEnemigo = ANIM_TROLL_CORRE;

//n6 Tipo de objeto animado
//(0 - jugador; 1 - enemigo; 2 - inanimado)
std::vector<int> listaTipoObjeto;

//n7 Tipo de colisionador (geometría)
//elemento[0]	0 = sinCollider	 1 = SBB; 2 = OBB
//elemento[1]	0 = sin colisión 	1 = bloquea con colsión
//ie 		00 -> SBB transparente a colisión
//ie		12 -> OBB que sí bloquea colisión
std::vector<int> listaTipoCollider;

//n8 Tiempo de animación (para animación de muerte)
std::vector<double> listaTiempoAnim;

//n9 Personaje vivo o muerto
std::vector<bool> listaVivoOMuerto;


// Colliders
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> > collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > collidersSBB;
std::map<std::string, bool> collisionDetection;

// Framesbuffers
GLuint depthMap, depthMapFBO;

/**********************
 * OpenAL config
 */

 // OpenAL Defines
#define NUM_BUFFERS 4
#define NUM_SOURCES 4
#define NUM_ENVIRONMENTS 1
// Listener
ALfloat listenerPos[] = { 0.0, 0.0, 4.0 };
ALfloat listenerVel[] = { 0.0, 0.0, 0.0 };
ALfloat listenerOri[] = { 0.0, 0.0, 1.0, 0.0, 1.0, 0.0 };
// Source 0
ALfloat source0Pos[] = { -2.0, 0.0, 0.0 };
ALfloat source0Vel[] = { 0.0, 0.0, 0.0 };
// Source 1
ALfloat source1Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source1Vel[] = { 0.0, 0.0, 0.0 };
// Source 2
ALfloat source2Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source2Vel[] = { 0.0, 0.0, 0.0 };
// Source 3
ALfloat source3Pos[] = { 2.0, 0.0, 0.0 };
ALfloat source3Vel[] = { 0.0, 0.0, 0.0 };
// Buffers
ALuint buffer[NUM_BUFFERS];
ALuint source[NUM_SOURCES];
ALuint environment[NUM_ENVIRONMENTS];
// Configs
ALsizei size, freq;
ALenum format;
ALvoid* data;
int ch;
ALboolean loop;
std::vector<bool> sourcesPlay = { true, true, true, true };

//Variables de control
int iModeloSelect = 0;
bool enableCountSelected = true;
bool proyectilGenerado = false;
bool flechaCollision = false;

std::string fileName = "";

double deltaTime;
double currTime, lastTime;

// Se definen todos las funciones.
void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes);
void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
void initParticleBuffers();
void init(int width, int height, std::string strTitle, bool bFullScreen);
void destroy();
bool processInput(bool continueApplication = true);
void prepareScene();
void prepareDepthScene();
void renderScene(bool renderParticles = true);
void destruyeEnemigo(int NumModelo);
void destruyeFlecha(int NumModelo);
void actualizaMovimientos();
void animaMuerte();

void initParticleBuffers() {
	// Generate the buffers
	glGenBuffers(1, &initVel);   // Initial velocity buffer
	glGenBuffers(1, &startTime); // Start time buffer

	// Allocate space for all buffers
	int size = nParticles * 3 * sizeof(float);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferData(GL_ARRAY_BUFFER, nParticles * sizeof(float), NULL, GL_STATIC_DRAW);

	// Fill the first velocity buffer with random velocities
	glm::vec3 v(0.0f);
	float velocity, theta, phi;
	GLfloat* data = new GLfloat[nParticles * 3];
	for (unsigned int i = 0; i < nParticles; i++) {

		theta = glm::mix(0.0f, glm::pi<float>() / 6.0f, ((float)rand() / RAND_MAX));
		phi = glm::mix(0.0f, glm::two_pi<float>(), ((float)rand() / RAND_MAX));

		v.x = sinf(theta) * cosf(phi);
		v.y = cosf(theta);
		v.z = sinf(theta) * sinf(phi);

		velocity = glm::mix(0.6f, 0.8f, ((float)rand() / RAND_MAX));
		v = glm::normalize(v) * velocity;

		data[3 * i] = v.x;
		data[3 * i + 1] = v.y;
		data[3 * i + 2] = v.z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);

	// Fill the start time buffer
	delete[] data;
	data = new GLfloat[nParticles];
	float time = 0.0f;
	float rate = 0.00075f;
	for (unsigned int i = 0; i < nParticles; i++) {
		data[i] = time;
		time += rate;
	}
	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glBufferSubData(GL_ARRAY_BUFFER, 0, nParticles * sizeof(float), data);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	delete[] data;

	glGenVertexArrays(1, &VAOParticles);
	glBindVertexArray(VAOParticles);
	glBindBuffer(GL_ARRAY_BUFFER, initVel);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, startTime);
	glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void initParticleBuffersFire() {
	// Generate the buffers
	glGenBuffers(2, posBuf);    // position buffers
	glGenBuffers(2, velBuf);    // velocity buffers
	glGenBuffers(2, age);       // age buffers

	// Allocate space for all buffers
	int size = nParticlesFire * sizeof(GLfloat);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glBufferData(GL_ARRAY_BUFFER, 3 * size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);
	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glBufferData(GL_ARRAY_BUFFER, size, 0, GL_DYNAMIC_COPY);

	// Fill the first age buffer
	std::vector<GLfloat> initialAge(nParticlesFire);
	float rate = particleLifetime / nParticlesFire;
	for (unsigned int i = 0; i < nParticlesFire; i++) {
		int index = i - nParticlesFire;
		float result = rate * index;
		initialAge[i] = result;
	}
	// Shuffle them for better looking results
	//Random::shuffle(initialAge);
	auto rng = std::default_random_engine{};
	std::shuffle(initialAge.begin(), initialAge.end(), rng);
	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialAge.data());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Create vertex arrays for each set of buffers
	glGenVertexArrays(2, particleArray);

	// Set up particle array 0
	glBindVertexArray(particleArray[0]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[0]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[0]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// Set up particle array 1
	glBindVertexArray(particleArray[1]);
	glBindBuffer(GL_ARRAY_BUFFER, posBuf[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, velBuf[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, age[1]);
	glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	// Setup the feedback objects
	glGenTransformFeedbacks(2, feedback);

	// Transform feedback 0
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[0]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[0]);

	// Transform feedback 1
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, posBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, velBuf[1]);
	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, age[1]);

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

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
	glfwSetScrollCallback(window, scrollCallback);
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

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// Inicialización de los shaders
	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox_fog.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation_shadow.vs", "../Shaders/multipleLights_shadow.fs");
	shaderTerrain.initialize("../Shaders/terrain_shadow.vs", "../Shaders/terrain_shadow.fs");
	shaderParticlesFountain.initialize("../Shaders/particlesFountain.vs", "../Shaders/particlesFountain.fs");
	shaderParticlesFire.initialize("../Shaders/particlesFire.vs", "../Shaders/particlesFire.fs", { "Position", "Velocity", "Age" });
	shaderViewDepth.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado_depth_view.fs");
	shaderDepth.initialize("../Shaders/shadow_mapping_depth.vs", "../Shaders/shadow_mapping_depth.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");

	// Inicializacion de los objetos.
	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f, 20.0f, 20.0f));

	terrain.init();
	terrain.setShader(&shaderMulLighting);
	terrain.setPosition(glm::vec3(200, 0, 200));

	boxCollider.init();
	boxCollider.setShader(&shader);
	boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	sphereCollider.init();
	sphereCollider.setShader(&shader);
	sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));

	rayModel.init();
	rayModel.setShader(&shader);
	rayModel.setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// - marroja
	//n1 Modelo gráfico
	//n2 Matriz del modelo
	//n3 Vectores de movimiento para la animación
	//n4 Vector de escala, para escalar el tamaño del modelo
	//n5 Índices para la animación

	//Flecha
	//mFlecha.loadModel("../models/download/flecha2/flecha.fbx");
	//mFlecha.loadModel("../models/download/arquero/arrow.fbx");
	mFlecha.loadModel("../models/download/arquero/flecha.fbx");
	m4Flecha = glm::mat4(1.0f);
	vecMovFlecha = glm::vec3(0.0f, 0.0f, 0.0f);

	listaModelos.push_back(&mFlecha);			//n1
	listaMat4.push_back(&m4Flecha);				//n2
	listaVecMov.push_back(&vecMovFlecha);		//n3
	listaEscalas.push_back(&escalaFlecha);		//n4
	listaIndiceAnim.push_back(0);				//n5
	listaTipoObjeto.push_back(PROYECTIL);		//n6 0 = jugador; 1 = enemigo; 2 = objeto; 3 proyectil
	listaTipoCollider.push_back(11);			//n7 10 -> tiene colisión 2 -> OBB //La flecha que sea una esfera
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Arquero
	//mArquero.loadModel("../models/golem/golem1.fbx");
	mArquero.loadModel("../models/download/arquero/arquero.fbx");
	m4Arquero = glm::mat4(1.0f);
	vecMovArquero = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mArquero);		//n1
	listaMat4.push_back(&m4Arquero);		//n2
	listaVecMov.push_back(&vecMovArquero);	//n3
	listaEscalas.push_back(&escalaArquero);	//n4
	listaIndiceAnim.push_back(iAnimArquero);	//n5
	listaTipoObjeto.push_back(JUGADOR);			//n6
	listaTipoCollider.push_back(11);			//n7 10 -> tiene colisión 2 -> OBB
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	//n6 quizá valga la pena hacerlo el shader (qpd qph) queda-por-determinar queda-por-hacer

	//Troll
	//mTroll.loadModel("../models/download/Troll/amongus_anim.fbx");
	mTroll.loadModel("../models/download/troll/troll_green.fbx");
	m4Troll = glm::mat4(1.0f);
	vecMovTroll = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mTroll);				//n1
	listaMat4.push_back(&m4Troll);					//n2
	listaVecMov.push_back(&vecMovTroll);			//n3
	listaEscalas.push_back(&escalaTroll);			//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Troll Grey
	//mTroll.loadModel("../models/download/Troll/amongus_anim.fbx");
	mTrollGrey.loadModel("../models/download/troll/troll_green.fbx");
	m4TrollGrey = glm::mat4(1.0f);
	vecMovTrollGrey = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mTrollGrey);			//n1
	listaMat4.push_back(&m4TrollGrey);				//n2
	listaVecMov.push_back(&vecMovTrollGrey);		//n3
	listaEscalas.push_back(&escalaTrollGrey);		//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Troll Orange
	//mTrollOrange.loadModel("../models/download/troll/troll_orange.fbx");
	mTrollOrange.loadModel("../models/download/troll/troll_green.fbx");
	m4TrollOrange = glm::mat4(1.0f);
	vecMovTrollOrange = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mTrollOrange);			//n1
	listaMat4.push_back(&m4TrollOrange);			//n2
	listaVecMov.push_back(&vecMovTrollOrange);		//n3
	listaEscalas.push_back(&escalaTrollOrange);		//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Ork Green
	//mOrkGreen.loadModel("../models/download/ork_new/ork_green.fbx");
	mOrkGreen.loadModel("../models/download/troll/troll_green.fbx");
	m4OrkGreen = glm::mat4(1.0f);
	vecMovOrkGreen = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mOrkGreen);				//n1
	listaMat4.push_back(&m4OrkGreen);				//n2
	listaVecMov.push_back(&vecMovOrkGreen);			//n3
	listaEscalas.push_back(&escalaOrkGreen);		//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Ork Orange
	//mOrkOrange.loadModel("../models/download/ork_new/ork_orange.fbx");
	mOrkOrange.loadModel("../models/download/troll/troll_green.fbx");
	m4OrkOrange = glm::mat4(1.0f);
	vecMovOrkOrange = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mOrkOrange);			//n1
	listaMat4.push_back(&m4OrkOrange);				//n2
	listaVecMov.push_back(&vecMovOrkOrange);		//n3
	listaEscalas.push_back(&escalaOrkOrange);		//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Ork Pink
	//mOrkPink.loadModel("../models/download/ork_new/ork_pink.fbx");
	mOrkPink.loadModel("../models/download/troll/troll_green.fbx");
	m4OrkPink = glm::mat4(1.0f);
	vecMovOrkPink = glm::vec3(0.0f, 0.0f, 0.08f);

	listaModelos.push_back(&mOrkPink);				//n1
	listaMat4.push_back(&m4OrkPink);				//n2
	listaVecMov.push_back(&vecMovOrkPink);			//n3
	listaEscalas.push_back(&escalaOrkPink);			//n4
	listaIndiceAnim.push_back(iAnimEnemigo);		//n5
	listaTipoObjeto.push_back(ENEMIGO);				//n6 0 = jugador; 1 = enemigo; 2 = objeto
	listaTipoCollider.push_back(11);				//n7 10 -> tiene colisión 2 -> OBB
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting); // sí se puede manipular desde el vector

	//Shooting Range
	mShootingRange.loadModel("../models/download/created/campo_tiro.fbx");
	m4ShootingRange = glm::mat4(1.0f);

	listaModelos.push_back(&mShootingRange); //n1
	listaMat4.push_back(&m4ShootingRange); //n2
	listaEscalas.push_back(&escalaShootingRange); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaModelos.back()->setShader(&shaderMulLighting);

	//Fort
	//mFort.loadModel("../models/polygon/created/Textures/fortaleza.fbx");
	mFort.loadModel("../models/polygon/created/Textures/fortaleza_lite.fbx");
	m4Fort = glm::mat4(1.0f);

	listaModelos.push_back(&mFort); //n1
	listaMat4.push_back(&m4Fort); //n2
	listaEscalas.push_back(&escalaFort); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	// House Destroyed
	mHouseDestroyed.loadModel("../models/download/created/casa_destroyed.fbx");
	m4HouseDestroyed = glm::mat4(1.0f);

	listaModelos.push_back(&mHouseDestroyed); //n1
	listaMat4.push_back(&m4HouseDestroyed); //n2
	listaEscalas.push_back(&escalaHouseDestroyed); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	// Debris
	mDebris.loadModel("../models/download/created/debris.fbx");
	m4Debris = glm::mat4(1.0f);

	listaModelos.push_back(&mDebris); //n1
	listaMat4.push_back(&m4Debris); //n2
	listaEscalas.push_back(&escalaDebris); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(11); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	listaVecMov.push_back(&SIN_MOV);		//n3
	listaIndiceAnim.push_back(0);			//n5

	//Shooting Range
	mShootingRange.loadModel("../models/download/created/campo_tiro.fbx");
	m4ShootingRange = glm::mat4(1.0f);

	listaModelos.push_back(&mShootingRange); //n1
	listaMat4.push_back(&m4ShootingRange); //n2
	listaEscalas.push_back(&escalaShootingRange); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	listaVecMov.push_back(&SIN_MOV);		//n3
	listaIndiceAnim.push_back(0);			//n5

	//Fort
	//mFort.loadModel("../models/polygon/created/Textures/fortaleza.fbx");
	mFort.loadModel("../models/polygon/created/Textures/fortaleza_lite.fbx");
	m4Fort = glm::mat4(1.0f);

	listaModelos.push_back(&mFort); //n1
	listaMat4.push_back(&m4Fort); //n2
	listaEscalas.push_back(&escalaFort); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	listaVecMov.push_back(&SIN_MOV);		//n3
	listaIndiceAnim.push_back(0);			//n5

	// House Destroyed
	mHouseDestroyed.loadModel("../models/download/created/casa_destroyed.fbx");
	m4HouseDestroyed = glm::mat4(1.0f);
	
	listaModelos.push_back(&mHouseDestroyed); //n1
	listaMat4.push_back(&m4HouseDestroyed); //n2
	listaEscalas.push_back(&escalaHouseDestroyed); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(0); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	listaVecMov.push_back(&SIN_MOV);		//n3
	listaIndiceAnim.push_back(0);			//n5

	// Debris
	mDebris.loadModel("../models/download/created/debris.fbx");
	m4Debris = glm::mat4(1.0f);

	listaModelos.push_back(&mDebris); //n1
	listaMat4.push_back(&m4Debris); //n2
	listaEscalas.push_back(&escalaDebris); //n4
	listaTipoObjeto.push_back(2); //n6
	listaTipoCollider.push_back(11); //n7
	listaTiempoAnim.push_back(0.0);				//n8 tiempo animación (muerte o golpe)
	listaVivoOMuerto.push_back(VIVO);			//n9 estado de vivo o muerto (la flecha siempre está viva)
	listaModelos.back()->setShader(&shaderMulLighting);

	listaVecMov.push_back(&SIN_MOV);		//n3
	listaIndiceAnim.push_back(0);			//n5


	// - fin modelos

	camera->setPosition(glm::vec3(0.0, 0.0, 10.0));
	camera->setDistanceFromTarget(distanceFromTarget);
	camera->setSensitivity(1.0);

	// Definimos el tamanio de la imagen
	int imageWidth, imageHeight;
	FIBITMAP* bitmap;
	unsigned char* data;

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
		FIBITMAP* bitmap = skyboxTexture.loadImage(true);
		unsigned char* data = skyboxTexture.convertToData(bitmap, imageWidth,
			imageHeight);
		if (data) {
			glTexImage2D(types[i], 0, GL_RGBA, imageWidth, imageHeight, 0,
				GL_BGRA, GL_UNSIGNED_BYTE, data);
		}
		else
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureCesped.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainBackground("../Textures/grassy2.png");
	Texture textureTerrainBackground("../Textures/low_poly/grass.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBackground.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBackground.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBackgroundID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBackground.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainR("../Textures/mud.png");
	Texture textureTerrainR("../Textures/low_poly/dirt.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainR.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainR.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainRID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainR.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainG("../Textures/grassFlowers.png");
	Texture textureTerrainG("../Textures/low_poly/grass_flowers.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainG.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainG.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainGID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainG.freeImage(bitmap);

	// Definiendo la textura a utilizar
	//Texture textureTerrainB("../Textures/path.png");
	Texture textureTerrainB("../Textures/low_poly/brick_2.jpg");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainB.loadImage();
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainB.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainB.freeImage(bitmap);

	// Definiendo la textura a utilizar
	Texture textureTerrainBlendMap("../Textures/blendMap.png");
	// Carga el mapa de bits (FIBITMAP es el tipo de dato de la libreria)
	bitmap = textureTerrainBlendMap.loadImage(true);
	// Convertimos el mapa de bits en un arreglo unidimensional de tipo unsigned char
	data = textureTerrainBlendMap.convertToData(bitmap, imageWidth,
		imageHeight);
	// Creando la textura con id 1
	glGenTextures(1, &textureTerrainBlendMapID);
	// Enlazar esa textura a una tipo de textura de 2D.
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	// Libera la memoria de la textura
	textureTerrainBlendMap.freeImage(bitmap);

	//Texture textureParticleFire("../Textures/fire.png");
	Texture textureParticleFire("../Textures/low_poly/fire_2.png");
	bitmap = textureParticleFire.loadImage();
	data = textureParticleFire.convertToData(bitmap, imageWidth, imageHeight);
	glGenTextures(1, &textureParticleFireID);
	glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0,
			GL_BGRA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
		std::cout << "Failed to load texture" << std::endl;
	textureParticleFire.freeImage(bitmap);

	std::uniform_real_distribution<float> distr01 = std::uniform_real_distribution<float>(0.0f, 1.0f);
	std::mt19937 generator;
	std::random_device rd;
	generator.seed(rd());
	int size = nParticlesFire * 2;
	std::vector<GLfloat> randData(size);
	for (int i = 0; i < randData.size(); i++) {
		randData[i] = distr01(generator);
	}

	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_1D, texId);
	glTexStorage1D(GL_TEXTURE_1D, 1, GL_R32F, size);
	glTexSubImage1D(GL_TEXTURE_1D, 0, 0, size, GL_RED, GL_FLOAT, randData.data());
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	shaderParticlesFire.setInt("Pass", 1);
	shaderParticlesFire.setInt("ParticleTex", 0);
	shaderParticlesFire.setInt("RandomTex", 1);
	shaderParticlesFire.setFloat("ParticleLifetime", particleLifetime);
	shaderParticlesFire.setFloat("ParticleSize", particleSize);
	shaderParticlesFire.setVectorFloat3("Accel", glm::value_ptr(glm::vec3(0.0f, 0.1f, 0.0f)));
	shaderParticlesFire.setVectorFloat3("Emitter", glm::value_ptr(glm::vec3(0.0f)));

	glm::mat3 basis;
	glm::vec3 u, v, n;
	v = glm::vec3(0, 1, 0);
	n = glm::cross(glm::vec3(1, 0, 0), v);
	if (glm::length(n) < 0.00001f) {
		n = glm::cross(glm::vec3(0, 1, 0), v);
	}
	u = glm::cross(v, n);
	basis[0] = glm::normalize(u);
	basis[1] = glm::normalize(v);
	basis[2] = glm::normalize(n);
	shaderParticlesFire.setMatrix3("EmitterBasis", 1, false, glm::value_ptr(basis));

	/*******************************************
	 * Inicializacion de los buffers de la fuente
	 *******************************************/
	initParticleBuffers();

	/*******************************************
	 * Inicializacion de los buffers del fuego
	 *******************************************/
	initParticleBuffersFire();

	/*******************************************
	 * Inicializacion del framebuffer para
	 * almacenar el buffer de profunidadad
	 *******************************************/
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	/*glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/*******************************************
	 * OpenAL init
	 *******************************************/
	alutInit(0, nullptr);
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
	alGetError(); // clear any error messages
	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating buffers !!\n");
		exit(1);
	}
	else {
		printf("init() - No errors yet.");
	}
	// Config source 0
	// Generate buffers, or else no sound will happen!
	alGenBuffers(NUM_BUFFERS, buffer);
	buffer[0] = alutCreateBufferFromFile("../sounds/fountain.wav");
	buffer[1] = alutCreateBufferFromFile("../sounds/fire.wav");
	buffer[2] = alutCreateBufferFromFile("../sounds/darth_vader.wav");
	buffer[3] = alutCreateBufferFromFile("../sounds/lawyer1.wav");
	int errorAlut = alutGetError();
	if (errorAlut != ALUT_ERROR_NO_ERROR) {
		printf("- Error open files with alut %d !!\n", errorAlut);
		exit(2);
	}


	alGetError(); /* clear error */
	alGenSources(NUM_SOURCES, source);

	if (alGetError() != AL_NO_ERROR) {
		printf("- Error creating sources !!\n");
		exit(2);
	}
	else {
		printf("init - no errors after alGenSources\n");
	}
	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 3.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcef(source[0], AL_MAX_DISTANCE, 2000);

	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 3.0f);
	alSourcefv(source[1], AL_POSITION, source1Pos);
	alSourcefv(source[1], AL_VELOCITY, source1Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcef(source[1], AL_MAX_DISTANCE, 2000);

	alSourcef(source[2], AL_PITCH, 1.0f);
	alSourcef(source[2], AL_GAIN, 0.3f);
	alSourcefv(source[2], AL_POSITION, source2Pos);
	alSourcefv(source[2], AL_VELOCITY, source2Vel);
	alSourcei(source[2], AL_BUFFER, buffer[2]);
	alSourcei(source[2], AL_LOOPING, AL_TRUE);
	alSourcef(source[2], AL_MAX_DISTANCE, 500);

	alSourcef(source[3], AL_PITCH, 1.0f);
	alSourcef(source[3], AL_GAIN, 0.3f);
	alSourcefv(source[3], AL_POSITION, source3Pos);
	alSourcefv(source[3], AL_VELOCITY, source3Vel);
	alSourcei(source[3], AL_BUFFER, buffer[3]);
	alSourcei(source[3], AL_LOOPING, AL_FALSE);
	alSourcef(source[3], AL_MAX_DISTANCE, 500);
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
	shaderTerrain.destroy();
	shaderParticlesFountain.destroy();
	shaderParticlesFire.destroy();

	// Basic objects Delete
	skyboxSphere.destroy();
	boxCollider.destroy();
	sphereCollider.destroy();
	rayModel.destroy();
	boxViewDepth.destroy();
	boxLightViewBox.destroy();

	// - marroja
	for (auto& m : listaModelos) {
		m->destroy();
	}

	// Textures Delete
	glBindTexture(GL_TEXTURE_2D, 0);
	glDeleteTextures(1, &textureCespedID);
	glDeleteTextures(1, &textureWallID);
	glDeleteTextures(1, &textureWindowID);
	glDeleteTextures(1, &textureHighwayID);
	glDeleteTextures(1, &textureLandingPadID);
	glDeleteTextures(1, &textureTerrainBackgroundID);
	glDeleteTextures(1, &textureTerrainRID);
	glDeleteTextures(1, &textureTerrainGID);
	glDeleteTextures(1, &textureTerrainBID);
	glDeleteTextures(1, &textureTerrainBlendMapID);
	glDeleteTextures(1, &textureParticleFountainID);
	glDeleteTextures(1, &textureParticleFireID);

	// Cube Maps Delete
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDeleteTextures(1, &skyboxTextureID);

	// Remove the buffer of the fountain particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &initVel);
	glDeleteBuffers(1, &startTime);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticles);

	// Remove the buffer of the fire particles
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(2, posBuf);
	glDeleteBuffers(2, velBuf);
	glDeleteBuffers(2, age);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
	glDeleteTransformFeedbacks(2, feedback);
	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VAOParticlesFire);
}

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
	int mode) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_ESCAPE:
			exitApp = true;
			break;
		}
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;
	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
	distanceFromTarget -= yoffset;
	camera->setDistanceFromTarget(distanceFromTarget);
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
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

	if (!iniciaPartida) {
		bool statusEnter = glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS;
		if (textureActivaID == textureCespedID && statusEnter)
			iniciaPartida = true;
		if (!presionarOpcion && glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
			presionarOpcion = true;
			if (textureActivaID = textureCespedID)
				textureActivaID = textureWallID;
			else if (textureActivaID = textureWallID)
				textureActivaID = textureWindowID;
		}
		else if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE) {
			presionarOpcion = false;
		}
	}

	//para mover cualquier modelo seleccionable - marroja
	listaIndiceAnim[iModeloSelect] = 6;

	//Específica para control
	if (glfwJoystickPresent(GLFW_JOYSTICK_1) == GLFW_TRUE) { //glfwJoystickPresent(0)
		std::cout << "Esta conectado el Joystick" << std::endl;
		int numeroAxes, numeroBotones;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &numeroAxes);
		std::cout << "Numero de ejes :=>" << numeroAxes << std::endl;
		std::cout << "Axes[0]=>" << axes[0] << std::endl;
		std::cout << "Axes[1]=>" << axes[1] << std::endl;
		std::cout << "Axes[2]=>" << axes[2] << std::endl;
		std::cout << "Axes[3]=>" << axes[3] << std::endl;
		std::cout << "Axes[4]=>" << axes[4] << std::endl;
		std::cout << "Axes[5]=>" << axes[5] << std::endl;

		if (listaTipoObjeto[iModeloSelect] == 0) {

			if (fabs(axes[1]) > 0.2f) {
				if (axes[1] < 0)
					listaIndiceAnim[iModeloSelect] = 20;
				else
					listaIndiceAnim[iModeloSelect] = 21;
				*listaMat4[iModeloSelect] = glm::translate(*listaMat4[iModeloSelect], glm::vec3(0, 0, axes[1] * 0.25)); //* (*listaVecMov[iModeloSelect]).z));
			}

			if (fabs(axes[0]) > 0.2f) {
				if (axes[1] < 0)
					listaIndiceAnim[iModeloSelect] = 27;
				else
					listaIndiceAnim[iModeloSelect] = 28;
				*listaMat4[iModeloSelect] = glm::translate(*listaMat4[iModeloSelect], glm::vec3(-axes[1] * 0.25, 0, 0));
			}

			if (fabs(axes[2]) > 0.2f) {
				//camera->mouseMoveCamera(axes[2] * 0.5f, 0, deltaTime);
				*listaMat4[iModeloSelect] = glm::rotate(*listaMat4[iModeloSelect], glm::radians(axes[2] * 0.75f), glm::vec3(0.0f, 1.0f, 0.0f));
			}

			if (fabs(axes[3]) > 0.2f) {
				camera->mouseMoveCamera(0, -axes[3] * 0.5f, deltaTime);
			}

			const unsigned char* botones = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &numeroBotones);
			std::cout << "Numero de botones :=>" << numeroBotones << std::endl;

			if (botones[0] == GLFW_PRESS) {
				std::cout << "Se presiona el boton A :=>" << std::endl;
				listaIndiceAnim[iModeloSelect] = 5;
			}
			if (botones[1] == GLFW_PRESS) {
				std::cout << "Se presiona el boton B :=>" << std::endl;
				listaIndiceAnim[iModeloSelect] = 14;
			}
			if (botones[2] == GLFW_PRESS) {
				std::cout << "Se presiona el boton X :=>" << std::endl;
			}
			if (botones[3] == GLFW_PRESS) {
				std::cout << "Se presiona el boton Y :=>" << std::endl;
				listaIndiceAnim[iModeloSelect] = 19;
				*listaMat4[iModeloSelect] = glm::translate(*listaMat4[iModeloSelect], *listaVecMov[iModeloSelect]);
			}

			presiona = (botones[2] == GLFW_PRESS);
			suelta = (botones[2] == GLFW_RELEASE);
			keyJumpStatus = (botones[0] == GLFW_PRESS);
		}
	}


	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		camera->mouseMoveCamera(offsetX, 0.0, deltaTime);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		camera->mouseMoveCamera(0.0, offsetY, deltaTime);

	offsetX = 0;
	offsetY = 0;

	// Seleccionar modelo
	if (enableCountSelected && glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
		enableCountSelected = false;
		iModeloSelect++;
		if (iModeloSelect >= 2)
			iModeloSelect = 1;
		std::cout << "modelSelected:" << iModeloSelect << std::endl;
	}
	else if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
		enableCountSelected = true;

	if (listaTipoObjeto[iModeloSelect] == 0) { //|| listaTipoObjeto[iModeloSelect] == 1) {
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			*listaVecMov[iModeloSelect] = glm::vec3(0.0f, 0.0f, 0.1f);
			//listaIndiceAnim[iModeloSelect] = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			*listaVecMov[iModeloSelect] = glm::vec3(0.0f, 0.0f, -0.1f);
			//listaIndiceAnim[iModeloSelect] = 0;
		}
		else {
			*listaVecMov[iModeloSelect] = glm::vec3(0.0f, 0.0f, 0.0f);
			//listaIndiceAnim[iModeloSelect] = 0;
		}

			if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			*listaMat4[iModeloSelect] = glm::rotate(*listaMat4[iModeloSelect], 0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
			listaIndiceAnim[iModeloSelect] = 0;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			*listaMat4[iModeloSelect] = glm::rotate(*listaMat4[iModeloSelect], -0.02f, glm::vec3(0.0f, 1.0f, 0.0f));
			//listaIndiceAnim[iModeloSelect] = 0;
		}

		if (!proyectilGenerado) {
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS || presiona) {
				std::cout << "Se jaló proyectil (no generado)" << std::endl;
				proyectilGenerado = false;
				m4Flecha = glm::mat4(m4Arquero);
				m4Flecha[3][1] += 1.5;
				vecMovFlecha = glm::vec3(0.0f, 0.05f, 0.2f);
				listaIndiceAnim[iModeloSelect] = 16;
			}
			if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE || suelta) {
				//std::cout << "Se generó proyectil" << std::endl;
				proyectilGenerado = true;
				listaIndiceAnim[iModeloSelect] = 17;
			}
		}
		else {
			//*listaMat4[0] = glm::translate(*listaMat4[0], vecMovFlecha);
		}
	}

	bool keySpaceStatus = glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS;
	if (!isJump && keySpaceStatus || !isJump && keyJumpStatus) {
		isJump = true;
		startTimeJump = currTime;
		tmv = 0;
		sourcesPlay[3] = true;
	}


	glfwPollEvents();
	return continueApplication;
}

void applicationLoop() {
	bool psi = true;

	glm::mat4 view;
	glm::vec3 axis;
	glm::vec3 target;
	float angleTarget;

	//Crear aleatorio
	std::srand(unsigned(std::time(nullptr)));

	glm::vec3 random = glm::vec3(-139 + (std::rand() % 179), 0.0f, -139 + (std::rand() % 179));

	m4Arquero = glm::translate(m4Arquero, glm::vec3(34, 0, -14));//pos inicial
	m4Troll = glm::translate(m4Troll, glm::vec3(-4, 0, 4));//pos inicial
	m4TrollGrey = glm::translate(m4TrollGrey, random);
	m4TrollOrange = glm::translate(m4TrollOrange, random+glm::vec3(10.0f, 0.0f, 10.0f));
	m4OrkGreen = glm::translate(m4OrkGreen, random - glm::vec3(10.0f, 0.0f, 10.0f));
	m4OrkOrange = glm::translate(m4OrkOrange, random + glm::vec3(20.0f, 0.0f, -20.0f));
	m4OrkPink = glm::translate(m4OrkPink, random - glm::vec3(15.0f, 0.0f, 5.0f));

	m4ShootingRange = glm::translate(m4ShootingRange, glm::vec3(-25.0f, 0.0f, 25.0f));

	m4Fort = glm::translate(m4Fort, glm::vec3(50.0f, 0.0f, 25.0f));
	m4Fort = glm::rotate(m4Fort, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	
	m4HouseDestroyed = glm::translate(m4HouseDestroyed, glm::vec3(-30.0f, 0.0f, -10.0f));
	m4Debris = glm::translate(m4Debris, glm::vec3(-20.0f, 0.0f, -20.0f));


	lastTime = TimeManager::Instance().GetTime();

	// Time for the particles animation
	currTimeParticlesAnimation = lastTime;
	lastTimeParticlesAnimation = lastTime;

	currTimeParticlesAnimationFire = lastTime;
	lastTimeParticlesAnimationFire = lastTime;

	glm::vec3 lightPos = glm::vec3(10.0, 10.0, 0.0);
	shadowBox = new ShadowBox(lightPos, camera.get(), 15.0f, 0.01f, 45.0f);
	textureActivaID = textureCespedID;
	
	while (psi) {
		currTime = TimeManager::Instance().GetTime();
		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}
		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		psi = processInput(true);

		std::map<std::string, bool> collisionDetection;

		glm::mat4 projection = glm::perspective(glm::radians(45.0f),
			(float)screenWidth / (float)screenHeight, 0.1f, 100.0f);

		axis = glm::axis(glm::quat_cast(*(listaMat4[iModeloSelect])));
		angleTarget = glm::angle(glm::quat_cast(*(listaMat4[iModeloSelect])));
		target = (*listaMat4[iModeloSelect])[3];

		if (std::isnan(angleTarget))
			angleTarget = 0.0;
		if (axis.y < 0)
			angleTarget = -angleTarget;
		//if (iModeloSelect == 1)
		//	angleTarget -= glm::radians(90.0f);
		camera->setCameraTarget(target);
		camera->setAngleTarget(angleTarget);
		camera->updateCamera();
		view = camera->getViewMatrix();

		// Matriz de proyección del shadow mapping
		glm::mat4 lightProjection = glm::mat4(1.0f), lightView = glm::mat4(1.0f);
		glm::mat4 lightSpaceMatrix;
		shadowBox->update(screenWidth, screenHeight);
		glm::vec3 boxPosition = shadowBox->getCenter();
		lightView = glm::lookAt(boxPosition, boxPosition + glm::normalize(-lightPos), glm::vec3(0.0f, 1.0f, 0.0f));
		lightProjection[0][0] = 2.0f / shadowBox->getWidth();
		lightProjection[1][1] = 2.0f / shadowBox->getHeight();
		lightProjection[2][2] = -2.0f / shadowBox->getLength();
		lightProjection[3][3] = 1.0f;
		lightSpaceMatrix = lightProjection * lightView;
		shaderDepth.setMatrix4("lightSpaceMatrix", 1, false, glm::value_ptr(lightSpaceMatrix));

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
		shaderMulLighting.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader con multiples luces
		shaderTerrain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderTerrain.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		shaderTerrain.setMatrix4("lightSpaceMatrix", 1, false,
			glm::value_ptr(lightSpaceMatrix));
		// Settea la matriz de vista y projection al shader para el fountain
		shaderParticlesFountain.setMatrix4("projection", 1, false,
			glm::value_ptr(projection));
		shaderParticlesFountain.setMatrix4("view", 1, false,
			glm::value_ptr(view));
		// Settea la matriz de vista y projection al shader para el fuego
		shaderParticlesFire.setInt("Pass", 2);
		shaderParticlesFire.setMatrix4("projection", 1, false, glm::value_ptr(projection));
		shaderParticlesFire.setMatrix4("view", 1, false, glm::value_ptr(view));

		/*******************************************
		 * Propiedades de neblina
		 *******************************************/
		shaderMulLighting.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.266f, 0.3f, 0.635f))); //mas morado 0.635f, 0.553f, 0.71f
		shaderTerrain.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.266f, 0.3f, 0.635f)));
		shaderSkybox.setVectorFloat3("fogColor", glm::value_ptr(glm::vec3(0.266f, 0.3f, 0.635f)));

		/*******************************************
		 * Propiedades Luz direccional
		 *******************************************/
		shaderMulLighting.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderMulLighting.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderMulLighting.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderMulLighting.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades Luz direccional Terrain
		 *******************************************/
		shaderTerrain.setVectorFloat3("viewPos", glm::value_ptr(camera->getPosition()));
		shaderTerrain.setVectorFloat3("directionalLight.light.ambient", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.light.diffuse", glm::value_ptr(glm::vec3(0.5, 0.5, 0.5)));
		shaderTerrain.setVectorFloat3("directionalLight.light.specular", glm::value_ptr(glm::vec3(0.2, 0.2, 0.2)));
		shaderTerrain.setVectorFloat3("directionalLight.direction", glm::value_ptr(glm::vec3(-0.707106781, -0.707106781, 0.0)));

		/*******************************************
		 * Propiedades SpotLights
		 *******************************************/
		/*glm::vec3 spotPosition = glm::vec3(modelMatrixHeli * glm::vec4(0.32437, 0.226053, 1.79149, 1.0));
		shaderMulLighting.setInt("spotLightCount", 1);
		shaderTerrain.setInt("spotLightCount", 1);
		shaderMulLighting.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderMulLighting.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderMulLighting.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderMulLighting.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderMulLighting.setFloat("spotLights[0].constant", 1.0);
		shaderMulLighting.setFloat("spotLights[0].linear", 0.074);
		shaderMulLighting.setFloat("spotLights[0].quadratic", 0.03);
		shaderMulLighting.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderMulLighting.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.ambient", glm::value_ptr(glm::vec3(0.0, 0.0, 0.0)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.diffuse", glm::value_ptr(glm::vec3(0.2, 0.3, 0.2)));
		shaderTerrain.setVectorFloat3("spotLights[0].light.specular", glm::value_ptr(glm::vec3(0.3, 0.3, 0.3)));
		shaderTerrain.setVectorFloat3("spotLights[0].position", glm::value_ptr(spotPosition));
		shaderTerrain.setVectorFloat3("spotLights[0].direction", glm::value_ptr(glm::vec3(0, -1, 0)));
		shaderTerrain.setFloat("spotLights[0].constant", 1.0);
		shaderTerrain.setFloat("spotLights[0].linear", 0.074);
		shaderTerrain.setFloat("spotLights[0].quadratic", 0.03);
		shaderTerrain.setFloat("spotLights[0].cutOff", cos(glm::radians(12.5f)));
		shaderTerrain.setFloat("spotLights[0].outerCutOff", cos(glm::radians(15.0f)));*/

		/*******************************************
		 * Propiedades PointLights
		 *******************************************/
		shaderMulLighting.setInt("pointLightCount", lamp1Position.size() + lamp2Orientation.size());
		shaderTerrain.setInt("pointLightCount", lamp1Position.size() + lamp2Orientation.size());
		for (int i = 0; i < lamp1Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp1Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp1Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(0.5, 0.5, 0.5));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0, 10.3585, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.02);
		}
		for (int i = 0; i < lamp2Position.size(); i++) {
			glm::mat4 matrixAdjustLamp = glm::mat4(1.0f);
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, lamp2Position[i]);
			matrixAdjustLamp = glm::rotate(matrixAdjustLamp, glm::radians(lamp2Orientation[i]), glm::vec3(0, 1, 0));
			matrixAdjustLamp = glm::scale(matrixAdjustLamp, glm::vec3(1.0, 1.0, 1.0));
			matrixAdjustLamp = glm::translate(matrixAdjustLamp, glm::vec3(0.759521, 5.00174, 0));
			glm::vec3 lampPosition = glm::vec3(matrixAdjustLamp[3]);
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderMulLighting.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
			shaderMulLighting.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.01);
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.ambient", glm::value_ptr(glm::vec3(0.2, 0.16, 0.01)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.diffuse", glm::value_ptr(glm::vec3(0.4, 0.32, 0.02)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].light.specular", glm::value_ptr(glm::vec3(0.6, 0.58, 0.03)));
			shaderTerrain.setVectorFloat3("pointLights[" + std::to_string(lamp1Position.size() + i) + "].position", glm::value_ptr(lampPosition));
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].constant", 1.0);
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].linear", 0.09);
			shaderTerrain.setFloat("pointLights[" + std::to_string(lamp1Position.size() + i) + "].quadratic", 0.02);
		}

		/*if (!iniciaPartida) {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glViewport(0, 0, screenWidth, screenHeight);
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureActivaID);
			boxIntro.render();
			glfwSwapBuffers(window);
		}*/

		/*******************************************
		 * 1.- We render the depth buffer
		 *******************************************/
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// render scene from light's point of view
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		//glCullFace(GL_FRONT);
		prepareDepthScene();
		renderScene(false);
		//glCullFace(GL_BACK);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/*******************************************
		 * Debug to view the texture view map
		 *******************************************/
		 // reset viewport
		 /*glViewport(0, 0, screenWidth, screenHeight);
		 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		 // render Depth map to quad for visual debugging
		 shaderViewDepth.setMatrix4("projection", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 shaderViewDepth.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(1.0)));
		 glActiveTexture(GL_TEXTURE0);
		 glBindTexture(GL_TEXTURE_2D, depthMap);
		 boxViewDepth.setScale(glm::vec3(2.0, 2.0, 1.0));
		 boxViewDepth.render();*/

		 /*******************************************
		  * 2.- We render the normal objects
		  *******************************************/
		glViewport(0, 0, screenWidth, screenHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		prepareScene();
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		shaderMulLighting.setInt("shadowMap", 10);
		shaderTerrain.setInt("shadowMap", 10);
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
		renderScene();
		/*******************************************
		 * Debug to box light box
		 *******************************************/
		 /*glm::vec3 front = glm::normalize(-lightPos);
		 glm::vec3 right = glm::normalize(glm::cross(glm::vec3(0, 1, 0), front));
		 glm::vec3 up = glm::normalize(glm::cross(front, right));
		 glDisable(GL_CULL_FACE);
		 glm::mat4 boxViewTransform = glm::mat4(1.0f);
		 boxViewTransform = glm::translate(boxViewTransform, centerBox);
		 boxViewTransform[0] = glm::vec4(right, 0.0);
		 boxViewTransform[1] = glm::vec4(up, 0.0);
		 boxViewTransform[2] = glm::vec4(front, 0.0);
		 boxViewTransform = glm::scale(boxViewTransform, glm::vec3(shadowBox->getWidth(), shadowBox->getHeight(), shadowBox->getLength()));
		 boxLightViewBox.enableWireMode();
		 boxLightViewBox.render(boxViewTransform);
		 glEnable(GL_CULL_FACE);*/

		 /*******************************************
		  * Render de colliders
		  *******************************************/

		for (std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			collidersOBB.begin(); it != collidersOBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = matrixCollider * glm::mat4(std::get<0>(it->second).u);
			matrixCollider = glm::scale(matrixCollider, std::get<0>(it->second).e * 1.0f);
			boxCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			boxCollider.enableWireMode();
			//boxCollider.render(matrixCollider);
		}

		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			glm::mat4 matrixCollider = glm::mat4(1.0);
			matrixCollider = glm::translate(matrixCollider, std::get<0>(it->second).c);
			matrixCollider = glm::scale(matrixCollider, glm::vec3(std::get<0>(it->second).ratio * 1.0f));
			sphereCollider.setColor(glm::vec4(1.0, 1.0, 1.0, 1.0));
			sphereCollider.enableWireMode();
			//sphereCollider.render(matrixCollider);
		}  

		//----------------------Fin de RENDER modelos-------------------------------

		//----------------------Principio Colisionadores -------------------


		for (int i = 0; i < listaModelos.size(); i++) {
			glm::mat4 m4Collider = glm::mat4(*listaMat4[i]);

			//Se trunca el número de las unidades con la división y se revisa solo la decena
			if (listaTipoCollider[i] / 10 % 10 == 0) {
				//Detección de colsión SIN restricción
			}
			//Se trunca el número de las unidades con la división y se revisa solo la decena
			else if (listaTipoCollider[i] / 10 % 10 == 1) {
				//Detección de colisión y restricción de movimiento
			}

			if (listaTipoCollider[i] % 10 == 0) {//Sin geometría

			}
			else if (listaTipoCollider[i] % 10 == 1) {//SBB
				AbstractModel::SBB col;

				glm::vec3 escalaCollider = glm::vec3(1.0, 1.0, 1.0);
				m4Collider = glm::scale(m4Collider, escalaCollider);
				m4Collider = glm::translate(m4Collider, listaModelos[i]->getSbb().c);
				col.c = glm::vec3(m4Collider[3]);
				col.ratio = listaModelos[i]->getSbb().ratio * glm::length(*(listaEscalas[i]));
				addOrUpdateColliders(collidersSBB, std::to_string(i), col, *listaMat4[i]);		
			}
			else if (listaTipoCollider[i] % 10 == 2) {//OBB
				AbstractModel::OBB col;

				glm::vec3 escalaCollider = glm::vec3(1.0, 1.0, 1.0);
				m4Collider = glm::scale(m4Collider, escalaCollider);
				m4Collider = glm::translate(m4Collider, listaModelos[i]->getSbb().c);
				col.c = glm::vec3(m4Collider[3]);
				col.e = listaModelos[i]->getObb().e * glm::length(*(listaEscalas[i]));
				addOrUpdateColliders(collidersOBB, std::to_string(i), col, *listaMat4[i]);
			}
		}

		//*************************************************************
		//Detección de colisión
		//*************************************************************
		

		//*************************************************************
		//Detección de colisión
		//*************************************************************

		//colision entre esferas (SBB)
		for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> > ::iterator it =
			collidersSBB.begin(); it != collidersSBB.end(); it++) {
			bool isCollision = false;
			for (std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>> ::iterator
				jt = collidersSBB.begin(); jt != collidersSBB.end() && !isCollision; jt++) {

				int numModelo1 = stoi(it->first);
				int numModelo2 = stoi(jt->first);

				if (it != jt && testSphereSphereIntersection(std::get<0>(it->second), std::get<0>(jt->second))) {

					//PROYECTIL
					if (proyectilGenerado && (it->first._Equal("0") || jt->first._Equal("0"))) { //colision de flecha

						if ((listaTipoObjeto[numModelo1] == PROYECTIL && listaTipoObjeto[numModelo2] == JUGADOR) ||
							(listaTipoObjeto[numModelo2] == PROYECTIL && listaTipoObjeto[numModelo1] == JUGADOR)) {
							std::cout << "Existe colision entre jugador y flecha: " << it->first << " y " << jt->first << std::endl;
							continue;
						}
						else {

							std::cout << "Existe colision entre enemigo y flecha: " << it->first << " y " << jt->first << std::endl;
							flechaCollision = true;
							vecMovFlecha = glm::vec3(0.0f, 0.0f, 0.0f);

							if (listaTipoObjeto[numModelo1] == PROYECTIL) {
								destruyeEnemigo(numModelo2);
								destruyeFlecha(numModelo1);
							}
							else {
								destruyeEnemigo(numModelo1);
								destruyeFlecha(numModelo2);
							}

						}
					}
					//Colisión personaje-enemigo qph
					//std::cout << "Existe colision entre: " << it->first << " y " << jt->first << std::endl;

					isCollision = true;

				}
			}
			addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision); // para que no atraviese
		}

		std::map<std::string, bool>::iterator colIt;
		for (colIt = collisionDetection.begin(); colIt != collisionDetection.end();
			colIt++) {
			std::map<std::string,
				std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
				collidersSBB.find(colIt->first);
			std::map<std::string,
				std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				collidersOBB.find(colIt->first);
			if (it != collidersSBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersSBB, it->first);
			}
			if (jt != collidersOBB.end()) {
				if (!colIt->second)
					addOrUpdateColliders(collidersOBB, jt->first);
				else { // cuando hay colision
					for (int i = 0; i < listaModelos.size(); i++) {
						if (jt->first.compare(std::to_string(i)) == 0)
							*listaMat4[i] = std::get<1>(jt->second);
					}

				}
			}
		}
		/*
			Fin de prueba de colisiones
		*/

		actualizaMovimientos();

		/****************************+
		 * Open AL sound data
		 */
		//source0Pos[0] = modelMatrixFountain[3].x;
		//source0Pos[1] = modelMatrixFountain[3].y;
		//source0Pos[2] = modelMatrixFountain[3].z;
		//alSourcefv(source[0], AL_POSITION, source0Pos);

		//source2Pos[0] = modelMatrixDart[3].x;
		//source2Pos[1] = modelMatrixDart[3].y;
		//source2Pos[2] = modelMatrixDart[3].z;
		//alSourcefv(source[2], AL_POSITION, source2Pos);

		//source3Pos[0] = modelMatrixLambo[3].x;
		//source3Pos[1] = modelMatrixLambo[3].y;
		//source3Pos[2] = modelMatrixLambo[3].z;
		//alSourcefv(source[3], AL_POSITION, source3Pos);

		//// Listener for the Thris person camera
		//listenerPos[0] = modelMatrixMayow[3].x;
		//listenerPos[1] = modelMatrixMayow[3].y;
		//listenerPos[2] = modelMatrixMayow[3].z;
		//alListenerfv(AL_POSITION, listenerPos);

		//glm::vec3 upModel = glm::normalize(modelMatrixMayow[1]);
		//glm::vec3 frontModel = glm::normalize(modelMatrixMayow[2]);

		//listenerOri[0] = frontModel.x;
		//listenerOri[1] = frontModel.y;
		//listenerOri[2] = frontModel.z;
		//listenerOri[3] = upModel.x;
		//listenerOri[4] = upModel.y;
		//listenerOri[5] = upModel.z;

		 /*******************************************
		 * Test Colisions
		 *******************************************/
		 /*for (std::map<std::string,
			 std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator it =
			 collidersOBB.begin(); it != collidersOBB.end(); it++) {
			 bool isCollision = false;
			 for (std::map<std::string,
				 std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				 collidersOBB.begin(); jt != collidersOBB.end(); jt++) {
				 if (it != jt
					 && testOBBOBB(std::get<0>(it->second),
						 std::get<0>(jt->second))) {
					 std::cout << "Colision " << it->first << " with "
						 << jt->first << std::endl;
					 isCollision = true;
				 }
			 }
			 addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		 }

		 for (std::map<std::string,
			 std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			 collidersSBB.begin(); it != collidersSBB.end(); it++) {
			 bool isCollision = false;
			 for (std::map<std::string,
				 std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator jt =
				 collidersSBB.begin(); jt != collidersSBB.end(); jt++) {
				 if (it != jt
					 && testSphereSphereIntersection(std::get<0>(it->second),
						 std::get<0>(jt->second))) {
					 std::cout << "Colision " << it->first << " with "
						 << jt->first << std::endl;
					 isCollision = true;
				 }
			 }
			 addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		 }

		 for (std::map<std::string,
			 std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4> >::iterator it =
			 collidersSBB.begin(); it != collidersSBB.end(); it++) {
			 bool isCollision = false;
			 std::map<std::string,
				 std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4> >::iterator jt =
				 collidersOBB.begin();
			 for (; jt != collidersOBB.end(); jt++) {
				 if (testSphereOBox(std::get<0>(it->second),
					 std::get<0>(jt->second))) {
					 std::cout << "Colision " << it->first << " with "
						 << jt->first << std::endl;
					 isCollision = true;
					 addOrUpdateCollisionDetection(collisionDetection, jt->first, isCollision);
				 }
			 }
			 addOrUpdateCollisionDetection(collisionDetection, it->first, isCollision);
		 }*/

		glfwSwapBuffers(window);
	}
}

void prepareScene() {
	skyboxSphere.setShader(&shaderSkybox);
	terrain.setShader(&shaderTerrain);
	for (Model* x : listaModelos)
		x->setShader(&shaderMulLighting);
}

void prepareDepthScene() {
	skyboxSphere.setShader(&shaderDepth);
	terrain.setShader(&shaderDepth);
	for (Model* x: listaModelos)
		x->setShader(&shaderDepth);
}

void renderScene(bool renderParticles) {
	/*******************************************
	 * Terrain Cesped
	 *******************************************/
	glm::mat4 modelCesped = glm::mat4(1.0);
	modelCesped = glm::translate(modelCesped, glm::vec3(0.0, 0.0, 0.0));
	modelCesped = glm::scale(modelCesped, glm::vec3(200.0, 0.001, 200.0));
	// Se activa la textura del background
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBackgroundID);
	shaderTerrain.setInt("backgroundTexture", 0);
	// Se activa la textura de tierra
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureTerrainRID);
	shaderTerrain.setInt("rTexture", 1);
	// Se activa la textura de hierba
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureTerrainGID);
	shaderTerrain.setInt("gTexture", 2);
	// Se activa la textura del camino
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBID);
	shaderTerrain.setInt("bTexture", 3);
	// Se activa la textura del blend map
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureTerrainBlendMapID);
	shaderTerrain.setInt("blendMapTexture", 4);
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(40, 40)));
	terrain.render();
	shaderTerrain.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(0, 0)));
	glBindTexture(GL_TEXTURE_2D, 0);

	// Forze to enable the unit texture to 0 always ----------------- IMPORTANT
	glActiveTexture(GL_TEXTURE0);

	// Render the lamps
	/*for (int i = 0; i < lamp1Position.size(); i++) {
		lamp1Position[i].y = terrain.getHeightTerrain(lamp1Position[i].x, lamp1Position[i].z);
		modelLamp1.setPosition(lamp1Position[i]);
		modelLamp1.setScale(glm::vec3(0.5, 0.5, 0.5));
		modelLamp1.setOrientation(glm::vec3(0, lamp1Orientation[i], 0));
		modelLamp1.render();
	}

	for (int i = 0; i < lamp2Position.size(); i++) {
		lamp2Position[i].y = terrain.getHeightTerrain(lamp2Position[i].x, lamp2Position[i].z);
		modelLamp2.setPosition(lamp2Position[i]);
		modelLamp2.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLamp2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLamp2.render();
		modelLampPost2.setPosition(lamp2Position[i]);
		modelLampPost2.setScale(glm::vec3(1.0, 1.0, 1.0));
		modelLampPost2.setOrientation(glm::vec3(0, lamp2Orientation[i], 0));
		modelLampPost2.render();
	}*/

	//--------------------------------Render-----------------------------------
	
	//Detección de terreno para los modelos caminantes - marroja
	/*int contadorModelo = 0;
	for (auto mat4modelo : listaMat4) {
		if (listaTipoObjeto[contadorModelo] == 2) {
			break;
		}
		glm::vec3 ejeY = glm::normalize(terrain.getNormalTerrain((*mat4modelo)[3][0], (*mat4modelo)[3][2]));
		glm::vec3 ejeX = glm::normalize(glm::vec3((*mat4modelo)[0]));
		glm::vec3 ejeZ = glm::normalize(glm::cross(ejeX, ejeY));
		ejeX = glm::normalize(glm::cross(ejeY, ejeZ));

		(*mat4modelo)[0] = glm::vec4(ejeX, 0.0f);
		(*mat4modelo)[1] = glm::vec4(ejeY, 0.0f);
		(*mat4modelo)[2] = glm::vec4(ejeZ, 0.0f);

		(*mat4modelo)[3][1] = terrain.getHeightTerrain((*mat4modelo)[3][0], (*mat4modelo)[3][2]);
	}*/

	//Para el render de los modelos de manera escalada - marroja
	/*for (int i = 0; i < listaModelos.size(); i++) {
		glm::mat4 matCuerpoModelo = *listaMat4[i];
		matCuerpoModelo = glm::scale(matCuerpoModelo, *(listaEscalas[i]));
		(listaModelos[i])->setAnimationIndex(*(listaIndiceAnim[i]));
		(listaModelos[i])->render(matCuerpoModelo);
	}*/

	/*if (proyectilGenerado) {
		*listaMat4[0] = *listaMat4[iModeloSelect];
		//*listaMat4[0] = glm::translate(*listaMat4[0], glm::vec3((*listaMat4[iModeloSelect])[3]));
		(*listaMat4[0])[3][1] += 1.5f;
		*listaMat4[0] = glm::scale(*listaMat4[0], escalaFlecha);
		listaModelos[0]->render(*listaMat4[0]);
	}*/

	int i = 0;
	for (glm::mat4* matrixModel : listaMat4) {
		if (listaTipoObjeto[i] == 0) { // Es jugador
			if (i == iModeloSelect) {
				(*matrixModel)[3][1] = -GRAVITY * tmv * tmv + 3.5 * tmv + terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2]);
				tmv = currTime - startTimeJump;

				if ((*matrixModel)[3][1] < terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2])) {
					isJump = false;
					(*matrixModel)[3][1] = terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2]);
					alSourceStop(source[3]);
				}
			}
			else
				(*matrixModel)[3][1] = terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2]);
			glm::mat4 modelMatrixBody = glm::mat4(*matrixModel);
			modelMatrixBody = glm::scale(modelMatrixBody, *(listaEscalas[i]));
			listaModelos[i]->setAnimationIndex(listaIndiceAnim[i]);
			listaModelos[i]->render(modelMatrixBody);
		}
		else if (listaTipoObjeto[i] == 1) { // es enemigo
			(*matrixModel)[3][1] = terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2]);
			glm::mat4 modelMatrixBody = glm::mat4(*matrixModel);
			modelMatrixBody = glm::scale(modelMatrixBody, *(listaEscalas[i]));
			listaModelos[i]->setAnimationIndex(listaIndiceAnim[i]);
			listaModelos[i]->render(modelMatrixBody);
		}
		else if (listaTipoObjeto[i] == 2) { // es inanimado
			(*matrixModel)[3][1] = terrain.getHeightTerrain((*matrixModel)[3][0], (*matrixModel)[3][2]);
			glm::mat4 modelMatrixBody = glm::mat4(*matrixModel);
			modelMatrixBody = glm::scale(modelMatrixBody, *(listaEscalas[i]));
			listaModelos[i]->render(modelMatrixBody);
		}
		else if (listaTipoObjeto[i] == 3) { // es proyectil
			glm::mat4 modelMatrixBody = glm::mat4(*matrixModel);
			modelMatrixBody = glm::scale(modelMatrixBody, *(listaEscalas[i]));
			listaModelos[i]->render(modelMatrixBody);
		}
		i++;
	}

	/**********
	 * Update the position with alpha objects
	 */
	 // Update the aircraft
	//blendingUnsorted.find("aircraft")->second = glm::vec3(modelMatrixAircraft[3]);

	/**********
	 * Sorter with alpha objects
	 */
	std::map<float, std::pair<std::string, glm::vec3>> blendingSorted;
	std::map<std::string, glm::vec3>::iterator itblend;
	for (itblend = blendingUnsorted.begin(); itblend != blendingUnsorted.end(); itblend++) {
		float distanceFromView = glm::length(camera->getPosition() - itblend->second);
		blendingSorted[distanceFromView] = std::make_pair(itblend->first, itblend->second);
	}

	/**********
	 * Render de las transparencias
	 */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	for (std::map<float, std::pair<std::string, glm::vec3> >::reverse_iterator it = blendingSorted.rbegin(); it != blendingSorted.rend(); it++) {
		if (renderParticles && it->second.first.compare("fountain") == 0) {
			/**********
			 * Init Render particles systems
			 */
			glm::mat4 modelMatrixParticlesFountain = glm::mat4(1.0);
			modelMatrixParticlesFountain = glm::translate(modelMatrixParticlesFountain, it->second.second);
			modelMatrixParticlesFountain[3][1] = terrain.getHeightTerrain(modelMatrixParticlesFountain[3][0], modelMatrixParticlesFountain[3][2]) + 0.36 * 10.0;
			modelMatrixParticlesFountain = glm::scale(modelMatrixParticlesFountain, glm::vec3(3.0, 3.0, 3.0));
			currTimeParticlesAnimation = TimeManager::Instance().GetTime();
			if (currTimeParticlesAnimation - lastTimeParticlesAnimation > 10.0)
				lastTimeParticlesAnimation = currTimeParticlesAnimation;
			//glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);
			// Set the point size
			glPointSize(10.0f);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFountainID);
			shaderParticlesFountain.turnOn();
			shaderParticlesFountain.setFloat("Time", float(currTimeParticlesAnimation - lastTimeParticlesAnimation));
			shaderParticlesFountain.setFloat("ParticleLifetime", 3.5f);
			shaderParticlesFountain.setInt("ParticleTex", 0);
			shaderParticlesFountain.setVectorFloat3("Gravity", glm::value_ptr(glm::vec3(0.0f, -0.3f, 0.0f)));
			shaderParticlesFountain.setMatrix4("model", 1, false, glm::value_ptr(modelMatrixParticlesFountain));
			glBindVertexArray(VAOParticles);
			glDrawArrays(GL_POINTS, 0, nParticles);
			glDepthMask(GL_TRUE);
			//glEnable(GL_DEPTH_TEST);
			shaderParticlesFountain.turnOff();
			/**********
			 * End Render particles systems
			 */
		}
		else if (renderParticles && it->second.first.compare("fire") == 0) {
			/**********
			 * Init Render particles systems
			 */
			lastTimeParticlesAnimationFire = currTimeParticlesAnimationFire;
			currTimeParticlesAnimationFire = TimeManager::Instance().GetTime();

			shaderParticlesFire.setInt("Pass", 1);
			shaderParticlesFire.setFloat("Time", currTimeParticlesAnimationFire);
			shaderParticlesFire.setFloat("DeltaT", currTimeParticlesAnimationFire - lastTimeParticlesAnimationFire);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_1D, texId);
			glEnable(GL_RASTERIZER_DISCARD);
			glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, feedback[drawBuf]);
			glBeginTransformFeedback(GL_POINTS);
			glBindVertexArray(particleArray[1 - drawBuf]);
			glVertexAttribDivisor(0, 0);
			glVertexAttribDivisor(1, 0);
			glVertexAttribDivisor(2, 0);
			glDrawArrays(GL_POINTS, 0, nParticlesFire);
			glEndTransformFeedback();
			glDisable(GL_RASTERIZER_DISCARD);

			shaderParticlesFire.setInt("Pass", 2);
			glm::mat4 modelFireParticles = glm::mat4(1.0);
			modelFireParticles[3] = m4Debris[3]; //se cambia a posición de casa incendiada (debris)
			modelFireParticles = glm::translate(modelFireParticles, it->second.second);
			modelFireParticles[3][1] = terrain.getHeightTerrain(modelFireParticles[3][0], modelFireParticles[3][2]);
			shaderParticlesFire.setMatrix4("model", 1, false, glm::value_ptr(modelFireParticles));

			shaderParticlesFire.turnOn();
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureParticleFireID);
			glDepthMask(GL_FALSE);
			glBindVertexArray(particleArray[drawBuf]);
			glVertexAttribDivisor(0, 1);
			glVertexAttribDivisor(1, 1);
			glVertexAttribDivisor(2, 1);
			glDrawArraysInstanced(GL_TRIANGLES, 0, 6, nParticlesFire);
			glBindVertexArray(0);
			glDepthMask(GL_TRUE);
			drawBuf = 1 - drawBuf;
			shaderParticlesFire.turnOff();

			/****************************+
			 * Open AL sound data
			 */
			source1Pos[0] = modelFireParticles[3].x;
			source1Pos[1] = modelFireParticles[3].y;
			source1Pos[2] = modelFireParticles[3].z;
			alSourcefv(source[1], AL_POSITION, source1Pos);

			/**********
			 * End Render particles systems
			 */
		}
	}

}

void actualizaMovimientos() {

	animaMuerte();

	for (int i = 0; i <= listaMat4.size(); i++) {

		if (proyectilGenerado && listaTipoObjeto[i] == PROYECTIL) {

			(*listaVecMov[i])[1] -= 0.004;

			if ((*listaMat4[i])[3][1] <= 0) {
				destruyeFlecha(i);
			}

			(*listaMat4[i]) = glm::translate((*listaMat4[i]), (*listaVecMov[i]));

		}

		if (listaTipoObjeto[i] == ENEMIGO) {

			if (listaVivoOMuerto[i] == VIVO) {

				glm::vec3 dirResta = glm::vec3(m4Arquero[3].x - (*listaMat4[i])[3].x, 0.0f, m4Arquero[3].z - (*listaMat4[i])[3].z);
				dirResta = glm::normalize(dirResta);
				dirResta = 0.06f * dirResta;
				*(listaVecMov[i]) = glm::vec3(dirResta);
				*(listaMat4[i]) = glm::translate((*listaMat4[i]), (*listaVecMov[i]));

				(listaModelos[i])->setAnimationIndex(ANIM_TROLL_CORRE);
			}

		}

		if (listaTipoObjeto[i] == JUGADOR) {

			(*listaMat4[i]) = glm::translate((*listaMat4[i]), (*listaVecMov[i]));

		}

	}

}

void destruyeEnemigo(int numModelo) {

	std::cout << "Se destruye enemigo:" << numModelo << std::endl;
	*(listaVecMov[numModelo]) = glm::vec3(0.0f, 0.0f, 0.0f);
	listaVivoOMuerto[numModelo] = MUERTO;
}

void animaMuerte() {


	for (int i = 0; i < listaVivoOMuerto.size(); i++) {
		if (listaVivoOMuerto[i] == MUERTO) {
			if (listaTiempoAnim[i] < 1) {
				listaIndiceAnim[i] = ANIM_TROLL_MUERE; //índice de animación de muerte
				(listaModelos[i])->setAnimationIndex(ANIM_TROLL_MUERE);
				listaTiempoAnim[i] += 1.1;
			}
			//Tiempo muerte qpr
			else if (listaTiempoAnim[i] <= 107.0) {
				//Espera a que se reprodusca la animación del troll muerto
				listaTiempoAnim[i] += 1.0;
			}

			else {
				std::cout << "Renace el enemigo:" << i << std::endl;
				listaVivoOMuerto[i] = VIVO; //los enemigos muertes reviven pero lejos
				listaTiempoAnim[i] = 0.0;
				glm::vec3 random = glm::vec3(-139 + (std::rand() % 179), 0.0f, -139 + (std::rand() % 179));

				*(listaMat4[i]) = glm::translate(*(listaMat4[i]), random);
				listaIndiceAnim[i] = ANIM_TROLL_CORRE; //qpr el índice (que esté muerto)
				*listaVecMov[i] = glm::vec3(0.0f, 0.0f, 0.07f); //o la velocidad que se deseé (pueden ir creciendo conforme más se maten etc)
			}
		}

	}

}

void destruyeFlecha(int numModelo) {
	//a menos que se cambie, el modelo de la flecha siempre será el modelo [0]

	std::cout << "Se destruye la flecha:" << numModelo << std::endl;

	*(listaMat4[numModelo]) = glm::mat4(1.0f);
	glm::translate(*(listaMat4[numModelo]), glm::vec3(0.0f, -1000.0f, 0.0f));
	*(listaVecMov[numModelo]) = glm::vec3(0.0f);
	proyectilGenerado = false;
}

int main(int argc, char** argv) {
	init(800, 700, "Window GLFW", false);
	applicationLoop();
	destroy();
	return 1;
}

