#include "QuadMesh.h"





QuadMesh::QuadMesh()
	: quads(), positions(), texcoords(), ids() {
}


// returns number of quads
void QuadMesh::load(char* filepath) {

	std::ifstream ifs(filepath);
	if (ifs.fail()) {
		fprintf(stderr, "file %s could not be opened.\n", filepath);
		printf("Press enter to exit...");
		getchar();
		exit(EXIT_FAILURE);
	}
	
	unload();	// destroy current mesh

	std::vector<glm::vec3> positions;
	std::string line;
	
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
			int bl, br, tl, cols, rows;
			float rr, rg, rb;
			int numEmitting;
			sscanf_s(line.c_str(), "%d %d %d %d %d %f %f %f %d",
				&bl, &br, &tl, &cols, &rows, &rr, &rg, &rb, &numEmitting);
			
			// read emittance info (if any), record emittance of each quad
			std::vector<glm::vec3> powers = std::vector<glm::vec3>(cols*rows,
					glm::vec3(0.0f, 0.0f, 0.0f));
			int col, row;
			float er, eg, eb;
			int linesRead = 0;
			while (linesRead<numEmitting && getNextDataLine(ifs, line)) {
				sscanf_s(line.c_str(), "%d %d %f %f %f", &col, &row, &er, &eg, &eb);
				powers[col*rows+row] = glm::vec3(er, eg, eb);
				linesRead++;
			}

			// get bottomleft, bottomright, topleft positions, reflectance
			glm::vec3 bottomLeft = positions[bl];
			glm::vec3 bottomRight = positions[br];
			glm::vec3 topLeft = positions[tl];
			glm::vec3 reflectance = glm::vec3(rr, rg, rb);
			
			// create grid of quads
			glm::vec3 u = (bottomRight-bottomLeft) / (float)cols;
			glm::vec3 v = (topLeft-bottomLeft) / (float)rows;
			glm::vec3 colBase = bottomLeft;
			glm::vec3 base;
			for (int i=0; i<cols; i++) {
				base = colBase;
				for (int j=0; j<rows; j++) {
					quads.resize(quads.size()+1);
					float area = glm::length(u) * glm::length(v);
					quads.back().init(base, u, v, reflectance, powers[i*rows+j] / area);
					base += v;
				}
				colBase += u;
			}

		}
		positions.clear();
	}
	ifs.close();
	updateVerticesArrays();
}


void QuadMesh::unload() {

	for (unsigned int i=0; i<quads.size(); i++) {
		quads[i].close();
	}
	quads.clear();
}


void QuadMesh::subdivideQuad(int i, Quad **bl_ptr, Quad **br_ptr, Quad **tl_ptr, Quad **tr_ptr) {

	// make space for 3 new quads (must do this before getting quad pointers)
	int newQuadIndex = quads.size();
	quads.resize(quads.size()+3);
	positions.resize(12*quads.size());
	texcoords.resize(8*quads.size());
	ids.resize(4*quads.size());

	// get old quad properties
	Quad *oldQuad = &quads[i];
	glm::vec3 halfU = oldQuad->getU() * 0.5f;
	glm::vec3 halfV = oldQuad->getV() * 0.5f;
	glm::vec3 position = oldQuad->getPosition();
	glm::vec3 reflectance = oldQuad->getReflectance();

	int parentId = oldQuad->getParentId();
	int nextSubdivideLevel = oldQuad->getSubdivideLevel() + 1;

	// update geometry of original quad to become the bottom-left quadrant
	*bl_ptr = oldQuad;
	oldQuad->setU(halfU);
	oldQuad->setV(halfV);
	oldQuad->setSubdivideLevel(nextSubdivideLevel);
	// update positions array
	updatePositionsOfQuad(i);

	
	// create new quad for bottom-right quadrant
	*br_ptr = &quads[newQuadIndex];
	quads[newQuadIndex].init(position+halfU, halfU, halfV,
			reflectance, parentId, nextSubdivideLevel);
	updatePositionsOfQuad(newQuadIndex);
	updateTexcoordsOfQuad(newQuadIndex);
	updateIdsOfQuad(newQuadIndex);

	newQuadIndex++;

	// create new quad for top-left quadrant
	*tl_ptr = &quads[newQuadIndex];
	quads[newQuadIndex].init(position+halfV, halfU, halfV,
			reflectance, parentId, nextSubdivideLevel);
	updatePositionsOfQuad(newQuadIndex);
	updateTexcoordsOfQuad(newQuadIndex);
	updateIdsOfQuad(newQuadIndex);

	newQuadIndex++;

	// create new quad for top-right quadrant
	*tr_ptr = &quads[newQuadIndex];
	quads[newQuadIndex].init(position+halfU+halfV, halfU, halfV,
			reflectance, parentId, nextSubdivideLevel);
	updatePositionsOfQuad(newQuadIndex);
	updateTexcoordsOfQuad(newQuadIndex);
	updateIdsOfQuad(newQuadIndex);
}


int QuadMesh::getNumQuads() const {
	return quads.size();
}

int QuadMesh::getNumVertices() const {
	return 4*quads.size();
}

Quad *QuadMesh::getQuad(int i){
	return &quads[i];
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

	for (unsigned int i=0; i<quads.size(); i++) {
		updatePositionsOfQuad(i);
		updateTexcoordsOfQuad(i);
		updateIdsOfQuad(i);
	}
}


void QuadMesh::updatePositionsOfQuad(int i) {

	Quad *q = &quads[i];
	unsigned int base = 12*i;

	glm::vec3 vertexPos = q->getPosition();
	positions[base++] = vertexPos.x;
	positions[base++] = vertexPos.y;
	positions[base++] = vertexPos.z;
	vertexPos += q->getU();
	positions[base++] = vertexPos.x;
	positions[base++] = vertexPos.y;
	positions[base++] = vertexPos.z;
	vertexPos += q->getV();
	positions[base++] = vertexPos.x;
	positions[base++] = vertexPos.y;
	positions[base++] = vertexPos.z;
	vertexPos = q->getPosition() + q->getV();
	positions[base++] = vertexPos.x;
	positions[base++] = vertexPos.y;
	positions[base++] = vertexPos.z;
}


void QuadMesh::updateTexcoordsOfQuad(int i) {
	unsigned int base = 8*i;
	texcoords[base++] = 0.0f;
	texcoords[base++] = 0.0f;
	texcoords[base++] = 1.0f;
	texcoords[base++] = 0.0f;
	texcoords[base++] = 1.0f;
	texcoords[base++] = 1.0f;
	texcoords[base++] = 0.0f;
	texcoords[base++] = 1.0f;
}

void QuadMesh::updateIdsOfQuad(int i) {
	unsigned int id = quads[i].getId();
	unsigned int base = 4*i;
	ids[base++] = id;
	ids[base++] = id;
	ids[base++] = id;
	ids[base++] = id;
}