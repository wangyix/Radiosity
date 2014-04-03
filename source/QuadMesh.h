#pragma once

#include "glincludes.h"
#include "Quad.h"
//#include "SceneShaderInterface.h"

class QuadMesh {

//private:
public:
	int numQuads;
	Quad *quads;

public:
	static const float positions[];
	static const float texcoords[];
	static const unsigned short indices[];

public:

	QuadMesh();
	~QuadMesh();

	/*
	void setVerticesSceneShader(SceneShaderInterface *ssi) const;
	void renderWithSceneShader(SceneShaderInterface *ssi, glm::mat4 viewProj) const;
	*/
};