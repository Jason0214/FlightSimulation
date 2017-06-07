#define GLEW_STATIC 
#include <glew.h>
#pragma comment (lib, "glew32s.lib")
#define FREEGLUT_STATIC
#include <GL\freeglut.h>


#ifndef GL_Lib
	#ifdef _DEBUG
		#define GL_Lib(name) name "d.lib"
	#else
		#define GL_Lib(name) name ".lib"
	#endif
#endif


#include <iostream>
#include <cstring>
#include "Exception.h"
#include "Skybox.h"
#include "Shader.h"
#include "Camera.h"
#include "Scene.h"
#include "Model.h"
#include "DepthMap.h"
#include "LightSrc.h"
#include "Plane.h"

using namespace std;


Camera camera(923.0f, 50.0f, 1000.0f);
SkyBox skybox;
Scene scene(2); //multi frustum
LightSrc sun(vec3(0.5f, 0.5f, 0.5f));
DepthMap shadowMap(1024, 1024); //TODO
PlaneModel* plane;
StaticModel* tree[4];
BackGround* mountain;
bool Key[256];
bool SpecialKey[1 << 8];

static GLint last_mouse_x;
static GLint last_mouse_y;


static void ChangeSize(int width, int height);
static void MouseMove(int, int);
static void ResetMouse(int state);
static void Movement(int);

static void KeyPress(unsigned char key, int x, int y) {
	Key[key] = true;
}
static void KeyRelease(unsigned char key, int x, int y) {
	Key[key] = false;
}
static void SpecialKeyPress(int key, int x, int y) {
	SpecialKey[key] = true;
}
static void SpecialKeyRelease(int key, int x, int y) {
	SpecialKey[key] = false;
}

static void init() {
	glGetError(); // for glew bug
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	memset(Key, false, 256 * sizeof(bool));
	const char* cubemap_pic_path[6] = { "./assets/skybox/right.jpg","./assets/skybox/left.jpg","./assets/skybox/top.jpg","./assets/skybox/bottom.jpg","./assets/skybox/back.jpg","./assets/skybox/front.jpg" };
	try {
		// init skybox
		skybox.LoadTexture(cubemap_pic_path, 6);
		skybox.DeployTexture();
		skybox.shader.LoadShader("./shaders/skybox_shader.vs", "./shaders/skybox_shader.frag");
		// init tree model
		Shader tree_shader;
		Shader mountain_shader;
		tree_shader.LoadShader("./shaders/tree_shader.vs", "./shaders/tree_shader.frag");
		mountain_shader.LoadShader("./shaders/background_shader.vs", "./shaders/background_shader.frag");
		for (unsigned int i = 0; i < 4; i++) {
			tree[i] = new StaticModel();
			tree[i]->shader = tree_shader;
		}
		tree[0]->Load("./assets/tree/OC26_2.obj");
		tree[1]->Load("./assets/tree/OC26_3.obj");
		tree[2]->Load("./assets/tree/OC26_4.obj");
		tree[3]->Load("./assets/tree/OC26_8.obj");
		mountain = new BackGround();
		mountain->Load("./assets/terrain/mountains_4.obj");
		mountain->shader = mountain_shader;
		mountain->LoadHeightData("./assets/terrain/mountain.hmp");
		plane = new PlaneModel(vec3(923.0f, 0.0f, 1000.0f));
		plane->Load("./assets/plane/Spowith.obj");
		plane->shader = mountain_shader;
		// assign the position of tree model
		scene.AppendObject(vec3(929.0f, 0, 1018.0f), tree[0] ,vec3(1.0f,0.0f,0.0f), -90.0f);
		scene.AppendObject(vec3(934, 0, 1018.0f), tree[1] ,vec3(1.0f, 0.0f, 0.0f), -90.0f);
		scene.AppendObject(vec3(940, 0, 1018.0f), tree[2] ,vec3(1.0f, 0.0f, 0.0f), -90.0f);
		scene.AppendObject(vec3(946, 0, 1018.0f), tree[3] ,vec3(1.0f, 0.0f, 0.0f), -90.0f);
		scene.background = mountain;
		scene.plane = plane;
		// init depth frame
		shadowMap.init();
		shadowMap.shader.LoadShader("./shaders/shadow_map_shader.vs", "./shaders/shadow_map_shader.frag");

//		shadowMap.test_init();
//		shadowMap.test_shader.LoadShader("./shaders/test_shader.vs", "./shaders/test_shader.frag");
	}
	catch (const LoadFileError & e) {
		cout << e.Info();
	}
	catch (const LoadModelError & e) {
		cout << e.Info();
	}
	catch (const ShaderCompileError & e) {
		cout << e.Info();
	}
	catch (const Exception & e) {
		cout << e.Info();
	}
	catch (const exception & e) {
		cout << e.what();
	}
}

