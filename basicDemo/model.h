#pragma once
#include <glad/glad.h> // Glad has to be include before glfw
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

class model
{

public:
	//string name;
	std::vector<glm::vec3> vertex, normal, color, tangent, bitangent, allVert;
	std::vector<glm::vec2> uv;
	int numVertex;
	// Index (GPU) of the geometry buffer
	unsigned int VBO[5];
	// Index (GPU) vertex array object
	unsigned int VAO[1];
	//Propiedades de color del modelo
	glm::vec3 kamb; //Componente ambiental 
	glm::vec3 kdif; //Componente difusa
	glm::vec3 kspec;//Componente especular
	glm::vec3 position;//position (default=vec3(0.0f))

	char shaderType;
	bool albedo;
	float shinniness;
	float roughness;
	float indexMaterial;
	float indexAmbient;
	float intensityParalax;
	float percentAmbient;
	//Textures 
	struct Texture
	{
		unsigned int diffuse = 1;
		unsigned int specular = 1;
		unsigned int normal = 1;
		unsigned int disp = 1;
		unsigned int blend = 1;
	};
	Texture texture;
	model();
	model(glm::vec3 pos);
	~model();

	model* loadObj(std::string path, glm::vec3 position);
	bool loadObj(std::string path, std::vector<glm::vec3>& vert, std::vector<glm::vec3>& norm, std::vector<glm::vec2>& uvText);
	void loadMTL(std::string path);

	//tangents and bitangents
	void getTangentBitanget(std::vector<glm::vec3>& vertices, std::vector<glm::vec2>& uvs, std::vector<glm::vec3>& normals, std::vector<glm::vec3>& tangents, std::vector<glm::vec3>& bitangents, std::vector<glm::vec3>& allVert);

	
	Texture getTexture();
	void setTexture(unsigned int diffID);
	void setTexture(unsigned int diffID, unsigned int specID);
	glm::vec3 getPosition();
	void setPosition(glm::vec3 pos);
	glm::vec3 getKAmbient();
	void setKAmbient(glm::vec3 ambC);
	glm::vec3 getKDiffuse();
	void setKDiffuse(glm::vec3 difC);
	glm::vec3 getKSpecular();
	void setKSpecular(glm::vec3 speC);
	float getShinniness();
	void setShinniness(float n);
	float getRoughness();
	void setRoghness(float n);
	float getindexMaterial();
	void setindexMaterial(float n);
	float getindexAmbient();
	void setindexAmbient(float n);
	float getintensityParalax();
	void setintensityParalax(float n);
	float getpercentAmbient();
	void setpercentAmbient(float n);
	bool getAlbedo();
	void setAlbedo(bool n);
	char getShader();
	void setShader(char n);

private:

};

