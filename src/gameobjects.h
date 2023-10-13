#pragma once

#include "geometry.h"
#include "glmodel.h"
#include "collision.h"

#include "drawtype.h"

#define JUMP_SPEED 5.f

struct XYcoords {
	float x;
	float y;
};

class GameObject {
public:
	virtual void Draw(DRAW_TYPE type) = 0;
	virtual void Update(float dt) = 0;

	bool IsTerrain() const;
};

class Terrain : public GameObject {
	
	Geometry* mesh_;
	SquareHitbox* hitbox_;
	
	float w_, h_, d_;
	float x_, y_;

	float pitch_, yaw_;

	bool ishazard_ = false;

	unsigned int vbo_;
	unsigned int prg_;
	glm::vec3 color_ = glm::vec3(0.1f, 0.7f, 0.1f);
	bool glow_ = false;

public:

	Terrain(float width, float height, float depth, float x, float y, float yaw, unsigned int prg);

	void Draw(DRAW_TYPE type) override;
	void Update(float dt) override;
	void UpdateColor(float r, float g, float b);
	void SetGlow(bool g);

	bool IsTerrain();
	SquareHitbox* GetHitbox();

	bool IsColliding(SquareHitbox* s);

	void SetHazard(bool hazard);
	bool IsHazard();
};

class Character : public GameObject {
	
	// camera targets

	class CamTarget {
	public:
		float x_ = 0.f;
		float y_ = 0.f;
	} target_;

	class CamFollower {
	public:
		float x_ = 0.f;
		float y_ = 0.f;
		float v_;		// velocity
		void Follow(CamTarget t, float dt);
	} follower_;

	float t_offset_ = 0.f;

	void UpdateCameraTargets();

	// collisions
	SquareHitbox* hb_fro_;
	SquareHitbox* hb_top_;
	SquareHitbox* hb_bot_;

	bool coll_fro_;
	bool coll_top_;
	bool coll_bot_;

	float coyotetime_ = 0.f;

	void UpdateHitboxes();
	
	bool isdead_ = false;

	enum class STATE {
		IDLE,
		JUMP,
		FALL,
	} state_;

	float x_, y_;
	float y_vel_;

	float jumpcharge_;
	bool jumpquery_ = false;
	
	bool facingRight_;

	void UpdateYaw();

	// rendering values

	glm::vec3 color_;

	double dt_;

	Model* model_;
	float yaw_;
	float scale_;

	unsigned int prg_;

public:
	
	Character(std::string path, float x, float y, float scale, unsigned int program);

	XYcoords GetCameraTargetXY();
	XYcoords GetCharacTargetXY();

	bool GetDeathStatus();

	void Draw(DRAW_TYPE type) override;
	void MoveX(float dist);
	void Jump();
	void Update(float dt) override;
	void UpdateColor(float r, float g, float b);

	void CalculateCollisions(std::vector<Terrain*> t);

	bool CheckFinish(Terrain* finishBlock);

	bool IsHazard();


};

class Skybox : public GameObject {

	float angle_;
	unsigned int vbo_;
	unsigned int prg_;
	unsigned int texture_;

	struct U {
		unsigned int zoomX;
		unsigned int zoomY;
		unsigned int offset;
	};

	struct U uniforms_;

public:

	Skybox(std::string path, float angle, unsigned int program);

	void SetAngle(float angle);
	float GetAngle(float angle);

	void Draw(DRAW_TYPE type) override;
	void Update(float dt) override;
};

class Coin : public GameObject {};