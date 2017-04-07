#pragma once

#include <glm/glm.hpp>



class Camera {
public:
	Camera();

	void setPosition(const glm::vec3& position);
	void offsetPosition(const glm::vec3& offset);
	float fieldOfView() const;
	void setFieldOfView(float fieldOfView);
	float nearPlane() const;
	float farPlane() const;
	void setNearAndFarPlanes(float nearPlane, float farPlane);

	
	void offsetOrientation(float upAngle, float rightAngle);
	void lookAt(glm::vec3 position);
	float viewportAspectRatio() const;
	void setViewportAspectRatio(float viewportAspectRatio);

	const glm::vec3& position() const;
	glm::mat4 orientation() const;
	glm::vec3 forward() const;
	glm::vec3 right() const;
	glm::vec3 up() const;
	glm::mat4 matrix() const;
	glm::mat4 projection() const;
	glm::mat4 view() const;

	void MoveForward(float d);
	void MoveRight(float d);
	void MoveUpWorld(float d);
	void MoveUpRelative(float d);


private:
	glm::vec3 _position;
	float _horizontalAngle;
	float _verticalAngle;
	float _fieldOfView;
	float _nearPlane;
	float _farPlane;
	float _viewportAspectRatio;

	void normalizeAngles();
};


