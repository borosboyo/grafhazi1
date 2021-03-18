//=============================================================================================
// Mintaprogram: Zöld háromszög. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!! 
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak 
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : Boros Gergõ
// Neptun : IGMEF9
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"


class InstantRender : public GPUProgram {

	// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
	const char* const vertexSource = R"(
		#version 330				// Shader 3.3
		precision highp float;		// normal floats, makes no difference on desktop computers

		uniform mat4 MVP;			// uniform variable, the Model-View-Projection transformation matrix
		layout(location = 0) in vec2 vp;	// Varying input: vp = vertex position is expected in attrib array 0

		void main() {
			gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;		// transform vp from modeling space to normalized device space
		}
	)";

	// fragment shader in GLSL
	const char* const fragmentSource = R"(
		#version 330			// Shader 3.3
		precision highp float;	// normal floats, makes no difference on desktop computers
	
		uniform vec3 color;		// uniform variable, the color of the primitive
		out vec4 outColor;		// computed color of the current pixel

		void main() {
			outColor = vec4(color, 1);	// computed color is the color of the primitive
		}
	)";

	unsigned int vao, vbo;


public:
	
	InstantRender() {
		glViewport(0, 0, windowWidth, windowHeight);
		glLineWidth(2.0f); glPointSize(10.0f);

		create(vertexSource, fragmentSource, "outColor");
		glGenVertexArrays(1, &vao); glBindVertexArray(vao);
		glGenBuffers(1, &vbo); 		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);  // attribute array 0
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), NULL);
	}


	~InstantRender() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}


	void DrawGPU(int type, std::vector<vec2> vertices, vec3 color) {
		setUniform(color, "color");
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2), &vertices[0], GL_DYNAMIC_DRAW);
		glDrawArrays(type, 0, vertices.size());
	}

	unsigned int getVao() {
		return vao;
	}
};

std::vector<vec3> graphVertices;
std::vector<vec3> graphLines;
std::vector<vec2> viewGraph;

class Graph {


public:
	Graph() {
		int temp = 0;
		float x;
		float y;
		float w;
		while (temp != 50) {
			float randX = (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
			float randY = (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
			if (randX * randX + randY * randY <= 1) {
				viewGraph.push_back(vec2(randX, randY));
				x = randX / sqrtf(1.0f - randX * randX - randY * randY);
				y = randY / sqrtf(1.0f - randX * randX - randY * randY);
				w = 1.0f / sqrtf(1.0f - randX * randX - randY * randY);
				graphVertices.push_back(vec3(x, y, w));
				temp++;
			}
		}
	}

};

InstantRender* renderer;
Graph* graph;
const int nTessV = 30;
std::vector<vec2> circlePoints, userPoints;


// Initialization, create an OpenGL context
void onInitialization() {
	renderer = new InstantRender();
	for (int ii = 0; ii < nTessV; ii++) {
		float fi = ii * 2 *M_PI / nTessV * 1.2;
		circlePoints.push_back(vec2(cosf(fi), sinf(fi)));
	}
	graph = new Graph();

}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);     // background color
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear frame buffer

	// Set color to (0, 1, 0) = green
	int location = glGetUniformLocation(renderer->getId(), "color");
	glUniform3f(location, 0.0f, 1.0f, 0.0f); // 3 floats

	float MVPtransf[4][4] = { 1, 0, 0, 0,    // MVP matrix, 
							  0, 1, 0, 0,    // row-major!
							  0, 0, 1, 0,
							  0, 0, 0, 1 };

	location = glGetUniformLocation(renderer->getId(), "MVP");	// Get the GPU location of uniform variable MVP
	glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);	// Load a 4x4 row-major float matrix to the specified location

	glBindVertexArray(renderer->getVao());  // Draw call
	renderer->DrawGPU(GL_TRIANGLE_FAN, circlePoints, vec3(0.5f, 0.5f, 0.5f));
	renderer->DrawGPU(GL_POINTS, viewGraph, vec3(1, 0, 0));
	//renderer->DrawGPU(GL_LINES, userPoints, vec3(1, 0.8f, 0.0f));
	glutSwapBuffers(); // exchange buffers for double buffering
}

// Key of ASCII code pressed
void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();         // if d, invalidate display, i.e. redraw
}

// Key of ASCII code released
void onKeyboardUp(unsigned char key, int pX, int pY) {
}

// Move mouse with key pressed
void onMouseMotion(int pX, int pY) {	// pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}

// Mouse click event
void onMouse(int button, int state, int pX, int pY) { // pX, pY are the pixel coordinates of the cursor in the coordinate system of the operation system
	// Convert to normalized device space
	float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
	float cY = 1.0f - 2.0f * pY / windowHeight;

	char* buttonStat;
	switch (state) {
	case GLUT_DOWN: buttonStat = "pressed"; break;
	case GLUT_UP:   buttonStat = "released"; break;
	}

	switch (button) {
	case GLUT_LEFT_BUTTON:   printf("Left button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY); break;
	case GLUT_MIDDLE_BUTTON: printf("Middle button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY); break;
	case GLUT_RIGHT_BUTTON:  printf("Right button %s at (%3.2f, %3.2f)\n", buttonStat, cX, cY);  break;



	}


	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {  // GLUT_LEFT_BUTTON / GLUT_RIGHT_BUTTON and GLUT_DOWN / GLUT_UP
		printf("px: %d, py: %d\n", pX, pY);
		float cX = 2.0f * pX / windowWidth - 1;	// flip y axis
		float cY = 1.0f - 2.0f * pY / windowHeight;
		if (cX * cX + cY * cY >= 1) return;
		userPoints.push_back(vec2(cX, cY));
		int n = userPoints.size() - 1;
		glutPostRedisplay();     // redraw
	}


	float x = cX / sqrtf(1.0f - cX * cX - cY * cY);
	float y = cY / sqrtf(1.0f - cX * cX - cY * cY);;
	float w = 1.0f / sqrtf(1.0f - cX * cX - cY * cY);;

	printf("X: %3.2f, Y: %3.2f, W: %3.2f\n", x, y, w);
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}
