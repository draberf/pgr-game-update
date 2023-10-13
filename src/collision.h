# pragma once

class Hitbox {

};

class SquareHitbox : public Hitbox {
public:
	float w_, h_, x_, y_;
	SquareHitbox(float width, float height, float x, float y);
	void SetSize(float width, float height);
	void MoveBox(float x, float y);

	bool IsColliding(SquareHitbox* s);
};

