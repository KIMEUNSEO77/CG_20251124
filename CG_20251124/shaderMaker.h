#pragma once
#include <iostream>

#include <glew.h>
#include "filetobuf.h"

GLint width = 900, height = 900;
GLuint shaderProgramID;
GLuint vertexShader;
GLuint fragmentShader;

GLuint bgShaderProgramID;
GLuint bgVertexShader;
GLuint bgFragmentShader;

void make_vertexShaders()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("vertex.glsl");
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cerr << "Error: vertex shader            \n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("fragment.glsl");
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader            \n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, vertexShader);
	glAttachShader(shaderID, fragmentShader);

	glLinkProgram(shaderID);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program          \n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}


void make_vertexShaders_bg()
{
	GLchar* vertexSource;
	vertexSource = filetobuf("bg_vertex.glsl");
	bgVertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(bgVertexShader, 1, &vertexSource, NULL);
	glCompileShader(bgVertexShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(bgVertexShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(bgVertexShader, 512, NULL, errorLog);
		std::cerr << "Error: vertex shader            \n" << errorLog << std::endl;
		return;
	}
}

void make_fragmentShaders_bg()
{
	GLchar* fragmentSource;
	fragmentSource = filetobuf("bg_fragment.glsl");
	bgFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(bgFragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(bgFragmentShader);
	GLint result;
	GLchar errorLog[512];
	glGetShaderiv(bgFragmentShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(bgFragmentShader, 512, NULL, errorLog);
		std::cerr << "ERROR: frag_shader            \n" << errorLog << std::endl;
		return;
	}
}

GLuint make_shaderProgram_bg()
{
	GLint result;
	GLchar* errorLog = NULL;
	GLuint shaderID;
	shaderID = glCreateProgram();

	glAttachShader(shaderID, bgVertexShader);
	glAttachShader(shaderID, bgFragmentShader);

	glLinkProgram(shaderID);

	glDeleteShader(bgVertexShader);
	glDeleteShader(bgFragmentShader);

	glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
		std::cerr << "ERROR: shader program          \n" << errorLog << std::endl;
		return false;
	}
	glUseProgram(shaderID);
	return shaderID;
}
