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
	std::vector<int> ids;

public:

	QuadMesh();

	void load(char *filepath);
	void unload();

	int getNumQuads() const;
	int getNumVertices() const;

	const Quad &getQuad(int i) const;

	const float *getPositionsArray() const;
	const float *getTexcoordsArray() const;
	const int *getIdsArray() const;

	int getBaseVertex(int quadIndex) const;

private:

	static bool getNextDataLine(std::ifstream &ifs, std::string &line);

	void updateVerticesArrays();
};