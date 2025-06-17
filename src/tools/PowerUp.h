#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "../GameObject.h"

// The size of a PowerUp block
const glm::vec2 POWERUP_SIZE(60.0f, 20.0f);
// Velocity a PowerUp block has when spawned
const glm::vec2 VELOCITY(0.0f, 150.0f);

/* 道具的具体效果
Speed: 增加小球20%的速度
Sticky: 当小球与玩家挡板接触时，小球会保持粘在挡板上的状态直到再次按下空格键，这可以让玩家在释放小球前找到更合适的位置
Pass-Through: 非实心砖块的碰撞处理被禁用，使小球可以穿过并摧毁多个砖块
Pad-Size-Increase: 增加玩家挡板50像素的宽度
Confuse: 短时间内激活confuse后期特效，迷惑玩家
Chaos: 短时间内激活chaos后期特效，使玩家迷失方向
*/

// PowerUp inherits its state and rendering functions from
// GameObject but also holds extra information to state its
// active duration and whether it is activated or not. 
// The type of PowerUp is stored as a string.
class PowerUp : public GameObject
{
public:
    // powerup state
    std::string Type;
    float       Duration;
    bool        Activated;
    // constructor
    PowerUp(std::string type, glm::vec3 color, float duration, glm::vec2 position, Texture2D texture)
        : GameObject(position, POWERUP_SIZE, texture, color, VELOCITY), Type(type), Duration(duration), Activated() { }
};