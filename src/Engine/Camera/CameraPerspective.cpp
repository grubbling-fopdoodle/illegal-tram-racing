#include "CameraPerspective.h"
#include "../../Game/Game.h"
#include "../../Utils.h"

CameraPerspective::CameraPerspective() :
	mode(Mode::PITCH),
	target(0, 0, 0),
	targetTarget(target),
	position(0, 0, 0),
	yaw(0),
	targetYaw(yaw),
	pitch(30),
	targetPitch(pitch),
	minPitch(1),
	maxPitch(89),
	distance(9),
	minDistance(0.1f),
	maxDistance(100),
	fov(80),
	nearClipPlane(0.1f),
	farClipPlane(1000),
	targetSmoothness(0.002),
	yawSmoothness(0.125),
	pitchSmoothness(0.0005)
{
}

void CameraPerspective::Update()
{
	target = Utils::SmoothStep(target, targetTarget, targetSmoothness, Game::DeltaTime());
	yaw = Utils::SmoothStep(yaw, targetYaw, yawSmoothness, Game::DeltaTime());
	pitch = Utils::SmoothStep(pitch, targetPitch, pitchSmoothness, Game::DeltaTime());
}

glm::mat4 CameraPerspective::ViewMatrix() const
{
	glm::vec3 newPosition;
	switch (mode) {
		case Mode::POSITION:
			newPosition = position;
			break;
		case Mode::PITCH:
		case Mode::PITCH_YAW:
		{
			glm::mat4 trans;
			trans = glm::translate(trans, glm::vec3(0, 0, distance));

			glm::mat4 yawMat;
			yawMat = glm::rotate(yawMat, glm::radians(yaw), glm::vec3(0, 1, 0));
			glm::vec3 rightVector = yawMat * glm::vec4(1, 0, 0, 1);

			glm::mat4 pitchMat;
			pitchMat = glm::rotate(pitchMat, glm::radians(-pitch), rightVector);

			newPosition = pitchMat * yawMat * trans * glm::vec4(0, 0, 0, 1);
			newPosition += target;
			//Utils::DisplayVec3(newPosition);		
		}
	}
	newPosition += screenshakeOffset;
	return glm::lookAt(newPosition, target, glm::vec3(0, 1, 0));
}

glm::mat4 CameraPerspective::ProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), Game::AspectRatio(), nearClipPlane, farClipPlane);
}

glm::vec3 CameraPerspective::UpVector() const
{
	return ViewMatrix() * glm::vec4(0, 1, 0, 0);
}

glm::vec3 CameraPerspective::RightVector() const
{
	return ViewMatrix() * glm::vec4(1, 0, 0, 0);
}

glm::vec3 CameraPerspective::DirectionVector() const
{
	return glm::normalize(target - position);
}

void CameraPerspective::SetMode(Mode mode)
{
	this->mode = mode;
}

void CameraPerspective::SetPosition(glm::vec3&& position)
{
	this->position = position;
}

void CameraPerspective::SetYaw(float yawDegrees)
{
	targetYaw = Utils::NormalizeAngle(targetYaw, yawDegrees);
	//std::cout << "targetYaw: " << targetYaw << std::endl;
}

void CameraPerspective::SetPitch(float pitchDegrees)
{
	targetPitch = glm::clamp(pitchDegrees, minPitch, maxPitch);
}

void CameraPerspective::SetMinPitch(float minPitch)
{
	this->minPitch = minPitch;
}

void CameraPerspective::SetMaxPitch(float maxPitch)
{
	this->maxPitch = maxPitch;
}

void CameraPerspective::SetTarget(glm::vec3&& target)
{
	this->targetTarget = target;
}

void CameraPerspective::SetDistance(float distance)
{
	this->distance = glm::clamp(distance, minDistance, maxDistance);
}

void CameraPerspective::SetMinDistance(float minDistance)
{
	this->minDistance = minDistance;
}

void CameraPerspective::SetMaxDistance(float maxDistance)
{
	this->maxDistance = maxDistance;
}

void CameraPerspective::Zoom(float deltaDistance)
{
	switch (mode) {
	case Mode::POSITION:
	{
		glm::vec3 distanceV = position - target;
		float distance = glm::length(distanceV);
		float ratio = (distance + deltaDistance) / distance;
		distanceV *= ratio;
		position = target + distanceV;
	}
	break;
	case Mode::PITCH:
	case Mode::PITCH_YAW:
		SetDistance(this->distance + deltaDistance);
		break;
	}
}

void CameraPerspective::RotateAround(float deltaYaw, float deltaPitch)
{
	switch (mode) {
	case Mode::POSITION:
	{
		return;
	}
	break;
	case Mode::PITCH_YAW:
		SetYaw(yaw + deltaYaw);
		SetPitch(pitch + deltaPitch);
		break;
	case Mode::PITCH:
		SetPitch(pitch + deltaPitch);
	}
}
