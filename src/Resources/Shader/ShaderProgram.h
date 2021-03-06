#pragma once

#include "../../glbinding.h"

#include "ShaderSource.h"
#include "ShaderSourceId.h"
#include <glm/detail/type_vec3.hpp>

class ShaderProgram
{
	ShaderSourceId vertexShader;
	ShaderSourceId fragmentShader;

	unsigned int glId;

public:
	ShaderProgram(unsigned int vertexShader, unsigned int fragmentShader);

	void Use();
	void SetVec3(std::string name, glm::vec3 value) const;
	void SetFloat(std::string name, float value) const;

	const unsigned int& GlId() const;

};
