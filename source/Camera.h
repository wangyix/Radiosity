#pragma once

#include "glincludes.h"

class Camera {
private:
	glm::vec3 position;
	glm::vec3 right;
	glm::vec3 up;
	glm::vec3 lookNeg;

	float nearPlane;
	float farPlane;
	float fovy;
	float aspect;

	glm::mat4 proj;

public:
	Camera();
	~Camera();

	void setPosition(const glm::vec3 &position);

	void setLens(float nearplane, float farplane, float fovy);
	void setAspect(float aspect);

	glm::vec3 moveForward(float Dist);
	glm::vec3 moveRight(float Dist);
	glm::vec3 moveUp(float Dist);

	void rotateRight(float deg);
	void rotateUp(float deg);
	void lookAt(const glm::vec3 &target);
	

	glm::mat4 getViewProj() const;

private:
	void updateProj();
	void orthonormalize();
};