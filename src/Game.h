#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameLevel.h"


enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100, 20);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);

class Game {
public:
	GameState state;
	GLboolean keys[1024];
	GLuint width, height;

	std::vector<GameLevel> levels;
	GLuint level;

	Game(GLuint width, GLuint height);
	~Game();

	void Init();

	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
};