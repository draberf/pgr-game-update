#include "collision.h"

SquareHitbox::SquareHitbox(float width, float height, float x, float y)
{
	this->w_ = width; this->h_ = height;
	this->x_ = x;	  this->y_ = y;
}

void SquareHitbox::SetSize(float width, float height)
{
	this->w_ = width;
	this->h_ = height;
}

void SquareHitbox::MoveBox(float x, float y)
{
	this->x_ = x;
	this->y_ = y;
}

// source: https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection 
bool SquareHitbox::IsColliding(SquareHitbox* s)
{
	// collision along X
	bool collX = x_ + w_ > s->x_ && s->x_ + s->w_ > x_;
	// collision along Y
	bool collY = y_ + h_ > s->y_ && s->y_ + s->h_ > y_;

	return collX && collY;
}
