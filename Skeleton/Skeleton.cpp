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


vec3 modelVertices[50];
vec2 viewVertices[50];
int numberOfVertices = 50;
double percentOfEdges = 0.05;

//Alapbol 1225, igy 61
int numberOfLines = ((numberOfVertices * (numberOfVertices - 1)) / 2) * (percentOfEdges);
//masodik koord numberOfLines
vec2 graphEdges[61][2];
const int nTessV = 50;
vec2 circlePoints[50];


class Graph {

public:
	Graph() {
		initVertices();
		initLines();

	}

	void initVertices() {
		int temp = 0;
		float x;
		float y;
		float w;
		while (temp != numberOfVertices) {
			float randX = (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
			float randY = (float)rand() / (float)(RAND_MAX / 2) - 1.0f;
			if (randX * randX + randY * randY <= 1) {
				viewVertices[temp] = vec2(randX, randY);
				x = randX / sqrtf(1.0f - randX * randX - randY * randY);
				y = randY / sqrtf(1.0f - randX * randX - randY * randY);
				w = 1.0f / sqrtf(1.0f - randX * randX - randY * randY);
				modelVertices[temp] = vec3(x, y, w);
				temp++;
			}
		}
	}

	void initLines() {
		int nextLine = 0;
		bool checkIfEdgeExists;
		int firstIndex, secondIndex;
		vec2 firstVertice, secondVertice; 
		/*
		while (nextLine != numberOfLines) {
			firstIndex = rand() * numberOfVertices / RAND_MAX;
			secondIndex = rand() * numberOfVertices - 1 / RAND_MAX;
			if (secondIndex >= firstIndex) {
				++secondIndex;
			}
			firstVertice = viewVertices[firstIndex];
			secondVertice = viewVertices[secondIndex];
			checkIfEdgeExists = false;

			for (int ii = 0; ii < 61; ii++) {

				if (isEqual(graphEdges[0][1], firstVertice) && isEqual(graphEdges[1][1], secondVertice)) {
					checkIfEdgeExists = true;
				}
				else if (isEqual(graphEdges[1][1], firstVertice) && isEqual(graphEdges[0][1], secondVertice)) {
					checkIfEdgeExists = true;
				}

			}
			if (!checkIfEdgeExists) {
				graphEdges[0][1] = firstVertice;
				graphEdges[1][1] = secondVertice;
			}

			nextLine++;
		}
		*/
		firstVertice = viewVertices[45];
		printf("%3.2f %3.2f\n", firstVertice.x, firstVertice.y);


		secondVertice = viewVertices[42];
		printf("%3.2f %3.2f\n", secondVertice.x, secondVertice.y);

		graphEdges[0][50] = firstVertice;
		graphEdges[1][50] = secondVertice;


		printf("FIRST: %3.2f %3.2f,  SECOND: %3.2f %3.2f ", graphEdges[0][50].x, graphEdges[0][50].y,
			graphEdges[1][50].x, graphEdges[1][50].y);


		

	}

	bool isEqual(vec2 v1, vec2 v2) {
		float epsilon = 1.19e-7f;
		if ((fabs(v1.x - v2.x) <= epsilon) && (fabs(v1.y - v2.y) <= epsilon)) {
			return true;
		}
		return false;
	}

};

unsigned int vao, vbo;
Graph* graph;

class InstantRender : public GPUProgram {

public:
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


	InstantRender() {
		glViewport(0, 0, windowWidth, windowHeight);
		glLineWidth(2.0f); glPointSize(10.0f);

		create(vertexSource, fragmentSource, "outColor");
		glGenVertexArrays(1, &vao); glBindVertexArray(vao);
		glGenBuffers(1, &vbo); 		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);  
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0 , NULL);

		create(vertexSource, fragmentSource, "outColor");
	}

	void DrawGPU(int type, vec2 vertices[], vec3 color, int size) {
		setUniform(color, "color");

		//itt lehet size is
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * size, vertices, GL_STATIC_DRAW);
		glDrawArrays(type, 0, sizeof(vec2) * size);
	}

	~InstantRender() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};



InstantRender* gpuProgram;

// Initialization, create an OpenGL context
void onInitialization() {

	gpuProgram = new InstantRender();
	graph = new Graph();

	for (int ii = 0; ii < nTessV; ii++) {
		float fi = ii * 2 * M_PI / nTessV;
		circlePoints[ii] = vec2(cosf(fi),sinf(fi));
	}
}

// Window has become invalid: Redraw
void onDisplay() {
	glClearColor(0, 0, 0, 0);     // background color
	glClear(GL_COLOR_BUFFER_BIT); // clear frame buffer

	// Set color to (0, 1, 0) = green
	int location = glGetUniformLocation(gpuProgram->getId(), "color");
	glUniform3f(location, 0.5f, 0.5f, 0.5f); // 3 floats

	float MVPtransf[4][4] = { 1, 0, 0, 0,    // MVP matrix, 
							  0, 1, 0, 0,    // row-major!
							  0, 0, 1, 0,
							  0, 0, 0, 1 };

	location = glGetUniformLocation(gpuProgram->getId(), "MVP");	// Get the GPU location of uniform variable MVP
	glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);	// Load a 4x4 row-major float matrix to the specified location


	gpuProgram->DrawGPU(GL_TRIANGLE_FAN, circlePoints, vec3(0.5f,0.5f,0.5f),50);
	gpuProgram->DrawGPU(GL_POINTS, viewVertices, vec3(1.0f, 0.0f, 0.0f),50);


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


	float x = cX / sqrtf(1.0f - cX * cX - cY * cY);
	float y = cY / sqrtf(1.0f - cX * cX - cY * cY);;
	float w = 1.0f / sqrtf(1.0f - cX * cX - cY * cY);;

	printf("X: %3.2f, Y: %3.2f, W: %3.2f\n", x, y, w);
}

// Idle event indicating that some time elapsed: do animation here
void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); // elapsed time since the start of the program
}
