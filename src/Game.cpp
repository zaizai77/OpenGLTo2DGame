#include "Game.h"
#include "SpriteRenderer.h"
#include "ResourceManager.h"
#include "BallObject.h"
#include "ParticalGenerator.h"

SpriteRenderer* renderer;
GameObject* Player;
BallObject* ball;
ParticleGenerator* partical;

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
	ResourceManager::LoadShader("src/Shaders/partical.shader", "partical");
	ResourceManager::GetShader("partical").Use().SetMatrix4("projection", projection);

	renderer = new SpriteRenderer(ResourceManager::GetShader("sprite"));

	ResourceManager::LoadTexture("res/textures/background.jpg", GL_FALSE, "background");
	ResourceManager::LoadTexture("res/textures/awesomeface.png", GL_TRUE, "face");
	ResourceManager::LoadTexture("res/textures/block.png", GL_FALSE, "block");
	ResourceManager::LoadTexture("res/textures/block_solid.png", GL_FALSE, "block_solid");
	ResourceManager::LoadTexture("res/textures/paddle.png", true, "paddle");
	ResourceManager::LoadTexture("res/textures/particle.png", GL_TRUE, "partical");

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

	partical = new ParticleGenerator(
		ResourceManager::GetShader("partical"),
		ResourceManager::GetTexture("partical"),
		500
	);
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
	// check for collisions
	this->DoCollisions();
	// check loss condition
	if (ball->Position.y >= this->height) {
		this->ResetLevel();
		this->ResetPlayer();
	}
	//update particals
	partical->Update(dt, *ball, 2, glm::vec2(ball->Radius / 2));
}

void Game::Render(){
	if (this->state == GAME_ACTIVE) {
		renderer->DrawSprite(ResourceManager::GetTexture("background"),
			glm::vec2(0, 0), glm::vec2(this->width, this->height), 0.0f);

		this->levels[this->level].Draw(*renderer);

		//Draw Player
		Player->Draw(*renderer);

		partical->Draw();

		//Draw Ball
		ball->Draw(*renderer);
	}
	
}

void Game::ResetLevel() {
	if (this->level == 0) {
		this->levels[0].Load("res/levels/one.lvl", this->width, this->height / 2);
	} else if (this->level == 1) {
		this->levels[1].Load("res/levels/two.lvl", this->width, this->height / 2);
	}
	else if (this->level == 2) {
		this->levels[2].Load("res/levels/three.lvl", this->width, this->height / 2);
	}
	else if (this->level == 3) {
		this->levels[3].Load("res/levels/four.lvl", this->width, this->height / 2);
	}
}

void Game::ResetPlayer() {
	Player->Size = PLAYER_SIZE;
	Player->Position = glm::vec2(this->width / 2.0f - PLAYER_SIZE.x / 2.0f, this->height - PLAYER_SIZE.y);
	ball->Reset(Player->Position + glm::vec2(PLAYER_SIZE.x / 2.0f - BALL_RADIUS, 
		-(BALL_RADIUS * 2.0f)), INITIAL_BALL_VELOCITY);
}

bool CheckCollision(GameObject& one, GameObject& two);
Collision CheckCollision(BallObject& one, GameObject& two);
Direction VectorDirection(glm::vec2 closest);

void Game::DoCollisions() {
	for (GameObject& box : this->levels[this->level].bricks) {
		if (!box.Destroyed) {
			Collision collision = CheckCollision(*ball, box);
			if (std::get<0>(collision)) //if collision is true 
			{
				//destory block if not solid
				if (!box.IsSolid) {
					box.Destroyed = true;
				}
				//collision resolution
				Direction dir = std::get<1>(collision);
				glm::vec2 diff_vector = std::get<2>(collision);
				if (dir == LEFT || dir == RIGHT) // horizontal collision
				{
					ball->Velocity.x = -ball->Velocity.x;
					float penetration = ball->Radius - std::abs(diff_vector.x);
					if (dir == LEFT) {
						ball->Position.x += penetration;
					}
					else {
						ball->Position.x -= penetration;
					}
				}
				else {
					ball->Velocity.y = -ball->Velocity.y; // reverse vertical velocity
					// relocate
					float penetration = ball->Radius - std::abs(diff_vector.y);
					if (dir == UP)
						ball->Position.y -= penetration; // move ball bback up
					else
						ball->Position.y += penetration; // move ball back down
				}
			}
		}
	}

	//check collision for player pad(unless stuck)
	Collision result = CheckCollision(*ball, *Player);
	if (!ball->Stuck && std::get<0>(result)) {
		// check where it hit the board, and change velocity based on where it hit the board
		float centerBoard = Player->Position.x + Player->Size.x / 2.0f;
		float distance = (ball->Position.x + ball->Radius) - centerBoard;
		float percentage = distance / (Player->Size.x / 2.0f);
		// then move accordingly
		float strength = 2.0f;
		glm::vec2 oldVelocity = ball->Velocity;
		ball->Velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
		//Ball->Velocity.y = -Ball->Velocity.y;
		 // keep speed consistent over both axes (multiply by length of old velocity, 
		//so total strength is not changed)
		ball->Velocity = glm::normalize(ball->Velocity) * glm::length(oldVelocity);
		// fix sticky paddle
		ball->Velocity.y = -1.0f * abs(ball->Velocity.y);
	}
}

bool CheckCollision(GameObject& one, GameObject& two) {
	// collision x-axis?
	bool collisionX = one.Position.x + one.Size.x >= two.Position.x &&
		two.Position.x + two.Size.x >= one.Position.x;
	// collision y-axis?
	bool collisionY = one.Position.y + one.Size.y >= two.Position.y &&
		two.Position.y + two.Size.y >= one.Position.y;
	// collision only if on both axes
	return collisionX && collisionY;
}

Collision CheckCollision(BallObject& one, GameObject& two) {
	//get center point circle first
	glm::vec2 center(one.Position + one.Radius);
	//calculate AABB info (center,half-extents)
	glm::vec2 aabb_half_extens(two.Size.x / 2.0f, two.Size.y / 2.0f);
	glm::vec2 aabb_center(two.Position.x + aabb_half_extens.x, two.Position.y + aabb_half_extens.y);
	//get difference vector between both centers
	glm::vec2 difference = center - aabb_center;
	glm::vec2 clamped = glm::clamp(difference, -aabb_half_extens, aabb_half_extens);
	//now that we know clamped values,add this to AABB_center and we get the values of box closest to circle
	glm::vec2 closest = aabb_center + clamped;
	// now retrieve vector between center circle and closest point AABB and check if length < radius
	difference = closest - center;

	if (glm::length(difference) < one.Radius) // not <= since in that case a collision also occurs when object one exactly touches object two, which they are at the end of each collision resolution stage.
		return std::make_tuple(true, VectorDirection(difference), difference);
	else
		return std::make_tuple(false, UP, glm::vec2(0.0f, 0.0f));
}

// calculates which direction a vector is facing
Direction VectorDirection(glm::vec2 target) {
	glm::vec2 compass[] = {
		glm::vec2(0.0f,1.0f), // up
		glm::vec2(1.0f,0.0f), //right
		glm::vec2(0.0f,-1.0f), // down
		glm::vec2(-1.0f,0.0f) // left
	};
	float max = 0.0f;
	GLuint best_match = -1;
	for (unsigned int i = 0; i < 4; i++) {
		float dot_product = glm::dot(glm::normalize(target), compass[i]);

		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}
	return (Direction)best_match;
}
