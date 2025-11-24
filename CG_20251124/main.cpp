#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "filetobuf.h"
#include "shaderMaker.h"
#include <vector>

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint cubeVAO = 0, cubeVBO = 0;       // 정육면체
GLuint pyramidVAO = 0, pyramidVBO = 0; // 삼각뿔
bool cubeMode = true;
bool lightMode = true;  // 조명 켜기/끄기

// 정육면체 vertex 좌표값
float cube[8][3] =
{
	{0.15f, 0, -0.15f}, {-0.15f, 0, -0.15f}, {-0.15f, 0, 0.15f}, {0.15f, 0, 0.15f},
	{0.15f, 0.3f, -0.15f}, {-0.15f, 0.3f, -0.15f}, {-0.15f, 0.3f, 0.15f}, {0.15f, 0.3f, 0.15f}
};
int faces[6][4] = {
	{0, 1, 2, 3}, // 아래면
	{4, 7, 6, 5}, // 윗면
	{1, 5, 6, 2}, // 뒷면
	{0, 3, 7, 4}, // 앞면
	{0, 4, 5, 1}, // 왼쪽
	{3, 2, 6, 7}  // 오른쪽
};
// 정육면체 꼭짓점별 색상
float cubeColors[8][3] = {
	{1,0,0},    // 0
	{0,1,0},    // 1
	{0,0,1},    // 2
	{1,1,0},    // 3
	{1,0,1},    // 4
	{0,1,1},    // 5
	{0.5f,0.5f,0.5f}, // 6
	{1,0.5f,0}  // 7
};


// 삼각뿔 vertex 좌표값
float pyramid[5][3] =
{
	{0, 0.3f, 0}, {0.15f, 0, -0.15f}, {-0.15f, 0, -0.15f}, {-0.15f, 0, 0.15f},
	{0.15f, 0, 0.15f}
};
int pyramidFaces[6][3] = {
	{1, 2, 3}, {1, 3, 4}, {0, 3, 2}, {0, 2, 1}, {0, 4, 3}, {0, 1, 4}  // 0, 1은 한 면
};
// 삼각뿔 꼭짓점별 색상
float pyramidColors[5][3] = {
	{1,0,0},    // 0
	{0,1,0},    // 1
	{0,0,1},    // 2
	{1,1,0},    // 3
	{1,0,1}     // 4
};

int cubeVertexCount = 0;

void pushVertex(std::vector<GLfloat>& vtx, const glm::vec3& p, const glm::vec3& n) 
{
	vtx.push_back(p.x); vtx.push_back(p.y); vtx.push_back(p.z);
	vtx.push_back(n.x); vtx.push_back(n.y); vtx.push_back(n.z);
}

void InitCube()
{
    std::vector<GLfloat> vertices;
    // 각 면별 정점 인덱스
    for (int i = 0; i < 6; i++) 
    {
        int v0 = faces[i][0], v1 = faces[i][1], v2 = faces[i][2], v3 = faces[i][3];
        glm::vec3 p0(cube[v0][0], cube[v0][1], cube[v0][2]);
        glm::vec3 p1(cube[v1][0], cube[v1][1], cube[v1][2]);
        glm::vec3 p2(cube[v2][0], cube[v2][1], cube[v2][2]);
        glm::vec3 p3(cube[v3][0], cube[v3][1], cube[v3][2]);

        // 면 노말(반시계 기준)
        glm::vec3 n = glm::normalize(glm::cross(p2 - p0, p1 - p0));

        // v0, v1, v2
        pushVertex(vertices, p0, n);
        pushVertex(vertices, p1, n);
        pushVertex(vertices, p2, n);
        // v0, v2, v3
        pushVertex(vertices, p0, n);
        pushVertex(vertices, p2, n);
        pushVertex(vertices, p3, n);
    }

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // 위치 (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // 노말 (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    cubeVertexCount = static_cast<int>(vertices.size() / 6);
}

int pyramidVertexCount = 0;

