#include <glew.h>
#include <freeglut.h>
#include <freeglut_ext.h> 
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "filetobuf.h"
#include "shaderMaker.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void make_vertexShaders();
void make_fragmentShaders();
GLuint make_shaderProgram();
GLvoid drawScene();
GLvoid Reshape(int w, int h);

GLuint tex_1, tex_2, tex_3, tex_4, tex_5, tex_6;

GLuint cubeVAO = 0, cubeVBO = 0;       // 정육면체
GLuint pyramidVAO = 0, pyramidVBO = 0; // 삼각뿔
bool cubeMode = true;
bool lightMode = true;  // 조명 켜기/끄기

bool rotatingY = false; float angleY = 0.0f;
bool rotatingX = false; float angleX = 0.0f;

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

void pushVertex(std::vector<GLfloat>& vtx, const glm::vec3& p, const glm::vec3& n, const glm::vec2& uv)
{
	vtx.push_back(p.x); vtx.push_back(p.y); vtx.push_back(p.z);   // position
	vtx.push_back(n.x); vtx.push_back(n.y); vtx.push_back(n.z);   // normal
	vtx.push_back(uv.x); vtx.push_back(uv.y);   // texcoord
}

void InitCube()
{
    std::vector<GLfloat> vertices;

    for (int i = 0; i < 6; i++)
    {
        int v0 = faces[i][0], v1 = faces[i][1], v2 = faces[i][2], v3 = faces[i][3];

        glm::vec3 p0(cube[v0][0], cube[v0][1], cube[v0][2]);
        glm::vec3 p1(cube[v1][0], cube[v1][1], cube[v1][2]);
        glm::vec3 p2(cube[v2][0], cube[v2][1], cube[v2][2]);
        glm::vec3 p3(cube[v3][0], cube[v3][1], cube[v3][2]);

        // 면 노말
        glm::vec3 n = glm::normalize(glm::cross(p2 - p0, p1 - p0));

        // 간단히: 한 면의 4개 꼭짓점 uv
        glm::vec2 uv0(0.0f, 0.0f);
        glm::vec2 uv1(1.0f, 0.0f);
        glm::vec2 uv2(1.0f, 1.0f);
        glm::vec2 uv3(0.0f, 1.0f);

        // 삼각형 1: v0, v1, v2
        pushVertex(vertices, p0, n, uv0);
        pushVertex(vertices, p1, n, uv1);
        pushVertex(vertices, p2, n, uv2);
        // 삼각형 2: v0, v2, v3
        pushVertex(vertices, p0, n, uv0);
        pushVertex(vertices, p2, n, uv2);
        pushVertex(vertices, p3, n, uv3);
    }

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * vertices.size(),
        vertices.data(),
        GL_STATIC_DRAW);

    // position : location = 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal : location = 1
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texcoord : location = 2
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    cubeVertexCount = static_cast<int>(vertices.size() / 8);
}


int pyramidVertexCount = 0;

void InitPyramid()
{
    std::vector<GLfloat> vertices;

    // 꼭대기 0, 바닥 1~4 → 총 5개의 정점 노멀
    glm::vec3 vNormals[5] = {
        glm::vec3(0), glm::vec3(0),
        glm::vec3(0), glm::vec3(0), glm::vec3(0)
    };

    // --- 면 노멀 누적 함수 ------------------------------------------------
    auto addFaceNormal = [&](int ia, int ib, int ic)
        {
            glm::vec3 p0(pyramid[ia][0], pyramid[ia][1], pyramid[ia][2]);
            glm::vec3 p1(pyramid[ib][0], pyramid[ib][1], pyramid[ib][2]);
            glm::vec3 p2(pyramid[ic][0], pyramid[ic][1], pyramid[ic][2]);

            glm::vec3 fn = glm::normalize(glm::cross(p2 - p0, p1 - p0));
            vNormals[ia] += fn;
            vNormals[ib] += fn;
            vNormals[ic] += fn;
        };

    // --- 옆면 노멀 누적 ---------------------------------------------------
    addFaceNormal(0, 3, 2);
    addFaceNormal(0, 2, 1);
    addFaceNormal(0, 4, 3);
    addFaceNormal(0, 1, 4);

    // --- 정점별 노멀 정규화 ----------------------------------------------
    for (int i = 0; i < 5; i++)
        vNormals[i] = glm::normalize(vNormals[i]);

    // --- push 함수 (여기서 vNormals 사용 가능) --------------------------
    auto push = [&](int ia, int ib, int ic, bool isBase,
        const glm::vec2& t0,
        const glm::vec2& t1,
        const glm::vec2& t2)
        {
            glm::vec3 p0(pyramid[ia][0], pyramid[ia][1], pyramid[ia][2]);
            glm::vec3 p1(pyramid[ib][0], pyramid[ib][1], pyramid[ib][2]);
            glm::vec3 p2(pyramid[ic][0], pyramid[ic][1], pyramid[ic][2]);

            if (isBase)
            {
                glm::vec3 n(0.0f, -1.0f, 0.0f);  // 바닥 평면 노멀
                pushVertex(vertices, p0, n, t0);
                pushVertex(vertices, p1, n, t1);
                pushVertex(vertices, p2, n, t2);
            }
            else
            {
                // 여기서 vNormals 사용해도 이제 오류 없음!!
                pushVertex(vertices, p0, vNormals[ia], t0);
                pushVertex(vertices, p1, vNormals[ib], t1);
                pushVertex(vertices, p2, vNormals[ic], t2);
            }
        };

    // --- 바닥 두 삼각형 ---------------------------------------------------
    push(1, 2, 3, true,
        glm::vec2(0, 0),
        glm::vec2(1, 0),
        glm::vec2(1, 1));

    push(1, 3, 4, true,
        glm::vec2(0, 0),
        glm::vec2(1, 1),
        glm::vec2(0, 1));

    // --- 옆면 네 삼각형 ---------------------------------------------------
    glm::vec2 top(0.5f, 1.0f);

    push(0, 3, 2, false, top, glm::vec2(1, 0), glm::vec2(0, 0));
    push(0, 2, 1, false, top, glm::vec2(0, 0), glm::vec2(1, 0));
    push(0, 4, 3, false, top, glm::vec2(0, 0), glm::vec2(1, 0));
    push(0, 1, 4, false, top, glm::vec2(1, 0), glm::vec2(0, 0));

    // --- VBO/VAO ----------------------------------------------------------
    glGenVertexArrays(1, &pyramidVAO);
    glGenBuffers(1, &pyramidVBO);

    glBindVertexArray(pyramidVAO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GLfloat) * vertices.size(),
        vertices.data(),
        GL_STATIC_DRAW);

    // pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texcoord
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    pyramidVertexCount = static_cast<int>(vertices.size() / 8);
}



