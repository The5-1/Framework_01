#define GLEW_STATIC //Using the static lib, so we need to enable it
#include <iostream>
#include <GL/glew.h>
#include <GL/glut.h>
#include <Ant/AntTweakBar.h>
#include <memory>
#include <algorithm>
#include "helper.h"
#include "Shader.h"
#include "Skybox.h"
#include "times.h"
#include "InstancedMesh.h"
#include "FBO.h"
#include "Texture.h"
#include "glm/gtx/string_cast.hpp"
#include "marchingCubesVolume.h"

#include "HalfEdgeMesh.h"
#include "MeshResampler.h"

//Time
Timer timer;
int frame;
long timeCounter, timebase;
char timeString[50];

//Resolution (has to be changed in helper.h too)
glm::vec2 resolution = glm::vec2(1024, 768);

//Externals
cameraSystem cam(1.0f, 1.0f, glm::vec3(20.95f, 20.95f, -0.6f));
glm::mat4 projMatrix;
glm::mat4 viewMatrix;

//Skybox
Skybox skybox;
char* negz = "D:/Dev/Assets/SkyboxTextures/Yokohama2/negz.jpg";
char* posz = "D:/Dev/Assets/SkyboxTextures/Yokohama2/posz.jpg";
char* posy = "D:/Dev/Assets/SkyboxTextures/Yokohama2/posy.jpg";
char* negy = "D:/Dev/Assets/SkyboxTextures/Yokohama2/negy.jpg";
char* negx = "D:/Dev/Assets/SkyboxTextures/Yokohama2/negx.jpg";
char* posx = "D:/Dev/Assets/SkyboxTextures/Yokohama2/posx.jpg";

//Shaders
Shader textureShader;
Shader basicShader;
Shader modelLoaderShader;
Shader phongShader;

//Skybox
Shader skyboxShader;

//Models
simpleModel *teaPot = 0;

//Material Properties Variable
glm::vec3 uAmbient = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 uDiffuse = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 uSpecular = glm::vec3(0.5f, 0.5f, 0.5f);
float uShininess = 32.0f;

// tweak bar
TwBar *tweakBar;
bool wireFrameTeapot = false;
/* *********************************************************************************************************
TweakBar
********************************************************************************************************* */
void setupTweakBar() {
	TwInit(TW_OPENGL_CORE, NULL);
	tweakBar = TwNewBar("Settings");
	TwAddSeparator(tweakBar, "Wireframe", nullptr);
	TwAddVarRW(tweakBar, "Wireframe Teapot", TW_TYPE_BOOLCPP, &wireFrameTeapot, " label='Wireframe Teapot' ");

	TwAddVarRW(tweakBar, "uAmbient", TW_TYPE_DIR3F, &uAmbient, " label='uAmbient' ");
	TwAddVarRW(tweakBar, "uDiffuse", TW_TYPE_DIR3F, &uDiffuse, " label='uDiffuse' ");
	TwAddVarRW(tweakBar, "uSpecular", TW_TYPE_DIR3F, &uSpecular, " label='uSpecular' ");
	TwAddVarRW(tweakBar, "uShininess", TW_TYPE_FLOAT, &uShininess, " label='uShininess' ");
}

/* *********************************************************************************************************
Initiation
This will be only used ONCE!!!

Send all models/textures to the GPU ONCE!!!
********************************************************************************************************* */
unsigned int VBO, VAO, EBO;
unsigned int texture;
int width, height, nrChannels;
//Transformations Variablen
/*
glm::vec4 transVec(1.0f, 0.0f, 0.0f, 1.0f);
glm::mat4 transMat = glm::mat4(glm::translate(transMat, glm::vec3(1.0f, 1.0f, 0.0f)));
*/

void init() {
	
	teaPot = new simpleModel("D:/Dev/Assets/Teapot/teapot.obj", true);
	teaPot->upload();

	/****************************************************************
	T First Triangle
	****************************************************************/
	float vertices[] = {
		// positions          // colors           // texture coords
		0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
		0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
		-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
		-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// load and create a texture 
	// -------------------------
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
										   // set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load image, create texture and generate mipmaps
	// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
	unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
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



	/////*
	//float vertices[] = {
	//	// positions          // colors           // texture coords
	//	0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
	//	0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
	//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	//};

	//unsigned int indices[] = {
	//	0, 1, 3, // first triangle
	//	1, 2, 3  // second triangle
	//};
	//
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//glGenBuffers(1, &EBO);

	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	////With texture we have to change to the new vertex format
	////glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	////glEnableVertexAttribArray(0);
	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);
	//// texture coord attribute
	//glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	//glEnableVertexAttribArray(2);

	////Texture
	//glGenTextures(1, &texture);
	//glBindTexture(GL_TEXTURE_2D, texture);

	////Repeating outside of the texture
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	////Texture Filtering
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	////loading the texture
	//unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
	////generating Texture

	//if (data)
	//{
	//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	//	glGenerateMipmap(GL_TEXTURE_2D);
	//}
	//else
	//{
	//	std::cout << "Failed to load texture" << std::endl;
	//}
	//stbi_image_free(data);

	////Texture END

	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glBindVertexArray(VAO);

	//glBindVertexArray(0);

	/*****************************************************************
	Skybox (Only for aesthetic reasons, can be deleted)
	*****************************************************************/
	skybox.createSkybox(negz, posz, posy, negy, negx, posx);



}