void InitPyramid()
{
    std::vector<GLfloat> vertices;

    // 옆면(정점 0을 포함하는 4개 삼각형)의 면 노말들을 정점별로 누적
    glm::vec3 vNormals[5] = {
        glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)
    };

    auto addFaceNormal = [&](int ia, int ib, int ic) {
        glm::vec3 p0(pyramid[ia][0], pyramid[ia][1], pyramid[ia][2]);
        glm::vec3 p1(pyramid[ib][0], pyramid[ib][1], pyramid[ib][2]);
        glm::vec3 p2(pyramid[ic][0], pyramid[ic][1], pyramid[ic][2]);

        glm::vec3 fn = glm::normalize(glm::cross(p2 - p0, p1 - p0));
        vNormals[ia] += fn;
        vNormals[ib] += fn;
        vNormals[ic] += fn;
        };

    // 옆면 4개: (0,3,2), (0,2,1), (0,4,3), (0,1,4)
    addFaceNormal(0, 3, 2);
    addFaceNormal(0, 2, 1);
    addFaceNormal(0, 4, 3);
    addFaceNormal(0, 1, 4);

    for (int i = 0; i < 5; i++) vNormals[i] = glm::normalize(vNormals[i]);

    // 실제 버퍼에 푸시
    //   - 바닥 2개 삼각형: 정점마다 normal = (0,-1,0) (Flat)
    //   - 옆면 4개 삼각형: 정점마다 normal = vNormals[정점인덱스] (Smooth)
    auto push = [&](int ia, int ib, int ic, bool isBase) {
        glm::vec3 p0(pyramid[ia][0], pyramid[ia][1], pyramid[ia][2]);
        glm::vec3 p1(pyramid[ib][0], pyramid[ib][1], pyramid[ib][2]);
        glm::vec3 p2(pyramid[ic][0], pyramid[ic][1], pyramid[ic][2]);

        if (isBase)
        {
            glm::vec3 n(0.0f, -1.0f, 0.0f); // 바닥은 평평하게
            pushVertex(vertices, p0, n);
            pushVertex(vertices, p1, n);
            pushVertex(vertices, p2, n);
        }
        else {
            // 옆면은 정점 노말 사용
            pushVertex(vertices, p0, vNormals[ia]);
            pushVertex(vertices, p1, vNormals[ib]);
            pushVertex(vertices, p2, vNormals[ic]);
        }
        };

    // 바닥
    push(1, 2, 3, true);
    push(1, 3, 4, true);

    // 옆면
    push(0, 3, 2, false);
    push(0, 2, 1, false);
    push(0, 4, 3, false);
    push(0, 1, 4, false);

    // VBO/VAO 업로드 
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    pyramidVertexCount = static_cast<int>(vertices.size() / 6);
}

void Keboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 'q': exit(0); break;
    }
}

void main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);  // 깊이 버퍼 추가
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(width, height);
	glutCreateWindow("Tesk_29");

	glewExperimental = GL_TRUE;
	glewInit();

    InitCube();
	InitPyramid();

	// callback 함수 등록
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keboard);

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramID);

    // 조명 켜기/끄기 설정
    GLuint lightOnLoc = glGetUniformLocation(shaderProgramID, "lightOn");
    glUniform1i(lightOnLoc, lightMode ? 1 : 0);

    // 조명/객체 색 설정
    GLint lightLoc = glGetUniformLocation(shaderProgramID, "lightColor");
    GLint objLoc = glGetUniformLocation(shaderProgramID, "objectColor");

    glm::vec3 lightBasePos(0.0f, 0.0f, 1.5f);
    glm::vec3 lightPos = glm::vec3(glm::vec4(lightBasePos, 1.0f));

    GLint uLightPos = glGetUniformLocation(shaderProgramID, "lightPos");  // 조명 위치
    GLuint viewPosLoc = glGetUniformLocation(shaderProgramID, "viewPos");    // 카메라 위치
    glUniform3f(lightLoc, 1.0f, 1.0f, 1.0f);      // 흰 조명
    glUniform3f(objLoc, 1.0f, 0.7f, 0.7f);      // 오브젝트 색
    glUniform3f(uLightPos, lightPos.x, lightPos.y, lightPos.z); // 조명 위치

    GLint viewLoc = glGetUniformLocation(shaderProgramID, "view");
    GLint projLoc = glGetUniformLocation(shaderProgramID, "projection");

    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 4.0f);
    glm::vec3 cameraDirection = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);  // 카메라 위치 전달

    glm::mat4 vTransform = glm::mat4(1.0f);
    vTransform = glm::lookAt(cameraPos, cameraDirection, cameraUp);
    vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f)); // X축 -30도 회전
    vTransform = glm::rotate(vTransform, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // Y축 30도 회전
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &vTransform[0][0]);

    glm::mat4 pTransform = glm::mat4(1.0f);
    pTransform = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &pTransform[0][0]);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    if (cubeMode)
    {
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
    }

    else
    {
        glBindVertexArray(pyramidVAO);
        glDrawArrays(GL_TRIANGLES, 0, 18); // 각 면은 삼각형 1개(3정점)
        glBindVertexArray(0);
    }

	glutSwapBuffers();
}

GLvoid Reshape(int w, int h)
{
    width = w;
    height = h;
	glViewport(0, 0, w, h);
}