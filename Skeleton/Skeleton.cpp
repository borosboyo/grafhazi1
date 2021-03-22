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
        gl_Position = vec4(vp.x/vp.z, vp.y/vp.z, 0, 1) * MVP;

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
		gpuProgram.setUniform(color, "color");
		gpuProgram.setUniform(MVP() * camera.V() * camera.P(), "MVP");
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
};


class Line : public Graph {
	vec3 p1;
	vec3 p2;
	vec3 color = vec3(1.0f,1.0f,0.0f);
public:
	Line() {
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glBindVertexArray(vao);
	}
	void Draw() {
		vec3 line[2] = { p1,p2 };
		gpuProgram.setUniform(color, "color");
		gpuProgram.setUniform(MVP() * camera.V() * camera.P(), "MVP");
		//lehet itt a size
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
		return p1.x * p2.x + p1.y * p2.y - p1.z * p2.z;
	}

	float d(vec3 p1, vec3 p2) {
		return acoshf(multiply(p1, p2) * -1.0);
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



class AllLines {
private:
	//1225 grafel lehet osszesen, ennek 5 szazaleka 61
	Line lines[61];
	vec3 color;

public:
	AllLines(AllVertices vertices) {
		//initLines(vertices);
	}

	void initLines(AllVertices vertices) {
		int nextLine = 0;
		vec3 firstVertice, secondVertice;
		bool checkIfEdgeExists;
		int firstIndex, secondIndex;

		while (nextLine != 61 - 1) {
			firstIndex = rand() * 61 / RAND_MAX;
			secondIndex = rand() * (61 - 1) / RAND_MAX;
			if (secondIndex >= firstIndex) {
				++secondIndex;
			}
			firstVertice =  vertices.allVertices[firstIndex].getCenter();
			secondVertice = vertices.allVertices[secondIndex].getCenter();
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
		float epsilon = 1.19e-7f;
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

	mat4 MVP() {
		return mat4{ 1, 0, 0, 0,    
					 0, 1, 0, 0,    
					 0, 0, 1, 0,
					 0, 0, 0, 1 };
	}
};



class VerticeTexture {
	Texture texture;
	vec2 uvs[50];
public:
	VerticeTexture(int width, int height, const std::vector<vec4>& image, Vertice vertice) {
		texture.create(width, height, image);
		for (int ii = 0; ii < 50; ii++) {
			uvs[ii].x = vertice.circlePoints[ii].x / vertice.circlePoints[ii].z;
			uvs[ii].y = vertice.circlePoints[ii].y / vertice.circlePoints[ii].z;
		}

		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glGenBuffers(2, vbo);

		// vertex coordinates: vbo[1] -> Attrib Array 1 -> vertexUV of the vertex shader
		glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // make it active, it is an array
		glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);     // stride and offset: it is tightly packed		
	}


	void Draw() {
		mat4 MVPTransform = camera.V() * camera.P();
		gpuProgram.setUniform(MVPTransform, "MVP");
		gpuProgram.setUniform(texture, "textureUnit");

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 50);

	}

};

class AllVerticeTextures {

};









AllVertices* vertices;
AllLines* lines;


VerticeTexture* tex;

void onInitialization() {
	glViewport(0, 0, windowWidth, windowHeight);
	glLineWidth(5.0f); glPointSize(5.0f);

	vertices = new AllVertices();


	/*
	

	int width = 128, height = 128;				// create checkerboard texture procedurally
	std::vector<vec4> image(width * height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			float luminance = ((x / 16) % 2) ^ ((y / 16) % 2);
			image[y * width + x] = vec4(luminance, luminance, luminance, 1);
		}
	}
	tex = new VerticeTexture(width, height, image, vertices->allVertices[0]);

		*/


	//lines = new AllLines(*vertices);


	gpuProgram.create(vertexSource, fragmentSource, "fragmentColor");
}


void onDisplay() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vertices->DrawVertices();

	//lines->DrawLines();

	//tex->Draw();

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
