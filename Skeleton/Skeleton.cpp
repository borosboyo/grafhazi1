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
	layout(location = 1) in vec2 vertexUV;

	out vec2 texCoord;

    void main() {
		texCoord = vertexUV;
        gl_Position = vec4(vp.x/vp.z, vp.y/vp.z, 0, sqrt(vp.x * vp.x + vp.y * vp.y + 1 ));
//gl_Position = vec4(vp.x/vp.z, vp.y/vp.z, 0, 1);

    }
)";

// fragment shader in GLSL
const char* const fragmentSource = R"(
	#version 330			// Shader 3.3
	precision highp float;	// normal floats, makes no difference on desktop computers


	uniform sampler2D textureUnit;
	in vec2 texCoord;

	uniform vec3 color;		// uniform variable, the color of the primitive
	out vec4 fragmentColor;		// computed color of the current pixel

	
	void main() {
		fragmentColor = texture(textureUnit, texCoord); // computed color is the color of the primitive
	}
)";



GPUProgram gpuProgram;

class Camera {
	vec2 wCenter;
	vec2 wSize;
public:
	Camera(vec2 wc = vec2(0,0), vec2 ws = vec2(2,2)) : wCenter(wc), wSize(ws){}
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

Camera camera;


unsigned int vao;
unsigned int vbo[2];

class Tex {
	Texture tex[50];

public:

	Tex() {

	}
};

class Graph {
public:
	Graph() {
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(1, &vbo[0]);
		glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	}
	virtual void Draw() = 0;
	~Graph(){
		glDeleteBuffers(1, &vbo[0]);
		glDeleteVertexArrays(1, &vao);
	}
};

class Vertice : public Graph {
public:

	// 16 es 32 kozott legyen
	vec3 circlePoints[50];
	vec3 center;
	float r = 0.05f;
	vec3 color = vec3(0.5f, 0.5f, 0.5f);

	Vertice() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}

	void Draw() {
		glBufferData(GL_ARRAY_BUFFER, 50 * sizeof(vec3), &circlePoints[0], GL_STATIC_DRAW);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 50);
	}

	void setCenter(vec3 newCenter) {
		center = newCenter;
	}

	vec3 getCenter() {
		return center;
	}

	void setCirclePoints() {
		for (int ii = 0; ii < 50; ii++) {
			float fi = ii * 2 * M_PI / 50;
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
		if (isnan(p1.x) || isnan(p1.y)) {
			p1 = { 0.0f,0.0f,1.0f };
			return 0.0f;
		}
		else {
			return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
		}
		
	}

	float distance(vec3 p1, vec3 p2) {
		if (isnan(acoshf(-multiply(p1, p2)))) {
			return 0.0f;
		}
		else {
			return acoshf(-multiply(p1, p2));
		}
	}

	bool isEqual(float f1, float f2) {
		float epsilon = 1.19e-11f;
		if ((fabs(f1 - f2) <= epsilon)) {
			return true;
		}
		return false;
	}

	void Mirror(vec3 m1, vec3 m2) {

		float d1 = distance(m1, center);
		vec3 v1 = (m1 - center * coshf(d1)) / sinhf(d1);
		vec3 p1 = center * coshf(2 * d1) + v1 * sinhf(2 * d1);
		
		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * cosh(d2)) / sinhf(d2);
		center = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		setCirclePoints();
		Draw();
	}
};

class AllVertices {
public:
	Vertice allVertices[50];
	AllVertices() {
		for (int ii = 0; ii < 50; ii++) {
			float x = ((float)rand() / RAND_MAX) * 2.0f - 1;
			float y = ((float)rand() / RAND_MAX) * 2.0f - 1;
			float z = sqrtf(x * x + y * y + 1);
			Vertice* tmp = new Vertice();
			tmp->setCenter(vec3(x, y, z));
			tmp->setCirclePoints();
			allVertices[ii] = *tmp;
		}
	}

	void DrawVertices() {
		for (int ii = 0; ii < 50; ii++) {
			allVertices[ii].Draw();
		}
	}

	float multiply(vec3 p1, vec3 p2) {
		if (isnan(p1.x) || isnan(p1.y)) {
			p1 = { 0.0f,0.0f,1.0f };
			return 0.0f;
		}
		else {
			return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
		}

	}

	float distance(vec3 p1, vec3 p2) {
		if (isnan(acoshf(-multiply(p1, p2)))) {
			return 0.0f;
		}
		else {
			return acoshf(-multiply(p1, p2));
		}
	}

