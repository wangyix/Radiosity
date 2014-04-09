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
	float *positions;
	int *ids;
	int *indices;

public:

	QuadMesh();
	~QuadMesh();

	int loadMesh(char *filepath);

	int getNumQuads() const;

private:
	void updateVerticesArrays();
};