static void Display() {
//	scene.Arrange(camera.front, camera.position);
	scene.Arrange(plane->position - plane->View(), plane->View());
		glViewport(0, 0, shadowMap.map_width, shadowMap.map_height);
		shadowMap.begRenderDirLight(sun.direction, 20.0f, plane->position);
			glClear(GL_DEPTH_BUFFER_BIT);
			scene.RenderFrame(shadowMap.shader);
		shadowMap.endRender();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, scene.window_width, scene.window_height);
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		/*gluLookAt(camera.position.x(), camera.position.y(), camera.position.z(), 
		camera.position.x() + camera.front.x(), camera.position.y() + camera.front.y(), camera.position.z() + camera.front.z(),
							camera.up.x(),camera.up.y(),camera.up.z());*/
		gluLookAt(plane->View().x(), plane->View().y(), plane->View().z(),
			plane->position.x(), plane->position.y(), plane->position.z(),
			0.0f, 1.0f, 0.0f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)scene.window_width / (GLfloat)scene.window_height,1.0f,20.0f);
//		skybox.Draw(camera.position.x(), camera.position.y(), camera.position.z());
		skybox.Draw(plane->View().x(), plane->View().y(), plane->View().z());
		scene.RenderAll(sun, shadowMap);
	scene.ResetArrange();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE );
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Mountain");
	glewExperimental = GL_TRUE;
	glewInit();
	//
	init();
	glutDisplayFunc(Display);
	glutReshapeFunc(ChangeSize);
	glutKeyboardFunc(KeyPress);
	glutKeyboardUpFunc(KeyRelease);
	glutSpecialFunc(SpecialKeyPress);
	glutSpecialUpFunc(SpecialKeyRelease);
	glutPassiveMotionFunc(MouseMove);
//	glutEntryFunc(ResetMouse);
	glutTimerFunc(50, Movement, 1);
	glutMainLoop();
}

static void Movement(int timer_id) {
	if (Key['W'] || Key['w']) {
		plane->PitchUp();
	}
	if (Key['S'] || Key['s']) {
		plane->PitchDown();
	}
	if (Key['A'] || Key['a'] || Key['D'] || Key['d']) {
		if (Key['A'] || Key['a']) {
			plane->RollLeft();
		}
		if (Key['D'] || Key['d']) {
			plane->RollRight();
		}
	}
	else {
		plane->RollBack();
	}
	if (Key['Q'] || Key['q'] || Key['E'] || Key['e']) {
		if (Key['Q'] || Key['q']) {
			plane->YawLeft();
		}
		if (Key['E'] || Key['e']) {
			plane->YawRight();
		}
	}
	else {
		plane->YawBack();
	}

	if (Key['1']) {
		plane->SpeedUp();
	}
	if (Key['2']) {
		plane->SpeedDown();
	}
	plane->Forward();
	glutPostRedisplay();
	glutTimerFunc(20, Movement, 1);
}

static void ChangeSize(int width, int height) {
	scene.window_height = height;
	scene.window_width = width;
	glViewport(0, 0, (GLuint)width, (GLuint)height);
//re init mouse
	glutSetCursor(GLUT_CURSOR_NONE);
	ResetMouse(0);
}

static void MouseMove(int x, int y) {
	const static GLfloat DEGREE_PER_PIXEL_MOVED_BY_MOUSE = 0.2f;
	camera.yaw += (x - last_mouse_x) * DEGREE_PER_PIXEL_MOVED_BY_MOUSE;
	camera.pitch += (last_mouse_y - y) * DEGREE_PER_PIXEL_MOVED_BY_MOUSE;
	camera.reposition();
	if (x < 50 || x > scene.window_width - 50 || y < 50 || y > scene.window_height - 50) {
		ResetMouse(0);
	}
	else {
		last_mouse_x = x;
		last_mouse_y = y;
	}
	glutPostRedisplay();
}

static void ResetMouse(int state) {
	glutWarpPointer(scene.window_width / 2, scene.window_height / 2);
	last_mouse_x = scene.window_width / 2;
	last_mouse_y = scene.window_height / 2;
}
