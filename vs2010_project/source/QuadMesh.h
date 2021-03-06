#pragma once

#include "glincludes.h"
#include "Quad.h"
#include "Utils.h"
#include <vector>
#include <fstream>
#include <string>
#include <vector>

class QuadMesh {

private:

	std::vector<Quad> quads;

	// vertices arrays
	std::vector<float> positions;
	std::vector<float> texcoords;
	std::vector<unsigned int> ids;

public:

	QuadMesh();

	void load(char *filepath);
	void unload();


	void subdivideQuad(int i, Quad **bl_ptr, Quad **br_ptr, Quad **tl_ptr, Quad **tr_ptr);


	int getNumQuads() const;
	int getNumVertices() const;

	Quad *getQuad(int i);

	const float *getPositionsArray() const;
	const float *getTexcoordsArray() const;
	const unsigned int *getIdsArray() const;

	int getBaseVertex(int quadIndex) const;

private:

	static bool getNextDataLine(std::ifstream &ifs, std::string &line);

	void updateVerticesArrays();

	void updatePositionsOfQuad(int i);
	void updateTexcoordsOfQuad(int i);
	void updateIdsOfQuad(int i);
};