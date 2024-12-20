#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "BallObject.h"

SpriteRenderer* renderer;
GameObject* Player;
BallObject* ball;

Game::Game(GLuint width, GLuint height)
	:state(GAME_ACTIVE),keys(),width(width),height(height) {

}

Game::~Game(){
	delete renderer;
	delete Player;
}

void Game::Init(){
	ResourceManager::LoadShader("src/Shaders/sprite.shader","sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
		static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ResourceManager::LoadTexture("res/textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("res/textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", true, "paddle");

	// 加载关卡
	GameLevel one;     one.Load("res/levels/one.lvl", this->width, this->height * 0.5);
	GameLevel two;     two.Load("res/levels/two.lvl", this->width, this->height * 0.5);
	GameLevel three; three.Load("res/levels/three.lvl", this->width, this->height * 0.5);
	GameLevel four;   four.Load("res/levels/four.lvl", this->width, this->height * 0.5);

	this->levels.push_back(one);
	this->levels.push_back(two);
	this->levels.push_back(three);
	this->levels.push_back(four);
	this->level = 0;

	glm::vec2 playerPos = glm::vec2(this->width / 2 - PLAYER_SIZE.x / 2, this->height - PLAYER_SIZE.y);
	Player = new GameObject(playerPos, PLAYER_SIZE, ResourceManager::GetTexture("paddle"));

	glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, -BALL_RADIUS * 2.0f);
	ball = new BallObject(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::GetTexture("face"));
}

void Game::ProcessInput(GLfloat dt){
	if (this->state == GAME_ACTIVE)
	{
		GLfloat velocity = PLAYER_VELOCITY * dt;
		// Move playerboard
		if (this->keys[GLFW_KEY_A])
		{
			if (Player->Position.x >= 0)
				Player->Position.x -= velocity;
			if (ball->Stuck)
				ball->Position.x -= velocity;
		}
		if (this->keys[GLFW_KEY_D])
		{
			if (Player->Position.x <= this->width - Player->Size.x)
				Player->Position.x += velocity;
			if (ball->Stuck)
				ball->Position.x += velocity;
		}
		if (this->keys[GLFW_KEY_SPACE])
			ball->Stuck = false;
	}
}

void Game::Update(GLfloat dt){
	ball->Move(dt, this->width);
}

void Game::Render(){
	if (this->state == GAME_ACTIVE) {
		renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f);
	}
	this->levels[this->level].Draw(*renderer);

	//Draw Player
	Player->Draw(*renderer);
	
	//Draw Ball
	ball->Draw(*renderer);
}