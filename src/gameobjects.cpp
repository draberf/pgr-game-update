#include <GL/glew.h>
#include <STB/stb_image.h>
#include <GLM/glm.hpp>
#include <GLM/gtc/type_ptr.hpp>

#include "gameobjects.h"

bool GameObject::IsTerrain() const
{
	return false;
}

Character::Character(std::string path, float x, float y, float scale, unsigned int program)
{
	this->x_ = x;
	this->y_ = y;

	this->dt_ = 1.f;

	this->scale_ = scale;

	this->facingRight_ = true;
	this->yaw_ = 0;
	this->UpdateYaw();

	this->model_ = new Model(".\\models\\hunter\\Hunter-Character-Model.fbx");

	this->hb_fro_ = new SquareHitbox(0.1f, 1.3f, 0.f, 0.f);
	this->hb_top_ = new SquareHitbox(0.3f, 0.1f, 0.f, 0.f);
	this->hb_bot_ = new SquareHitbox(0.3f, 0.1f, 0.f, 0.f);
	UpdateHitboxes();

	this->color_ = glm::vec3(0.2f, 0.3f, 0.8f);
	this->prg_ = program;
}

XYcoords Character::GetCameraTargetXY()
{
	return XYcoords({ follower_.x_, follower_.y_ });
}

XYcoords Character::GetCharacTargetXY()
{
	return XYcoords({this->x_, this->y_});
}

