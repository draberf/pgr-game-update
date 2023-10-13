#pragma once

#include <string>
#include "gameobjects.h"

#include <GLFW/glfw3.h>

// source: https://refactoring.guru/design-patterns/mediator/cpp/example
class BaseComponent;

class Mediator {
public:
	virtual void Notify(BaseComponent* sender, std::string event) const = 0;
};

class BaseComponent {
protected:
	Mediator* mediator_;
public:
	BaseComponent(Mediator* mediator = nullptr) : mediator_(mediator) {};
	void set_mediator(Mediator* mediator);
};

class ControlComponent : public BaseComponent {
public:
	void KeyDown(int key);
	void KeyPress(int key);
};

class CharacterComponent : public BaseComponent {
protected:
	
	double dt_;

	Character* character_;
public:
	CharacterComponent(Character* ch);
	Character* GetCharacter();
};

class ShaderComponent : public BaseComponent {

};

class ControlMediator : public Mediator {
private:
	ControlComponent*	control_;
	CharacterComponent* character_;
	ShaderComponent*	shader_;

public:
	ControlMediator(ControlComponent* c1, CharacterComponent* c2, ShaderComponent* c3) {

		this->control_		= c1;
		this->character_	= c2;
		this->shader_		= c3;

		this->control_	->set_mediator(this);
		this->character_->set_mediator(this);
		this->shader_	->set_mediator(this);
	}

	void Notify(BaseComponent* sender, std::string event) const override;
};