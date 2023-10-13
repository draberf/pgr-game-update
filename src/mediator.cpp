#include "mediator.h"

void BaseComponent::set_mediator(Mediator* mediator)
{
	this->mediator_ = mediator;
}


void ControlComponent::KeyDown(int key)
{
}

void ControlComponent::KeyPress(int key)
{
	if (key == GLFW_KEY_RIGHT) {
		mediator_->Notify(this, "right");
	}

	if (key == GLFW_KEY_LEFT) {
		mediator_->Notify(this, "left");
	}

	if (key == GLFW_KEY_SPACE) {
		mediator_->Notify(this, "jump");
	}
}

void ControlMediator::Notify(BaseComponent* sender, std::string event) const
{
	if (sender == control_) {
		if (event == "right") {
			this->character_->GetCharacter()->MoveX(0.05f);
		}

		if (event == "left") {
			this->character_->GetCharacter()->MoveX(-0.05f);
		}

		if (event == "jump") {
			this->character_->GetCharacter()->Jump();
		}
	}
	
}

CharacterComponent::CharacterComponent(Character* ch)
{
	this->character_ = ch;
}

Character* CharacterComponent::GetCharacter()
{
	return character_;
}
