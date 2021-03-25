//=============================================================================================
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
// Nev    : Boros Gergo
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

    void main() {
        gl_Position = vec4(vp.x/vp.z, vp.y/vp.z, 0, sqrt(vp.x * vp.x + vp.y * vp.y + 1)) ;
    }
)";

// fragment shader in GLSL
const char* const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers

	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 fragmentColor;		// computed color of the current pixel
	
	void main() {
		fragmentColor = vec4(color,1); // computed color is the color of the primitive
	}
)";


class Camera {
	vec2 wCenter;
	vec2 wSize;
public:
	Camera(vec2 wc = vec2(0, 0), vec2 ws = vec2(2, 2)) : wCenter(wc), wSize(ws) {}
	mat4 V() {
		return mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			-wCenter.x, -wCenter.y, 0, 1);
	}
	mat4 P() {
		return mat4(2 / wSize.x, 0, 0, 0,
			0, 2 / wSize.y, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
	mat4 Vinv() {
		return mat4(1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			wCenter.x, wCenter.y, 0, 1);
	}
	mat4 Pinv() {
		return mat4(wSize.x / 2, 0, 0, 0,
			0, wSize.y / 2, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1);
	}
};

GPUProgram gpuProgram;
Camera camera;
unsigned int vao;
unsigned int vbo;

class Graph {
public:
	virtual void Draw() = 0;
	~Graph() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};

class Container {
public:

	float multiply(vec3 p1, vec3 p2) {
		return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
	}

	float distance(vec3 p1, vec3 p2) {
		return acoshf(-multiply(p1, p2));
	}

	virtual void Push(vec3 q) = 0;

	mat4 MVP() {
		return mat4{ 1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}

};

class Vertice : public Graph {
public:
	vec3 circlePoints[20];
	vec3 center;
	float r = 0.05f;
	vec3 color = { 0.5f,0.5f,0.5f };

	void initVertice() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}

	void Draw() {
		gpuProgram.setUniform(color, "color");
		glBufferData(GL_ARRAY_BUFFER, 20 * sizeof(vec3), &circlePoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 20);
	}

	void setCenter(vec3 newCenter) {
		center = newCenter;
	}

	vec3 getCenter() {
		return center;
	}

	void setCirclePoints() {
		for (int ii = 0; ii < 20; ii++) {
			float fi = static_cast<float> (ii * 2 * M_PI) / 20.0f;
			float x = r * cosf(fi) + center.x;
			float y = r * sinf(fi) + center.y;
			float z = sqrtf(x * x + y * y + 1.0f);
			circlePoints[ii] = (vec3(x, y, z));
		}
	}

	mat4 MVP() {
		return mat4{ 1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}

	float multiply(vec3 p1, vec3 p2) {
		return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;

	}

	float distance(vec3 p1, vec3 p2) {
		return acoshf(-multiply(p1, p2));
	}

	bool isEqual(float f1, float f2) {
		float epsilon = 1.19e-11f;
		if ((fabsf(f1 - f2) <= epsilon)) {
			return true;
		}
		return false;
	}

	void Mirror(vec3 m1, vec3 m2) {

		float d1 = distance(m1, center);
		vec3 v1 = (m1 - center * coshf(d1)) / sinhf(d1);
		vec3 p1 = center * coshf(2 * d1) + v1 * sinhf(2 * d1);

		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * coshf(d2)) / sinhf(d2);
		center = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		setCirclePoints();
		Draw();

	}
};

class AllVertices : public Container {
public:
	Vertice allVertices[50];

	void initVerticeCoord() {
		for (int ii = 0; ii < 50; ii++) {
			float x = (static_cast<float> (rand()) / RAND_MAX) * 2.0f - 1.0f;
			float y = (static_cast<float> (rand()) / RAND_MAX) * 2.0f - 1.0f;
			float z = sqrtf(x * x + y * y + 1);
			Vertice* tmp = new Vertice();
			tmp->setCenter(vec3(x, y, z));
			tmp->setCirclePoints();
			allVertices[ii] = *tmp;
			allVertices[ii].initVertice();
		}
	}

	void DrawVertices() {
		for (int ii = 0; ii < 50; ii++) {
			allVertices[ii].Draw();
		}
	}


	bool isEqual(float f1, float f2) {
		float epsilon = 0.01f;
		if ((fabsf(f1 - f2) <= epsilon)) {
			return true;
		}
		return false;
	}

	void Push(vec3 q) {
		//Origo es celpont kozti m pont meghatarozas
		vec3 p1 = { 0.0f,0.0f,1.0f };

		float d1 = distance(p1, q);
		vec3 v1 = (q - p1 * coshf(d1)) / sinhf(d1);
		vec3 m1 = p1 * coshf(d1 / 30) + v1 * sinhf(d1 / 30);

		//Lepes ezen az egyenesen 
		vec3 p2 = p1;
		float d2 = distance(p2, m1);
		vec3 v2 = (m1 - p2 * coshf(d2)) / sinhf(d2);
		vec3 tempP = p2 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		//megvan az uj pont: kozte es a celpont kozott keressuk a tavolsagot ami az m2
		float d3 = distance(tempP, q);
		vec3 v3 = (q - tempP * coshf(d3)) / sinhf(d3);
		vec3 m2 = tempP * coshf(d3 / 20) + v3 * sinhf(d3 / 20);

		for (int ii = 0; ii < 50; ii++) {
			allVertices[ii].Mirror(m1, m2);
		}
	}

	Vertice getVertice(int idx) {
		return allVertices[idx];
	}

};

class Line : public Graph {
	vec3 p1;
	vec3 p2;
	vec3 color = { 1.0f,1.0f,0.0f };


public:

	void initLine() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}

	void Draw() {
		vec3 line[2] = { p1,p2 };
		gpuProgram.setUniform(color, "color");
		glBufferData(GL_ARRAY_BUFFER, 2 * sizeof(vec3), line, GL_STATIC_DRAW);
		glLineWidth(2);
		glDrawArrays(GL_LINES, 0, 2);
	}

	void setP1P2(vec3 newP1, vec3 newP2) {
		p1 = newP1;
		p2 = newP2;
	}

	vec3 getP1() {
		return p1;
	}

	vec3 getP2() {
		return p2;
	}

	mat4 MVP() {
		return mat4{ 1, 0, 0, 0,
					 0, 1, 0, 0,
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}

	float multiply(vec3 p1, vec3 p2) {
		return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
	}

	float distance(vec3 p1, vec3 p2) {
		if (isnan(acoshf(-multiply(p1, p2)))) {
			return 0.0f;
		}
		else {
			return acoshf(-multiply(p1, p2));
		}
	}

	void Mirror(vec3 m1, vec3 m2) {
		mirrorp1(m1, m2);
		mirrorp2(m1, m2);
		Draw();
	}

	void mirrorp1(vec3 m1, vec3 m2) {
		float d1 = distance(m1, this->p1);
		vec3 v1 = (m1 - this->p1 * coshf(d1)) / sinhf(d1);
		vec3 p1 = this->p1 * coshf(2 * d1) + v1 * sinhf(2 * d1);

		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * coshf(d2)) / sinhf(d2);
		this->p1 = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);
	}

	void mirrorp2(vec3 m1, vec3 m2) {
		float d1 = distance(m1, this->p2);
		vec3 v1 = (m1 - this->p2 * coshf(d1)) / sinhf(d1);
		vec3 p1 = this->p2 * coshf(2 * d1) + v1 * sinhf(2 * d1);

		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * coshf(d2)) / sinhf(d2);
		this->p2 = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);
	}

};