void loadShader(bool init) {
	textureShader = Shader("./shader/texture.vs.glsl", "./shader/texture.fs.glsl");
	basicShader = Shader("./shader/standard.vs.glsl", "./shader/standard.fs.glsl");
	modelLoaderShader = Shader("./shader/modelLoader.vs.glsl", "./shader/modelLoader.fs.glsl");
	skyboxShader = Shader("./shader/skybox.vs.glsl", "./shader/skybox.fs.glsl");
	phongShader = Shader("./shader/phong.vs.glsl", "./shader/phong.fs.glsl");
}

/* *********************************************************************************************************
Scenes: Unit cube + Pointcloud, Results of marching cubes
Drawing Loop
********************************************************************************************************* */
void standardScene(){
	skyboxShader.enable();
	skyboxShader.uniform("projMatrix", projMatrix);
	skyboxShader.uniform("viewMatrix", cam.cameraRotation);
	skybox.Draw(skyboxShader);
	skyboxShader.disable();

	

	/* ********************************************
	Half-Edge-Mesh
	**********************************************/

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glm::mat4 modelMatrix;

	if (wireFrameTeapot) {
		glPolygonMode(GL_FRONT, GL_LINE);
		glPolygonMode(GL_BACK, GL_LINE);
	}
	
	//TeaPot mit Phongshader zeichnen
	phongShader.enable();

	/*Material Propeties*/
	phongShader.uniform("material.ambient", uAmbient);
	phongShader.uniform("material.diffuse", uDiffuse);
	phongShader.uniform("material.specular", uSpecular);
	phongShader.uniform("material.shininess", uShininess);

	phongShader.uniform("col", glm::vec3(1.0f, 1.0f, 0.0f));
	phongShader.uniform("projMatrix", projMatrix);
	phongShader.uniform("viewMatrix", viewMatrix);
	//glm::vec3 macht aus einem vec4 einen vec3, es werden nur die ersten 3 werte genommen
	phongShader.uniform("cameraPosition", glm::vec3(cam.position));
	phongShader.uniform("cameraDirection", glm::vec3(cam.viewDir));
	//Light Position
	phongShader.uniform("lightPos", glm::vec3(3.0f, 1.0f, 2.0f));

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f));
	phongShader.uniform("modelMatrix", modelMatrix);
	
	teaPot->draw();

	phongShader.disable();
	

	//Mein Triangle mit dem Basic Shader
	textureShader.enable();
	textureShader.uniform("col", glm::vec3(1.0f, 1.0f, 0.0f));
	textureShader.uniform("projMatrix", projMatrix);
	textureShader.uniform("viewMatrix", viewMatrix);
	//glm::vec3 macht aus einem vec4 einen vec3, es werden nur die ersten 3 werte genommen
	textureShader.uniform("cameraPosition", glm::vec3(cam.position));
	textureShader.uniform("cameraDirection", glm::vec3(cam.viewDir));
	//Translation Matrix den Shader zur verfügung stellen
	//textureShader.uniform("transMat", transMat);

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 10.0f, 0.0f));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(5.0f));
	textureShader.uniform("modelMatrix", modelMatrix);
	
	//Leuchtender Teapot
	//teaPot->draw();

	glBindTexture(GL_TEXTURE_2D, texture);
	glBindVertexArray(VAO); 							
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	/****************************************************************
	T First Triangle End
	****************************************************************/
	
	textureShader.disable();
	

	if (wireFrameTeapot) {
		glPolygonMode(GL_FRONT, GL_FILL);
		glPolygonMode(GL_BACK, GL_FILL);
	}
}	


/* *********************************************************************************************************
Display + Main
********************************************************************************************************* */
void display() {
	//Timer
	timer.update();
	//FPS-Counter
	frame++;
	timeCounter = glutGet(GLUT_ELAPSED_TIME);
	if (timeCounter - timebase > 1000) {
		sprintf_s(timeString, "FPS:%4.2f", frame*1000.0 / (timeCounter - timebase));
		timebase = timeCounter;
		frame = 0;
		glutSetWindowTitle(timeString);
	}

	//OpenGL Clears
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);
	glClearColor(0.2f, 0.2f, 0.2f, 1);
	
	standardScene();

	TwDraw(); //Draw Tweak-Bar

	glutSwapBuffers();

	glutPostRedisplay();

}

int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitWindowSize(WIDTH, HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE | GLUT_STENCIL);

	glutCreateWindow("Basic Framework");

	setupTweakBar();

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		std::cerr << "Error : " << glewGetErrorString(err) << std::endl;
	}

	glutDisplayFunc(display); //ToDo: Wie soll die Szene aussehen

	glutKeyboardFunc(keyboard);
	glutMotionFunc(onMouseMove);
	glutMouseFunc(onMouseDown);
	glutReshapeFunc(reshape);
	glutIdleFunc(onIdle);

	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	TwGLUTModifiersFunc(glutGetModifiers);

	initGL(); //Advanced: OpenGL Einstellungen

	init(); //ToDo: Sende Infos an Grafikkarte

	glutMainLoop(); //Zeichnet pro frame

	TwTerminate();

	return 0;
}










