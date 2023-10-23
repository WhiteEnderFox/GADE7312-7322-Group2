#ifndef OBJECTCONTAINER_H
#define OBJECTCONTAINER_H

#include <vector>
#include "BasicCubeMesh.h"
#include "basicCylinderMesh.h" 
#include "basicConeMesh.h"
#include "basicSphereMesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//struct to store which mesh has which offsets
struct MeshWithTransform {
	//this mesh is whatever mesh object your children are using
	//NOTE: all child meshes have to be made with the same mesh class, so same vertex properties (for now anyways, easier)
	basicCubeMesh mesh;
	/*basicConeMesh mesh;
	basicSphereMesh mesh;
	basicCylinderMesh mesh;*/
	glm::vec3 pos;
	glm::vec3 rot;
	glm::vec3 scale;

	//Constructor that sets default variables,
	//this is shorthand setting of variables, basically just a normal constructor dont worry
	MeshWithTransform(basicCubeMesh mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
		: mesh(mesh), pos(position), rot(rotation), scale(scale) {}

};

class ObjectContainer {
public:
	
	ObjectContainer() {}

	//function to add an object/mesh to this container
	void addMesh(basicCubeMesh mesh, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
		meshes.push_back(MeshWithTransform(mesh, position, rotation, scale));
	}
	
	
	void Draw(glm::mat4 model, Shader& shader) {
		for (MeshWithTransform mesh : meshes) { //normal foreach loop 

			// apply local transformations to the model matrix
			glm::mat4 localModel = glm::mat4(1.0f); // Initialize with the identity matrix

			// apply local translation
			localModel = glm::translate(localModel, mesh.pos);

			// apply local rotation
			localModel = glm::rotate(localModel, glm::radians(mesh.rot.x), glm::vec3(1.0f, 0.0f, 0.0f));
			localModel = glm::rotate(localModel, glm::radians(mesh.rot.y), glm::vec3(0.0f, 1.0f, 0.0f));
			localModel = glm::rotate(localModel, glm::radians(mesh.rot.z), glm::vec3(0.0f, 0.0f, 1.0f));

			// apply local scale
			localModel = glm::scale(localModel, mesh.scale);

			// combine the local transformations with the provided model matrix
			// this means that you do animation on the whole matrix before moving the children around
			glm::mat4 finalModel = model * localModel;

			// set the model matrix in the shader
			shader.setMat4("model", finalModel);

			// render the mesh with the global + local matrix
			mesh.mesh.Draw(shader);
		}
	}

private:
	//private string to hold all objects for this container (including offsets/transforms)
	std::vector<MeshWithTransform> meshes;
};

#endif