class AllLines : public Container {
private:
	//1225 grafel lehet osszesen, ennek 5 szazaleka 61
	Line lines[61];

public:

	void initRandomLines(AllVertices* vc) {
		vec3 start, end;
		int firstIndex, secondIndex;

		for (int ii = 0; ii < 61; ii++) {
			firstIndex = (rand() * 50) / RAND_MAX;
			secondIndex = (rand() * (49)) / RAND_MAX;
			start = vc->getVertice(firstIndex).getCenter();
			end = vc->getVertice(secondIndex).getCenter();
			lines[ii].setP1P2(start, end);
			lines[ii].initLine();
		}
	}

	bool isEqual(vec3 v1, vec3 v2) {
		float epsilon = 1.19e-2f;;
		if ((fabsf(v1.x - v2.x) <= epsilon) && (fabsf(v1.y - v2.y) <= epsilon && (fabsf(v1.z - v2.z) <= epsilon))) {
			return true;
		}
		return false;
	}

	void DrawLines() {
		for (int ii = 0; ii < 61; ii++) {
			lines[ii].Draw();
		}
	}

	void Push(vec3 q) {
		//Origo es celpont kozti m pont meghatarozas
		vec3 p1 = { 0.0f,0.0f,1.0f };

		float d1 = distance(p1, q);
		vec3 v1 = (q - p1 * coshf(d1)) / sinhf(d1);
		vec3 m1 = p1 * coshf(d1 / 30) + v1 * sinhf(d1 / 30);

		//Lepes ezen az egyenesen 
		vec3 p2 = p1;
		float d2 = distance(p2, m1);
		vec3 v2 = (m1 - p2 * coshf(d2)) / sinhf(d2);
		vec3 tempP = p2 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		//megvan az uj pont: kozte es a celpont kozott keressuk a tavolsagot ami az m2
		float d3 = distance(tempP, q);
		vec3 v3 = (q - tempP * coshf(d3)) / sinhf(d3);
		vec3 m2 = tempP * coshf(d3 / 20) + v3 * sinhf(d3 / 20);


		for (int ii = 0; ii < 61; ii++) {
			lines[ii].Mirror(m1, m2);
		}
	}

};

AllVertices* verticesContainer;
AllLines* lines;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glLineWidth(1.0f);
	glClearColor(0.0f, 0.2f, 0.2f, 1.0f);							
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

	verticesContainer = new AllVertices();
	verticesContainer->initVerticeCoord();

	lines = new AllLines();
	lines->initRandomLines(verticesContainer);

	gpuProgram.create(vertexSource, fragmentSource, "fragmentColor");
}

void onDisplay() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	lines->DrawLines();
	verticesContainer->DrawVertices();

	glutSwapBuffers();
}


void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == ' ') {
		glutPostRedisplay();
	}
}


void onKeyboardUp(unsigned char key, int pX, int pY) {
}

bool mouseRightPressed = false;
void onMouseMotion(int pX, int pY) {
	float cX = 2.0f * pX / windowWidth - 1.0f;
	float cY = 1.0f - 2.0f * pY / windowHeight;
	if (mouseRightPressed) {
		float x = cX / sqrtf(1.0f - cX * cX - cY * cY);
		float y = cY / sqrtf(1.0f - cX * cX - cY * cY);
		float z = 1.0f / sqrtf(1.0f - cX * cX - cY * cY);
		if (!verticesContainer->isEqual(x, 0.0f) && !verticesContainer->isEqual(y, 0.0f) && !verticesContainer->isEqual(z, 1.0f)) {
			verticesContainer->Push(vec3(x, y, z));
			lines->Push(vec3(x, y, z));
			glutPostRedisplay();
		}
	}
}


void onMouse(int button, int state, int pX, int pY) {
	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseRightPressed = true;
		}
		else mouseRightPressed = false;
	}

}


void onIdle() {
	
}