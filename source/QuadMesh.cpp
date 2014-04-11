#include "QuadMesh.h"





QuadMesh::QuadMesh()
	: quads(), positions(), texcoords(), ids() {
}


// returns number of quads
void QuadMesh::load(char* filepath) {

	std::ifstream ifs(filepath);
	std::string line;
	
	unload();

	std::vector<glm::vec3> positions;
	
	// read lines until @ is found, indicating start of first set of vertices
	while (getNextDataLine(ifs, line)) {
		if (line[0]=='@')
			break;
	}


	while (!ifs.eof()) {
	
		// read vertices until @ is found, indicating start of quads info
		while (getNextDataLine(ifs, line)) {

			if (line[0]=='@')
				break;

			// read xyz position, add to list of positions
			float x, y, z;
			sscanf_s(line.c_str(), "%f %f %f", &x, &y, &z);
			positions.push_back(glm::vec3(x, y, z));
		}


		// read quads info until @ is found, indicating start of next
		// set of vertices/quads

		while (getNextDataLine(ifs, line)) {

			if (line[0]=='@')
				break;

			// read corner indices, width and height
			int bl, br, tl, w, h;
			float rr, rg, rb;
			int numEmitting;
			sscanf_s(line.c_str(), "%d %d %d %d %d %f %f %f %d",
				&bl, &br, &tl, &w, &h, &rr, &rg, &rb, &numEmitting);
			
			// read emittance info (if any), record emittance of each quad
			std::vector<glm::vec3> emittances = std::vector<glm::vec3>(h*w, glm::vec3(0.0f, 0.0f, 0.0f));
			int i, j;
			float er, eg, eb;
			int linesRead = 0;
			while (linesRead<numEmitting && getNextDataLine(ifs, line)) {
				sscanf_s(line.c_str(), "%d %d %f %f %f", &i, &j, &er, &eg, &eb);
				emittances[i*w+j] = glm::vec3(er, eg, eb);
				linesRead++;
			}

			// get bottomleft, bottomright, topleft positions, reflectance
			glm::vec3 bottomLeft = positions[bl];
			glm::vec3 bottomRight = positions[br];
			glm::vec3 topLeft = positions[tl];
			glm::vec3 reflectance = glm::vec3(rr, rg, rb);
			
			// create grid of quads
			glm::vec3 u = (bottomRight-bottomLeft) / (float)w;
			glm::vec3 v = (topLeft-bottomLeft) / (float)h;
			glm::vec3 rowBase = bottomLeft;
			glm::vec3 base;
			int k = 0;
			for (int i=0; i<h; i++) {
				base = rowBase;
				for (int j=0; j<w; j++) {
					quads.push_back(Quad(base, base+u, base+v,
							reflectance));
					quads.back().initTextures(emittances[i*w+j]);
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


void QuadMesh::unload() {

	for (unsigned int i=0; i<quads.size(); i++) {
		quads[i].closeTextures();
	}
	quads.clear();
}


int QuadMesh::getNumQuads() const {
	return quads.size();
}

int QuadMesh::getNumVertices() const {
	return 4*quads.size();
}

const Quad &QuadMesh::getQuad(int i) const {
	return quads[i];
}

const float *QuadMesh::getPositionsArray() const {
	return &positions[0];
}

const float *QuadMesh::getTexcoordsArray() const {
	return &texcoords[0];
}

const unsigned int *QuadMesh::getIdsArray() const {
	return &ids[0];
}

int QuadMesh::getBaseVertex(int quadIndex) const {
	return 4*quadIndex;
}

// reads the next non-blank, non-comment line from stream
// returns false if eof found
bool QuadMesh::getNextDataLine(std::ifstream &ifs, std::string &line) {
	while (std::getline(ifs, line)) {
		line = Utils::trim(line);
		// skip blank lines and comments
		if (line.length()!=0 && line[0]!='#')
			break;
	}
	return !ifs.eof();
}


void QuadMesh::updateVerticesArrays() {
	
	positions.resize(12*quads.size());
	texcoords.resize(8*quads.size());
	ids.resize(4*quads.size());

	unsigned int iPos = 0;
	unsigned int iTex = 0;
	unsigned int iId = 0;

	for (unsigned int i=0; i<quads.size(); i++) {
		
		glm::vec3 corners[4];
		corners[0] = quads[i].getPosition();
		corners[1] = corners[0] + quads[i].getU();
		corners[2] = corners[1] + quads[i].getV();
		corners[3] = corners[0] + quads[i].getV();

		for (unsigned int j=0; j<4; j++) {
			positions[iPos++] = corners[j].x;
			positions[iPos++] = corners[j].y;
			positions[iPos++] = corners[j].z;
			ids[iId++] = i;
		}

		texcoords[iTex++] = 0.0f;
		texcoords[iTex++] = 0.0f;
		texcoords[iTex++] = 1.0f;
		texcoords[iTex++] = 0.0f;
		texcoords[iTex++] = 1.0f;
		texcoords[iTex++] = 1.0f;
		texcoords[iTex++] = 0.0f;
		texcoords[iTex++] = 1.0f;
	}
}