// 텍스처
GLuint LoadTexture(const char* filename)
{
    int width, height, channels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filename, &width, &height, &channels, 0);
    if (!data)
    {
        std::cout << "Failed to load: " << filename << "\n";
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    // 필터링 & 래핑
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = GL_RGB;
    if (channels == 4) format = GL_RGBA;
    else if (channels == 3) format = GL_RGB;
    else if (channels == 1) format = GL_RED;

    glTexImage2D(GL_TEXTURE_2D,
        0,
        format,
        width,
        height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    return texID;   // 텍스처 ID를 리턴
}

void Timer(int value)
{
    if (rotatingY) angleY += 1.0f;
	if (rotatingX) angleX += 1.0f;

	glutPostRedisplay();
	glutTimerFunc(16, Timer, 0);
}
void Keboard(unsigned char key, int x, int y)
{
    switch (key)
    {
	case 'c': cubeMode = true; glutPostRedisplay(); break;
	case 'p': cubeMode = false; glutPostRedisplay(); break;
    case 'y': rotatingY = !rotatingY; rotatingX = false;  break;
    case 'x': rotatingX = !rotatingX; rotatingY = false; break;
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

	tex_1 = LoadTexture("tex_1.png");
	tex_2 = LoadTexture("tex_2.png");
	tex_3 = LoadTexture("tex_3.png");
	tex_4 = LoadTexture("tex_4.png");
	tex_5 = LoadTexture("tex_5.png");
	tex_6 = LoadTexture("tex_6.png");

	// callback 함수 등록
    glutDisplayFunc(drawScene);
    glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keboard);
	glutTimerFunc(0, Timer, 0);

	glEnable(GL_DEPTH_TEST); // 깊이 테스트 활성화

	make_vertexShaders();
	make_fragmentShaders();
	shaderProgramID = make_shaderProgram();

	glutMainLoop();
}

GLvoid drawScene()
{
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
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
    model = glm::translate(model, glm::vec3(0.0f, 0.15f, 0.0f));
    // 회전 적용
    model = glm::rotate(model, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::translate(model, glm::vec3(0.0f, -0.15f, 0.0f));

    GLuint modelLoc = glGetUniformLocation(shaderProgramID, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    glActiveTexture(GL_TEXTURE0);
    if (cubeMode)
    {
        glBindVertexArray(cubeVAO);

        glBindTexture(GL_TEXTURE_2D, tex_1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, tex_2);
		glDrawArrays(GL_TRIANGLES, 6, 6);

		glBindTexture(GL_TEXTURE_2D, tex_3);
		glDrawArrays(GL_TRIANGLES, 12, 6);

		glBindTexture(GL_TEXTURE_2D, tex_4);
		glDrawArrays(GL_TRIANGLES, 18, 6);
		glBindTexture(GL_TEXTURE_2D, tex_5);
		glDrawArrays(GL_TRIANGLES, 24, 6);
		glBindTexture(GL_TEXTURE_2D, tex_6);
		glDrawArrays(GL_TRIANGLES, 30, 6);

        glBindVertexArray(0);
    }

    else
    {
        glBindVertexArray(pyramidVAO);

        glBindTexture(GL_TEXTURE_2D, tex_1);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindTexture(GL_TEXTURE_2D, tex_2);
        glDrawArrays(GL_TRIANGLES, 6, 6);

        glBindTexture(GL_TEXTURE_2D, tex_3);
        glDrawArrays(GL_TRIANGLES, 12, 6);

        glBindTexture(GL_TEXTURE_2D, tex_4);
        glDrawArrays(GL_TRIANGLES, 18, 6);
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