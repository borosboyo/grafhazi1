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


// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char* const vertexSource = R"(
    #version 330                // Shader 3.3
    precision highp float;        // normal floats, makes no difference on desktop computers

    uniform mat4 MVP;            // uniform variable, the Model-View-Projection transformation matrix
    layout(location = 0) in vec3 vp;
    //layout(location = 0) in vec2 vp;    // Varying input: vp = vertex position is expected in attrib array 0

    void main() {
        gl_Position = vec4(vp.x/vp.z, vp.y/vp.z, 0, 1) * MVP;
        //gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;        // transform vp from modeling space to normalized device space
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

GPUProgram gpuProgram;

class Camera {
	vec2 wCenter;
	vec2 wSize;

public:

	Camera(vec2 wc = vec2(0,0), vec2 ws = vec2(2,2)) : wCenter(wc), wSize(ws){}
	mat4 V() {
		return TranslateMatrix(-wCenter);
	}
	mat4 P() {
		return ScaleMatrix(vec2(2 / wSize.x, 2 / wSize.y));
	}
	mat4 Vinv() {
		return TranslateMatrix(wCenter);
	}
	mat4 Pinv() {
		return ScaleMatrix(vec2(wSize.x / 2, wSize.y / 2));
	}


};

Camera camera;

class Graph {
protected:
	unsigned int vao;
	unsigned int vbo;
public:
	Graph() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
	}
	virtual void Draw() = 0;
	~Graph(){
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};

class Vertice : public Graph {
private:
	std::vector<vec3> circlePoints;
	vec3 center;
	float r;
	vec3 color;
public:
	Vertice(vec3 newCenter, float newRadius, vec3 newColor) {
		center = newCenter;
		r = newRadius;
		color = newColor;

		for (int ii = 0; ii < 50; ii++) {
			float fi = float(ii * 2 * M_PI / 50);
			float x = r * cosf(fi) + center.x;
			float y = r * sinf(fi) + center.y;
			float z = sqrtf(x * x + y * y + 1.0f);
			circlePoints.push_back(vec3(x, y, z));
		}

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}

	void Draw() {
		gpuProgram.setUniform(color, "color");
		gpuProgram.setUniform(M() * camera.V() * camera.P(), "MVP");
		glBufferData(GL_ARRAY_BUFFER, circlePoints.size() * sizeof(vec3), &circlePoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLE_FAN, 0, circlePoints.size());
	}

	mat4 M() {
		return mat4{ 1, 0, 0, 0,    // MVP matrix, 
				     0, 1, 0, 0,    // row-major!
				     0, 0, 1, 0,
				     0, 0, 0, 1 };
	}
	/*
	void printCircle() {
		for (int ii = 0; ii < 50; ii++) {
			printf("X: %3.2f Y: %3.2f Z: %3.2f\n", circlePoints[ii].x, circlePoints[ii].y, circlePoints[ii].z);
		}
	}
	*/
	void printCoords() {
		printf("X: %3.2f Y: %3.2f Z: %3.2f\n", center.x, center.y, center.z);
	}


};


class AllVertices {
private:
	std::vector<Vertice> allvertices;
public:
	AllVertices(float r, vec3 color) {
		for (int ii = 0; ii < 50; ii++) {
			float x = ((float)rand() / RAND_MAX) * 2.0f - 1;
			float y = ((float)rand() / RAND_MAX) * 2.0f - 1;
			float z = sqrtf(x * x + y * y + 1);
			Vertice* tmp = new Vertice(vec3(x, y, z), r, color);
			allvertices.push_back(*tmp);
		}
	}

	void DrawVertices() {
		for (int ii = 0; ii < 50; ii++) {
			allvertices[ii].Draw();
		}
	}

	float multiply(vec3 p1, vec3 p2) {
		return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
	}

	float d(vec3 p1, vec3 p2) {
		return acoshf(multiply(p1, p2) * -1.0);
	}
	/*
	std::vector<Vertice> getAllVertices() {
		return allvertices;
	}

		void printAllCoords() {
		for (int ii = 0; ii < 50; ii++) {
			allvertices[ii].printCoords();
		}
	}

	void printOneCircle(int idx) {
		allvertices[idx].printCircle();
	}


	*/

	mat4 M() {
		return mat4{ 1, 0, 0, 0,    // MVP matrix, 
					 0, 1, 0, 0,    // row-major!
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}
};


class Line : public Graph {
private: 
	vec3 p1;
	vec3 p2;
	vec3 color;
public:
	Line() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}
	void Draw() {
		vec3 line[2] = { p1,p2 };
		gpuProgram.setUniform(color, "color");
		gpuProgram.setUniform(M() * camera.V() * camera.P(), "MVP");
		//lehet itt a size
		glBufferData(GL_ARRAY_BUFFER, 2 , line, GL_STATIC_DRAW);
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void setP1P2(vec3 newP1, vec3 newP2) {
		p1 = newP1;
		p2 = newP2;
	}

	mat4 M() {
		return mat4{ 1, 0, 0, 0,    // MVP matrix, 
					 0, 1, 0, 0,    // row-major!
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}

};

class AllLines {
private:
	std::vector<Line> lines;
	vec3 color;

public:
	AllLines(AllVertices vertice, vec3 color) {
		
	}


	void initLines() {

	}

	void DrawEdges() {
		for (int ii = 0; ii < lines.size(); ii++) {
			lines[ii].Draw();
		}
	}

	mat4 M() {
		return mat4{ 1, 0, 0, 0,    // MVP matrix, 
					 0, 1, 0, 0,    // row-major!
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}
};


AllVertices* vertices;
AllLines* lines;

std::vector<vec3> random;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glLineWidth(5.0f); glPointSize(5.0f);


	vertices = new AllVertices(0.05f, vec3(1.0f, 1.0f, 0.0f));
	vertices->printAllCoords();
	vertices->printOneCircle(0);

	//lines = new AllLines(*vertices, 0.05f)



	gpuProgram.create(vertexSource, fragmentSource, "outColor");
}


void onDisplay() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertices->DrawVertices();

	glutSwapBuffers();
}


void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();     
}


void onKeyboardUp(unsigned char key, int pX, int pY) {
}


void onMouseMotion(int pX, int pY) {	
	float cX = 2.0f * pX / windowWidth - 1;	
	float cY = 1.0f - 2.0f * pY / windowHeight;
	printf("Mouse moved to (%3.2f, %3.2f)\n", cX, cY);
}


void onMouse(int button, int state, int pX, int pY) { 
	float cX = 2.0f * pX / windowWidth - 1;	
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


void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); 
}