	bool isEqual(float f1, float f2) {
		float epsilon = 0.01;
		if ((fabs(f1 - f2) <= epsilon)) {
			return true;
		}
		return false;
	}

	void PushVertices(vec3 q) {
		//Origo es celpont kozti m pont meghatarozas
		vec3 p1 = { 0.0f,0.0f,1.0f };

		float d1 = distance(p1, q);
		vec3 v1 = (q - p1 * coshf(d1)) / sinhf(d1);
		vec3 m1 = p1 * coshf(d1 / 300) + v1 * sinhf(d1 / 300);

		//Lepes ezen az egyenesen 
		vec3 p2 = p1;
		float d2 = distance(p2, m1);
		vec3 v2 = (m1 - p2 * coshf(d2)) / sinhf(d2);
		vec3 tempP = p2 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		//megvan az uj pont: kozte es a celpont kozott keressuk a tavolsagot ami az m2
		float d3 = distance(tempP, q);
		vec3 v3 = (q - tempP * coshf(d3)) / sinhf(d3);
		vec3 m2 = tempP * coshf(d3 / 200) + v3 * sinhf(d3 / 200);

		//printf("X: %3.2f Y: %3.2f Z: %3.2f ", m1.x, m1.y, m1.z);
		//printf("X: %3.2f Y: %3.2f Z: %3.2f \n", m2.x, m2.y, m2.z);


		for (int ii = 0; ii < 50; ii++) {
			allVertices[ii].Mirror(m1, m2);
		}
	}

	mat4 MVP() {
		return mat4{ 1, 0, 0, 0,     
					 0, 1, 0, 0,    
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}

	Vertice getVertice(int idx) {
		return allVertices[idx];
	}


};

AllVertices* verticesContainer;

class Line : public Graph {
	vec3 p1;
	vec3 p2;
	vec3 color = vec3(1.0f, 1.0f, 0.0f);
public:
	Line() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}
	void Draw() {
		vec3 line[2] = { p1,p2 };
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
		if (isnan(p1.x) || isnan(p1.y)) {
			p1 = { 0.0f,0.0f,1.0f };
			return 0.0f;
		}
		else {
			return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
		}

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

	void mirrorp1(vec3 m1, vec3 m2){
		float d1 = distance(m1, this->p1);
		vec3 v1 = (m1 - this->p1 * coshf(d1)) / sinhf(d1);
		vec3 p1 = this->p1 * coshf(2 * d1) + v1 * sinhf(2 * d1);

		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * cosh(d2)) / sinhf(d2);
		this->p1 = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);
	}

	void mirrorp2(vec3 m1, vec3 m2) {
		float d1 = distance(m1, this->p2);
		vec3 v1 = (m1 - this->p2 * coshf(d1)) / sinhf(d1);
		vec3 p1 = this->p2 * coshf(2 * d1) + v1 * sinhf(2 * d1);

		float d2 = distance(p1, m2);
		vec3 v2 = (m2 - p1 * cosh(d2)) / sinhf(d2);
		this->p2 = p1 * coshf(2 * d2) + v2 * sinhf(2 * d2);
	}

};

class AllLines {
private:
	//1225 grafel lehet osszesen, ennek 5 szazaleka 61
	Line lines[61];
	vec3 color;

public:
	AllLines() {
		initRandomLines();
	}

	void initRandomLines() {
		int nextLine = 0;
		vec3 firstVertice, secondVertice;
		bool checkIfEdgeExists;
		int firstIndex, secondIndex;

		while (nextLine !=  60) {
			firstIndex = (rand() * 50) / RAND_MAX;
			secondIndex = (rand() * (49)) / RAND_MAX;
			if (secondIndex == firstIndex) 
				continue;

			firstVertice = verticesContainer->allVertices[firstIndex].getCenter();
			secondVertice = verticesContainer->allVertices[secondIndex].getCenter();
			checkIfEdgeExists = false;

			for (int ii = 0; ii < 50; ii++) {
				if (isEqual(lines[ii].getP1(), firstVertice) && isEqual(lines[ii].getP2(), secondVertice)) {
					checkIfEdgeExists = true;
				}
				else if (isEqual(lines[ii].getP2(), firstVertice) && isEqual(lines[ii].getP1(), secondVertice)) {
					checkIfEdgeExists = true;
				}
			}

			if (!checkIfEdgeExists) {
				lines[nextLine].setP1P2(firstVertice, secondVertice);
			}
			nextLine++;
		}
	}

