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
#include <cstdlib>
#include <ctime>
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

#define CAMERA 2
#define PLANE 1
#define STOP 0

#define COLLISION_FREQUENCY 300
#define RENDER_FREQUENCY 20
GLuint STATUS;

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

// mouse callback function, used for wander around the scene
static void MouseMove(int, int);
static void ResetMouse(int state);


// a timer callback function for rendering
// the scene with stable frequency (20frames/sec)
static void RenderTimer(int timer_id);


// timer callback function for collision detect,
// it's costly, so do it with a lower frequency than render
static void CollisionDetect(int timer_id);

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

// the first function for initialize the game
// load model&shader&height map from assets.
static void init() {
	glGetError(); // for glew bug
	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	srand(time(0));
	STATUS = PLANE;
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	memset(Key, false, 256 * sizeof(bool));
	try {
		// init skybox
		string cubemap_pic_path[6] = { "./assets/skybox/right.jpg","./assets/skybox/left.jpg","./assets/skybox/top.jpg",
									"./assets/skybox/bottom.jpg","./assets/skybox/back.jpg","./assets/skybox/front.jpg" };
		skybox.LoadTexture(cubemap_pic_path, 6);
		skybox.DeployTexture();
		skybox.shader.LoadShader("./shaders/skybox_shader.vs", "./shaders/skybox_shader.frag");
		// init depth frame
		shadowMap.init();
		shadowMap.shader.LoadShader("./shaders/shadow_map_shader.vs", "./shaders/shadow_map_shader.frag");
		
		// init tree model
		Shader tree_shader;
		string tree_small_wrapper_path[2] = { "./assets/wrapper/Ctrl_tree_small_up.obj","./assets/wrapper/Ctrl_tree_down.obj"};
		string tree_big_wrapper_path[2] = { "./assets/wrapper/Ctrl_tree_big_up.obj","./assets/wrapper/Ctrl_tree_down.obj" };
		tree_shader.LoadShader("./shaders/tree_shader.vs", "./shaders/tree_shader.frag");
		for (unsigned int i = 0; i < 4; i++) {
			tree[i] = new StaticModel();
			tree[i]->shader = tree_shader;
			tree[i]->Load("./assets/tree/tree_low.obj", 1);
		}
		tree[0]->Load("./assets/tree/OC26_2.obj");
		tree[0]->LoadWrapper(tree_small_wrapper_path, 2);
		tree[1]->Load("./assets/tree/OC26_3.obj");
		tree[1]->LoadWrapper(tree_small_wrapper_path, 2);
		tree[2]->Load("./assets/tree/OC26_4.obj");
		tree[2]->LoadWrapper(tree_small_wrapper_path, 2);
		tree[3]->Load("./assets/tree/OC26_8.obj");
		tree[3]->LoadWrapper(tree_big_wrapper_path, 2);

		Shader mountain_shader;
		mountain_shader.LoadShader("./shaders/background_shader.vs", "./shaders/background_shader.frag");
		mountain = new BackGround();
		mountain->Load("./assets/terrain/mountains_4.obj");
		mountain->shader = mountain_shader;
		mountain->LoadHeightData("./assets/terrain/mountain.hmp");
		mountain->airport[0][0] = 465;
		mountain->airport[0][1] = 485;
		mountain->airport[1][0] = 550;
		mountain->airport[1][1] = 510;

		plane = new PlaneModel(vec3(935.0f, 0.0f, 1000.0f));
		string plane_wrapper_path[6] = { "./assets/wrapper/Ctrl_body.obj","./assets/wrapper/Ctrl_propeller.obj",
										"./assets/wrapper/Ctrl_tail.obj","./assets/wrapper/Ctrl_tail2.obj",
										"./assets/wrapper/Ctrl_wheel.obj","./assets/wrapper/Ctrl_wing.obj"};
		plane->Load("./assets/plane/Spowith.obj");
		plane->LoadWrapper(plane_wrapper_path, 6);
		plane->shader = mountain_shader;
		
		// assign the position of tree model
		scene.AppendObject(vec3(929.0f, 0, 1018.0f), tree[0]);
		scene.AppendObject(vec3(934.0f, 0, 1018.0f), tree[1]);
		scene.AppendObject(vec3(940.0f, 0, 1018.0f), tree[2]);
		scene.AppendObject(vec3(946.0f, 0, 1018.0f), tree[3]);
		scene.AppendObject(vec3(966.0f, 0, 1018.0f), tree[2]);
		scene.AppendObject(vec3(970.0f, 0, 1018.0f), tree[0]);
		scene.AppendObject(vec3(1200.0f, 5.0f, 990.0f), tree[1]);
		scene.AppendObject(vec3(1200.0f, 5.0f, 1000.0f), tree[2]);
		scene.background = mountain;
		scene.plane = plane;
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
	if (STATUS == CAMERA) {
		scene.Arrange(camera.front, camera.position);
	}
	else{
		scene.Arrange(plane->position - plane->View(), plane->View());
	}
		glViewport(0, 0, shadowMap.map_width, shadowMap.map_height);
		shadowMap.begRenderDirLight(sun.direction, 20.0f, plane->position);
			glClear(GL_DEPTH_BUFFER_BIT);
			scene.RenderFrame(shadowMap.shader);
		shadowMap.endRender();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, scene.window_width, scene.window_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(45.0f, (GLfloat)scene.window_width / (GLfloat)scene.window_height, 1.0f, 20.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		if (STATUS == CAMERA) {
			gluLookAt(camera.position.x(), camera.position.y(), camera.position.z(),
				camera.position.x() + camera.front.x(), camera.position.y() + camera.front.y(), 
				camera.position.z() + camera.front.z(), camera.up.x(), camera.up.y(), camera.up.z());
			skybox.Draw(camera.position.x(), camera.position.y(), camera.position.z());
		}
		else{
			gluLookAt(plane->View().x(), plane->View().y(), plane->View().z(),
				plane->position.x(), plane->position.y(), plane->position.z(),
				0.0f, 1.0f, 0.0f);
			skybox.Draw(plane->View().x(), plane->View().y(), plane->View().z());
		}
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
	glutEntryFunc(ResetMouse);
	glutTimerFunc(RENDER_FREQUENCY, RenderTimer, 0);
	glutTimerFunc(COLLISION_FREQUENCY, CollisionDetect, 0);
	glutMainLoop();
}

static void RenderTimer(int timer_id) {
	if (STATUS == STOP) {
		if (Key['0']) {
			STATUS = PLANE;
			plane->init();
		}
	}
	else if (STATUS == PLANE) {
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
		if (Key['Q'] || Key['q'] || Key['E'] || Key['e']) {
			if (Key['Q'] || Key['q']) {
				plane->YawLeft();
			}
			if (Key['E'] || Key['e']) {
				plane->YawRight();
			}
		}
		if (Key['1']) {
			plane->SpeedUp();
		}
		if (Key['2']) {
			plane->SpeedDown();
		}

		if (Key['C'] || Key['c']) {
			STATUS = CAMERA;
		}
		else {
			STATUS = PLANE;
		}
		plane->Forward();
	}
	else if (STATUS == CAMERA) {
		if (Key['W'] || Key['w']) {
			camera.movefront();
		}
		if (Key['S'] || Key['s']) {
			camera.moveback();
		}
		if (Key['A'] || Key['a']) {
			camera.moveleft();
		}
		if (Key['D'] || Key['d']) {
			camera.moveright();
		}
		if (Key['C'] || Key['c']) {
			STATUS = CAMERA;
		}
		else {
			STATUS = PLANE;
		}
	}
	if (STATUS != STOP) {
		glutPostRedisplay();
	}
	glutTimerFunc(RENDER_FREQUENCY, RenderTimer, 0);
}

static void CollisionDetect(int timer_id) {
	if (STATUS != STOP) {
		try {
			scene.CheckCollision();
		}
		catch (const Collision & e) {
			plane->is_crash = true;
			glutPostRedisplay();
			STATUS = STOP;
		}
		catch (const WarningBoard & e) {

		}
		catch (const ReachBoard & e) {
			plane->init();
		}
	}
	glutTimerFunc(COLLISION_FREQUENCY, CollisionDetect, 0);
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
}

static void ResetMouse(int state) {
	glutWarpPointer(scene.window_width / 2, scene.window_height / 2);
	last_mouse_x = scene.window_width / 2;
	last_mouse_y = scene.window_height / 2;
}
