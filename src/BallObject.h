#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "GameObject.h"
#include "texture.h"
#include "SpriteRenderer.h"

class BallObject : public GameObject {
public:
	// Ball State
	GLfloat Radius;
	GLboolean Stuck;

	BallObject();
	BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2D sprite);

	// Moves the ball, keeping it constrained within the window bounds (except bottom edge); returns new position
	glm::vec2 Move(GLfloat dt, GLuint window_width);
	// Resets the ball to original state with given position and velocity
	void      Reset(glm::vec2 position, glm::vec2 velocity);
};