#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "GameLevel.h"
#include "PowerUp.h"


enum GameState {
	GAME_ACTIVE,
	GAME_MENU,
	GAME_WIN
};

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT
};

//Defines a Collision typedef that represents collision data
// <collision?, what direction?, difference vector center - closest point>
typedef std::tuple<bool, Direction, glm::vec2> Collision;

// Initial size of the player paddle
const glm::vec2 PLAYER_SIZE(100.0f, 20.0f);
// Initial velocity of the player paddle
const GLfloat PLAYER_VELOCITY(500.0f);
// Initial velocity of the Ball
const glm::vec2 INITIAL_BALL_VELOCITY(100.0f, -350.0f);
// Radius of the ball object
const float BALL_RADIUS = 12.5f;

class Game {
public:
	GameState state;
	GLboolean keys[1024];
	GLuint width, height;

	std::vector<GameLevel> levels;
	GLuint level;

	std::vector<PowerUp> PowerUps;

	Game(GLuint width, GLuint height);
	~Game();

	void Init();

	void ProcessInput(GLfloat dt);
	void Update(GLfloat dt);
	void Render();
	void DoCollisions();

	//reset
	void ResetLevel();
	void ResetPlayer();

	// powerups 
	void SpawnPowerUps(GameObject& block); //在给定的砖块位置生成一个道具
	void UpdatePowerUps(GLfloat dt);  //管理所有激活的道具
};