	bool isEqual(vec3 v1, vec3 v2) {
		float epsilon = 1.19e-7f;;
		if ((fabs(v1.x - v2.x) <= epsilon) && (fabs(v1.y - v2.y) <= epsilon && (fabs(v1.z - v2.z) <= epsilon))) {
			return true;
		}
		return false;
	}

	void DrawLines() {
		for (int ii = 0; ii < 61; ii++) {
			lines[ii].Draw();
		}
	}

	float multiply(vec3 p1, vec3 p2) {
		if (isnan(p1.x) || isnan(p1.y)) {
			p1 = { 0.0f,0.0f,1.0f };
			return 0.0f;
		}
		else {
			return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
		}

	}

	float distance(vec3 p1, vec3 p2) {
		if (isnan(acoshf(-multiply(p1, p2)))) {
			return 0.0f;
		}
		else {
			return acoshf(-multiply(p1, p2));
		}
	}

	void PushLines(vec3 q) {
		//Origo es celpont kozti m pont meghatarozas
		vec3 p1 = { 0.0f,0.0f,1.0f};

		float d1 = distance(p1, q);
		vec3 v1 = (q - p1 * coshf(d1)) / sinhf(d1);
		vec3 m1 = p1 * coshf(d1 / 300) + v1 * sinhf(d1 / 300);

		//Lepes ezen az egyenesen 
		vec3 p2 = p1;
		float d2 = distance(p2, m1);
		vec3 v2 = (m1 - p2 * coshf(d2)) / sinhf(d2);
		vec3 tempP = p2 * coshf(2 * d2) + v2 * sinhf(2 * d2);

		//megvan az uj pont: kozte es a celpont kozott keressuk a tavolsagot ami az m2
		float d3 = distance(tempP, q);
		vec3 v3 = (q - tempP * coshf(d3)) / sinhf(d3);
		vec3 m2 = tempP * coshf(d3 / 200) + v3 * sinhf(d3 / 200);

	

		for (int ii = 0; ii < 61; ii++) {
			lines[ii].Mirror(m1, m2);
		}
	}

};

AllLines* lines;
Line* myLine;

//VerticeTexture* tex;

Texture sample;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glLineWidth(5.0f); glPointSize(5.0f);
	verticesContainer = new AllVertices();
	lines = new AllLines();

	std::vector<vec4> data;
	for (int ii = 0; ii < 100; ii++) {
		for (int jj = 0; jj < 100; jj++) {
			int index = (ii * 100 + ii) * 4;
			if (((ii / 10) + ii / 10) % 2) {
				//fosszin
				data.push_back(vec4(1.0, 0.5, 0, 1.0));
			}
			else {
				//mas szin
				data.push_back(vec4(0.5, 1.0, 0.5, 1.0));
			}
		}
	}

	sample.create(100, 100, data);
	//glutPostRedisplay();

	gpuProgram.create(vertexSource, fragmentSource, "fragmentColor");
}


void onDisplay() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	lines->DrawLines();
	verticesContainer->DrawVertices();

	glutSwapBuffers();

}


void onKeyboard(unsigned char key, int pX, int pY) {
	if (key == 'd') glutPostRedisplay();     
}


void onKeyboardUp(unsigned char key, int pX, int pY) {
}


bool mouseLeftPressed = false;

void onMouseMotion(int pX, int pY) {
	float cX = 2.0f * pX / windowWidth - 1;
	float cY = 1.0f - 2.0f * pY / windowHeight;
	if (mouseLeftPressed) {
		float x = cX / sqrtf(1 - cX * cX - cY * cY);
		float y = cY / sqrtf(1 - cX * cX - cY * cY);
		float z = 1 / sqrtf(1 - cX * cX - cY * cY);
		if (!verticesContainer->isEqual(x, 0.0f) && !verticesContainer->isEqual(y, 0.0f) && !verticesContainer->isEqual(z, 1.0f)) {
			verticesContainer->PushVertices(vec3(x, y, z));
			lines->PushLines(vec3(x, y, z));
		}
		
	}
	glutPostRedisplay();
}


void onMouse(int button, int state, int pX, int pY) {
	float cX = 2.0f * pX / windowWidth - 1;
	float cY = 1.0f - 2.0f * pY / windowHeight;

	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			mouseLeftPressed = true;
		}
		else mouseLeftPressed = false;
	}


}


void onIdle() {
	long time = glutGet(GLUT_ELAPSED_TIME); 
}
