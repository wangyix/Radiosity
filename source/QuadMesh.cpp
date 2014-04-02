#include "QuadMesh.h"

const float QuadMesh::positions[12] =  {	0.0, 0.0f, 0.0f,
											0.0, 1.0f, 0.0f,
											1.0, 1.0f, 0.0f,
											1.0, 0.0f, 0.0f };

const float QuadMesh::texcoords[8] =  {	0.0, 0.0f,
										0.0, 1.0f,
										1.0, 1.0f,
										1.0, 0.0f };

const unsigned short QuadMesh::indices[6] = {	0, 1, 2,
												0, 2, 3 };


QuadMesh::QuadMesh()
//	: quads(NULL), numQuads(0)
{

	// TEST !!!!!!!!!!!!!
	numQuads = 6;
	quads = new Quad[6];

	glm::vec3 cubeCorners[8];
	cubeCorners[0] = glm::vec3(0.0f, 0.0f, 0.0f);
	cubeCorners[1] = glm::vec3(1.0f, 0.0f, 0.0f);
	cubeCorners[2] = glm::vec3(1.0f, 1.0f, 0.0f);
	cubeCorners[3] = glm::vec3(0.0f, 1.0f, 0.0f);
	cubeCorners[4] = glm::vec3(0.0f, 0.0f, 1.0f);
	cubeCorners[5] = glm::vec3(1.0f, 0.0f, 1.0f);
	cubeCorners[6] = glm::vec3(1.0f, 1.0f, 1.0f);
	cubeCorners[7] = glm::vec3(0.0f, 1.0f, 1.0f);

	quads[0].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[3]);
	quads[1].setModel(cubeCorners[4], cubeCorners[5], cubeCorners[7]);
	quads[2].setModel(cubeCorners[0], cubeCorners[1], cubeCorners[4]);
	quads[3].setModel(cubeCorners[1], cubeCorners[2], cubeCorners[5]);
	quads[4].setModel(cubeCorners[2], cubeCorners[3], cubeCorners[6]);
	quads[5].setModel(cubeCorners[3], cubeCorners[0], cubeCorners[7]);


}

void QuadMesh::setVerticesSceneShader(SceneShaderInterface &ssi) const{
	ssi.setVertices(4, positions, texcoords, 6, indices);
}

void QuadMesh::renderWithSceneShader(SceneShaderInterface &ssi, glm::mat4 viewProj) const {
	
	// assume ssi has been bound already
	// assume ssi already has vertices

	for (int i=0; i<numQuads; i++) {
		ssi.setModelViewProj(viewProj * quads[i].getModel());
		ssi.setTexture(quads[i].getRadiosityTex());
		ssi.draw();
	}
}


QuadMesh::~QuadMesh() {
	if (quads!=NULL)
		delete quads;
}