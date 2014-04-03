#include "Camera.h"

Camera::Camera()
	: nearPlane(0.1f), farPlane(1000.0f), fovy(45.0f), aspect(4.0f/3.0f) {
	position = glm::vec3(0.0f, 0.0f, 0.0f);
	right = glm::vec3(1.0f, 0.0f, 0.0f);
	up = glm::vec3(0.0f, 0.0f, 1.0f);
	lookNeg = glm::vec3(0.0f, -1.0f, 0.0f);

	updateProj();
}

Camera::~Camera() {
}


void Camera::setPosition(const glm::vec3 &position) {
	this->position = position;
}

void Camera::setLens(float nearPlane, float farPlane, float fovy) {
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fovy = fovy;
	updateProj();
}

void Camera::setAspect(float aspect) {
	this->aspect = aspect;
	updateProj();
}

glm::vec3 Camera::moveForward(float dist) {
	position -= dist*lookNeg;
	return position;
}

glm::vec3 Camera::moveRight(float dist) {
	position += dist*right;
	return position;
}

glm::vec3 Camera::moveUp(float dist) {	// z is world up
	position[2] += dist;
	return position;
}

void Camera::rotateRight(float deg) {
	glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	right = glm::rotate(right, -deg, worldUp);
	lookNeg = glm::rotate(lookNeg, -deg, worldUp);
	orthonormalize();
}

void Camera::rotateUp(float deg) {
	
	up = glm::rotate(up, deg, right);
	lookNeg = glm::rotate(lookNeg, deg, right);
	
	// prevent rotating past straight-up or straight-down
	glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	if (glm::dot(up, worldUp) < 0.0f) {
		// set look direction to straight-up or -down, whichever's closer
		lookNeg = glm::dot(lookNeg, worldUp) > 0.0f ? worldUp : -worldUp;
		up = -glm::cross(right, lookNeg);
	}

	orthonormalize();
}

void Camera::lookAt(const glm::vec3 &target) {

	glm::vec3 toTarget = target - position;
	if (glm::length(toTarget) == 0)
		return;

	lookNeg = -glm::normalize(toTarget);
	glm::vec3 worldUp = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec3 rightNew = glm::cross(worldUp, lookNeg);
	
	if (glm::length(rightNew) == 0.0f) {
		// if we're looking straight-up or -down,
		up = glm::cross(lookNeg, right);
	} else {
		right = glm::normalize(rightNew);
		up = glm::cross(lookNeg, right);
	}
}


void Camera::updateProj() {
	proj = glm::perspective(fovy, aspect, nearPlane, farPlane);
}

void Camera::orthonormalize() {
	lookNeg = glm::normalize(lookNeg);
	up = glm::normalize(glm::cross(lookNeg, right));
	right = glm::cross(up, lookNeg);
}


glm::mat4 Camera::getViewProj() const {
	/*
	float tR = -glm::dot(position, right);
	float tU = -glm::dot(position, up);
	float tL = -glm::dot(position, lookNeg);

	glm::mat4 view(	right.x,	right.y,	right.z,	tR,
					up.x,		up.y,		up.z,		tU,
					lookNeg.x,	lookNeg.y,	lookNeg.z,	tL,
					0.0f,		0.0f,		0.0f,		1.0f );

	return proj * view;
	*/

	//test
	glm::mat4 t = glm::lookAt(glm::vec3(5.0f,5.0f,5.0f),
					glm::vec3(6.0f, 5.0f, 5.0f),
					glm::vec3(0.0f,1.0f,0.0f));

	return glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f) * 
		glm::lookAt(glm::vec3(5.0f,5.0f,5.0f),
					glm::vec3(0.0f, 0.0f, 0.0f),
					glm::vec3(0.0f,0.0f,1.0f));
}