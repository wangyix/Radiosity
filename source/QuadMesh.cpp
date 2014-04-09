#include "QuadMesh.h"

QuadMesh::QuadMesh()
	: quads(), positions(0), ids(0), indices(0) {
}

QuadMesh::~QuadMesh() {
	delete[] positions;
	delete[] ids;
	delete[] indices;
}

// returns number of quads
int QuadMesh::loadMesh(char* filepath) {

	std::ifstream ifs(filepath);
	std::string line;

	quads.clear();

	std::vector<glm::vec3> positions;

	while (!ifs.eof()) {

		// read lines until line starting with @ is found, indicating start of vertices
		while (std::getline(ifs, line)) {
			line = Utils::trim(line);
			if (line[0]=='@')
				break;
		}
	
		// read vertices
		while (std::getline(ifs, line)) {
			line = Utils::trim(line);

			// skip comments
			if (line[0]=='#')
				continue;

			// read xyz position, add to list of positions
			float x, y, z;
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			positions.push_back(glm::vec3(x, y, z));
		}

		// read lines until line starting with @ is found, indicating start of indices
		while (std::getline(ifs, line)) {
			line = Utils::trim(line);
			if (line[0]=='@')
				break;
		}

		// read indices, width and height, create quads
		while (std::getline(ifs, line)) {
			line = Utils::trim(line);

			// skip comments
			if (line[0]=='#')
				continue;

			// read corner indices, width and height
			int bl, br, tl, w, h;
			float r, g, b;
			sscanf_s(line.c_str(), "%d %d %d %d %d %f %f %f", &bl, &br, &tl, &w, &h, &r, &g, &b);
			
			// get bottomleft, bottomright, topleft positions
			glm::vec3 bottomLeft = positions[bl];
			glm::vec3 bottomRight = positions[br];
			glm::vec3 topLeft = positions[tl];
			glm::vec3 reflectance = glm::vec3(r, g, b);

			glm::vec3 u = (bottomRight-bottomLeft) / (float)w;
			glm::vec3 v = (topLeft-bottomLeft) / (float)h;

			glm::vec3 rowBase = bottomLeft;
			glm::vec3 base;
			int k = 0;
			for (int i=0; i<h; ++i) {
				base = rowBase;
				for (int j=0; j<w; ++j) {
					quads.push_back(Quad(base, base+u, base+v,
							reflectance, glm::vec3(0.0f,0.0f,0.0f)));	// CHANGE EMITTANCE!!!!
					base += u;
				}
				rowBase += v;
			}
		}

		positions.clear();
	}
	ifs.close();
	updateVerticesArrays();
}


void QuadMesh::updateVerticesArrays() {
	
	delete[] positions;
	delete[] ids;
	delete[] indices;

	positions = new float[12*quads.size()];
	ids = new int[4*quads.size()];
	indices = new int[6*quads.size()];

	int numPositions = 0;
	int numIds = 0;
	int numIndices = 0;
	for (int i=0; i<quads.size(); i++) {
		
		glm::vec3 corners[4];
		corners[0] = quads[i].getPosition();
		corners[1] = corners[0] + quads[i].getU();
		corners[2] = corners[1] + quads[i].getV();
		corners[3] = corners[0] + quads[i].getV();

		for (int j=0; j<4; j++) {
			positions[numPositions++] = corners[j].x;
			positions[numPositions++] = corners[j].y;
			positions[numPositions++] = corners[j].z;
			ids[numIds++] = i;
		}

		int baseIndex = 4*i;
		indices[numIndices++] = baseIndex;
		indices[numIndices++] = baseIndex + 1;
		indices[numIndices++] = baseIndex + 2;
		indices[numIndices++] = baseIndex;
		indices[numIndices++] = baseIndex + 2;
		indices[numIndices++] = baseIndex + 3;
	}
}