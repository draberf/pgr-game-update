#include "scenegraph.h"

#include <GL/glew.h>
#include <GLM/gtc/type_ptr.hpp>


#include <STB/stb_image.h>


SceneComponent::~SceneComponent()
{
}

void SceneComponent::SetParent(SceneComponent* parent)
{
	this->parent_ = parent;
}

SceneComponent* SceneComponent::GetParent()
{
	return this->parent_;
}

void SceneComponent::Add(SceneComponent* SceneComponent)
{
}

void SceneComponent::Remove(SceneComponent* SceneComponent)
{
}

bool SceneComponent::IsSceneComposite() const
{
	return false;
}




void SceneLeaf::Draw(DRAW_TYPE type)
{
	this->obj_->Draw(type);
}

void SceneLeaf::Update(float dt)
{
	this->obj_->Update(dt);
}




void SceneComposite::Add(SceneComponent* sceneComponent)
{
	this->children_.push_back(sceneComponent);
	sceneComponent->SetParent(this);
}

void SceneComposite::Remove(SceneComponent* sceneComponent)
{
	children_.remove(sceneComponent);
	sceneComponent->SetParent(nullptr);
}

bool SceneComposite::IsSceneComposite() const
{
	return true;
}

void SceneComposite::Draw(DRAW_TYPE type)
{
	for (SceneComponent* c : children_)
		c->Draw(type);
}

void SceneComposite::Update(float dt)
{
	for (SceneComponent* c : children_)
		c->Update(dt);
}




