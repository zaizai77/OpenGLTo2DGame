#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"

SpriteRenderer* renderer;

Game::Game(GLuint width, GLuint height)
	:state(GAME_ACTIVE),keys(),width(width),height(height) {

}

Game::~Game(){
	delete renderer;
}

void Game::Init(){
	ResourceManager::LoadShader("src/Shaders/sprite.shader","sprite");
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(this->width),
		static_cast<float>(this->height), 0.0f, -1.0f, 1.0f);
	ResourceManager::GetShader("sprite").Use().SetInteger("image", 0);
	ResourceManager::GetShader("sprite").SetMatrix4("projection", projection);

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ResourceManager::LoadTexture("res/textures/awesomeface.png", true, "face");
}

void Game::ProcessInput(GLfloat dt){

}

void Game::Update(GLfloat dt){

}

void Game::Render(){
	renderer->DrawSprite(ResourceManager::GetTexture("face"),
		glm::vec2(200.0f, 200.0f), glm::vec2(300.0f, 400.0f), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}