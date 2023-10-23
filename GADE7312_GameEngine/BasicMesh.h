#pragma once
#ifndef BASICMESH_H
#define BASICMESH_H

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <vector>

using namespace std;

struct BasicVertex 
{
	//Position Attribute
	glm::vec3 Position;
	//Colour Attribute
	glm::vec3 Colours;
	//texture co-ords
	glm::vec2 Tex;
};

class BasicMesh {
public:
	vector<BasicVertex>	vertices;
	vector<unsigned int> indices;

	unsigned int VAO;

	BasicMesh(vector<BasicVertex> vertices, vector<unsigned int> indices) 
	{
		this->vertices = vertices;
		this->indices = indices;

		setupMesh();

	}

	void Draw(Shader& shader) 
	{
		shader.use();
		shader.setVec3("colourIn", glm::vec3(sin(glfwGetTime()), cos(glfwGetTime()), tan(glfwGetTime())));

		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);

		glBindVertexArray(VAO);
		//glDrawArrays(GL_TRIANGLES, 0, 3);
	}

private:
	unsigned int VBO, EBO;
	
	void setupMesh() 
	{
		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		//glGenBuffers(1, &EBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(BasicVertex), &vertices[0], GL_DYNAMIC_DRAW);

		//EBO bind and setup (indices)
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		//ATTRIBUTE pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Position));
		glEnableVertexAttribArray(0);

		//COLOUR pointers
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Colours));
		glEnableVertexAttribArray(1);

		//TEXTURE pointers
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(BasicVertex), (void*)offsetof(BasicVertex, Tex));
		glEnableVertexAttribArray(2);

		

	}


};


#endif // !BASICMESH_H