void Character::Draw(DRAW_TYPE type)
{
	if (type != DRAW_TYPE::GEOMETRY) { return; }


	GLuint modelUniform = glGetUniformLocation(this->prg_, "u_model");
	GLuint colorUniform = glGetUniformLocation(this->prg_, "a_color");

	auto modelM =
		glm::translate(glm::mat4(1.f), glm::vec3(this->x_+0.03f, this->y_, 0.f)) *
		glm::rotate(glm::mat4(1.f), this->yaw_, glm::vec3(0.f, 1.f, 0.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(this->scale_, this->scale_, this->scale_));

		
		
		
	glProgramUniformMatrix4fv(this->prg_, modelUniform, 1, GL_FALSE, glm::value_ptr(modelM));
	glProgramUniform3f(this->prg_, colorUniform, color_.x, color_.y, color_.z);


	this->model_->Draw();

	return;
	// remove above return to render hitboxes

	Terrain* botHB = new Terrain(
		hb_bot_->w_, hb_bot_->h_, 0.5f,
		hb_bot_->x_, hb_bot_->y_,
		0.f,
		this->prg_
	);
	botHB->UpdateColor(1.f, 0.f, 0.f);

	Terrain* froHB = new Terrain(
		hb_fro_->w_, hb_fro_->h_, 0.6f,
		hb_fro_->x_, hb_fro_->y_,
		0.f,
		this->prg_
	);
	froHB->UpdateColor(0.f, 1.f, 0.f);

	Terrain* topHB = new Terrain(
		hb_top_->w_, hb_top_->h_, 0.7f,
		hb_top_->x_, hb_top_->y_,
		0.f,
		this->prg_
	);
	topHB->UpdateColor(0.f, 0.f, 1.f);

	botHB->Draw(DRAW_TYPE::GEOMETRY);
	froHB->Draw(DRAW_TYPE::GEOMETRY);
	topHB->Draw(DRAW_TYPE::GEOMETRY);


}

void Character::MoveX(float dist)
{
	facingRight_ = (dist > 0.f);

	if (dist == 0.f || coll_fro_) return;

	t_offset_ = dist * 15.f;
	this->x_ += dist;
	
}

void Character::Update(float dt)
{
	this->dt_ = dt;

	// hitboxes
	
	UpdateHitboxes();

	// visual

	UpdateYaw();
	UpdateCameraTargets();

	STATE newState = state_;

	// controls
	switch (state_) {
	case (STATE::IDLE):
		coyotetime_ = 0.2f;
		if (jumpquery_ && coll_bot_) {
			newState = STATE::JUMP;
			jumpcharge_ = 0.4f;
			break;
		}

		if (!coll_bot_) {
			newState = STATE::FALL;
			break;
		}

		break;

	case (STATE::FALL):

		if (coyotetime_ > 0.01f) {
			if (jumpquery_) {
				coyotetime_ = 0.f;
				newState = STATE::JUMP;
				jumpcharge_ = 0.4f;
				break;
			}
			coyotetime_ -= dt;
		}

		if (coll_bot_) {
			newState = STATE::IDLE;
			y_vel_ = 0;
			break;
		}

		y_vel_ += -9.f*dt;
		if (y_vel_ < -8.f) y_vel_ = -8.f;
		y_ += y_vel_*dt;

		jumpquery_ = false;

		break;

	case (STATE::JUMP):

		if (coll_top_) {
			// hit head
			y_vel_ = 0.f;
			newState = STATE::IDLE;
			break;
		}

		// constant upwards
		y_vel_ = JUMP_SPEED;

		if (!jumpquery_ || jumpcharge_ < 0.05f) {
			newState = STATE::FALL;
			y_vel_ = JUMP_SPEED;
		}
		else {
			jumpcharge_ -= 1.f * dt_;
		}

		y_ += y_vel_ * dt;

		if (jumpcharge_ < 0.35f) jumpquery_ = false;
	}
	state_ = newState;
}

void Character::UpdateColor(float r, float g, float b)
{
	this->color_ = glm::vec3(r, g, b);
}

void Character::CalculateCollisions(std::vector<Terrain*> t)
{
	coll_fro_ = false;
	coll_top_ = false;
	coll_bot_ = false;

	for (unsigned int i = 0; i < t.size(); i++) {
		if (!coll_fro_) {
			coll_fro_ = hb_fro_->IsColliding(t[i]->GetHitbox());
			if (coll_fro_ && t[i]->IsHazard()) isdead_ = true;
		}
		if (!coll_top_) {
			coll_top_ = hb_top_->IsColliding(t[i]->GetHitbox());
			if (coll_top_ && t[i]->IsHazard()) isdead_ = true;
		}
		if (!coll_bot_) {
			coll_bot_ = hb_bot_->IsColliding(t[i]->GetHitbox());
			if (coll_bot_ && t[i]->IsHazard()) isdead_ = true;
		}

		if (coll_fro_ && coll_top_ && coll_bot_) break;
	}
}

void Character::CamFollower::Follow(CamTarget t, float dt)
{
	auto dist = sqrt(
		(this->x_ - t.x_) * (this->x_ - t.x_) +
		(this->y_ - t.y_) * (this->y_ - t.y_)
	);

	if (dist < 0.003f) {
		this->x_ = t.x_;
		this->y_ = t.y_;
		this->v_ = 0.f;
	}
	else {
		this->v_  = 1.7f * (this->v_ + dist) / 2.f + 0.2f;
		this->x_ += (t.x_ - this->x_) * this->v_ * dt;
		this->y_ += (t.y_ - this->y_) * this->v_ * dt * 2.f;
	}

}

void Character::UpdateCameraTargets()
{
	// forward in facing direction
	target_.x_ = x_ + t_offset_+(facingRight_ ? +0.7f : -0.7f);
	t_offset_ = 0.f;

	// same height
	target_.y_ = y_;

	follower_.Follow(target_, dt_);
}

void Character::UpdateHitboxes()
{
	float side = (facingRight_ ? 1.f : -1.f);

	hb_fro_->MoveBox(x_ + side * 0.2f, y_ - 0.4f);
	hb_bot_->MoveBox(x_ +       -0.1f, y_ - 0.7f);
	hb_top_->MoveBox(x_ +       -0.1f, y_ + 1.f);
}

void Character::UpdateYaw()
{
	if (!facingRight_) {
		yaw_ += -0.3f;
		if (yaw_ < -glm::half_pi<float>()) {
			yaw_ = -glm::half_pi<float>();
		}
	}
	else {
		yaw_ +=  0.3f;
		if (yaw_ > glm::half_pi<float>()) {
			yaw_ = glm::half_pi<float>();
		}
	}
}

void Character::Jump()
{
	jumpquery_ = true;
}

bool Character::GetDeathStatus()
{
	return isdead_;
}

bool Character::IsHazard()
{
	return false;
}

bool Character::CheckFinish(Terrain* finishBlock)
{
	if (hb_bot_->IsColliding(finishBlock->GetHitbox())) return true;
	if (hb_fro_->IsColliding(finishBlock->GetHitbox())) return true;
	if (hb_top_->IsColliding(finishBlock->GetHitbox())) return true;

	return false;
}



Skybox::Skybox(std::string path, float angle, unsigned int program)
{
	this->angle_ = angle;

	this->prg_ = program;

	glGenBuffers(1, &(this->vbo_));

	this->uniforms_.zoomX = glGetUniformLocation(this->prg_, "offset");
	this->uniforms_.zoomY = glGetUniformLocation(this->prg_, "xZoom");
	this->uniforms_.offset = glGetUniformLocation(this->prg_, "yZoom");


	stbi_set_flip_vertically_on_load(true);
	int width, height, nrChannels;
	unsigned char* data = stbi_load(path.data(), &width, &height, &nrChannels, 0);

	glGenTextures(1, &(this->texture_));
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture_);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

void Skybox::SetAngle(float angle)
{
	this->angle_ = angle;
}

float Skybox::GetAngle(float angle)
{
	return this->angle_;
}

void Skybox::Draw(DRAW_TYPE type)
{
	if (type != DRAW_TYPE::SKYBOX) { return; }

	// reset to 0-2pi range
	while (this->angle_ > 2.f * glm::pi<float>()) {
		this->angle_ -= 2.f * glm::pi<float>();
	}
	while (this->angle_ < -2.f * glm::pi<float>()) {
		this->angle_ += 2.f * glm::pi<float>();
	}

	glBindVertexArray(this->vbo_);
	glUseProgram(this->prg_);

	glProgramUniform1f(this->prg_, this->uniforms_.zoomX, -0.5f * this->angle_ / glm::pi<float>());
	glProgramUniform1f(this->prg_, this->uniforms_.zoomY, 1.f);
	glProgramUniform1f(this->prg_, this->uniforms_.offset, 0.7f);

	glBindTexture(GL_TEXTURE_2D, this->texture_);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glClear(GL_DEPTH_BUFFER_BIT);
}

void Skybox::Update(float dt)
{

}



Terrain::Terrain(float width, float height, float depth, float x, float y, float yaw, unsigned int prg)
{


	this->mesh_ = new Block(width, height, depth);
	this->w_ = width;
	this->h_ = height;
	this->d_ = depth;
	this->x_ = x;
	this->y_ = y;

	this->prg_ = prg;

	this->yaw_ = yaw;
	this->pitch_ = 0;

	this->hitbox_ = new SquareHitbox(w_, h_, x_, y_);

	glGenBuffers(1, &(this->vbo_));


}

void Terrain::Draw(DRAW_TYPE type)
{
	if (type != DRAW_TYPE::GEOMETRY) { return; }

	GLuint modelUniform = glGetUniformLocation(this->prg_, "u_model");
	GLuint colorUniform = glGetUniformLocation(this->prg_, "a_color");

	auto modelM =
		glm::translate(glm::mat4(1.f), glm::vec3(this->x_, this->y_, 0.5f*this->d_)) *
		glm::rotate(glm::mat4(1.f), this->yaw_, glm::vec3(0.f, 1.f, 0.f));

	glBindBuffer(GL_ARRAY_BUFFER, this->vbo_);
	glBufferData(GL_ARRAY_BUFFER, mesh_->GeometrySize(), mesh_->GetGeometry(), GL_STATIC_DRAW);

	glProgramUniformMatrix4fv(this->prg_, modelUniform, 1, GL_FALSE, glm::value_ptr(modelM));
	glProgramUniform3f(this->prg_, colorUniform, color_.r, color_.g, color_.b);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, 6 * sizeof(float), (void*)12);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, 36);
	
}

void Terrain::Update(float dt)
{
}

void Terrain::UpdateColor(float r, float g, float b)
{
	this->color_ = glm::vec3(r, g, b);
}

void Terrain::SetGlow(bool g)
{
	glow_ = g;
}

bool Terrain::IsTerrain()
{
	return true;
}

void Terrain::SetHazard(bool hazard)
{
	ishazard_ = hazard;
}

bool Terrain::IsHazard()
{
	return ishazard_;
}

bool Terrain::IsColliding(SquareHitbox* s)
{
	return hitbox_->IsColliding(s);
}

SquareHitbox* Terrain::GetHitbox()
{
	return hitbox_;
}