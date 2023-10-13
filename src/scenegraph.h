#pragma once

// SceneComposite

#include <list>
#include <string>

#include <GLM/glm.hpp>

#include "gameobjects.h"
#include "drawtype.h"

// composite source: https://refactoring.guru/design-patterns/composite/cpp/example
class SceneComponent {

private:
    // transformation matrix
    glm::mat4 matrix_;


protected:
    SceneComponent* parent_;

public:
    virtual ~SceneComponent();
    void SetParent(SceneComponent* parent);
    SceneComponent* GetParent();

    virtual void Add(SceneComponent* SceneComponent);
    virtual void Remove(SceneComponent* SceneComponent);

    virtual bool IsSceneComposite() const;

    virtual void Draw(DRAW_TYPE type) = 0;
    virtual void Update(float dt) = 0;

};


class SceneLeaf : public SceneComponent {

    GameObject* obj_;

public:
    SceneLeaf(GameObject* obj) {
        this->obj_ = obj;
    }

    void Draw(DRAW_TYPE type) override;
    void Update(float dt) override;
};

class SceneComposite : public SceneComponent {

protected:
    std::list<SceneComponent*> children_;

public:
    void Add(SceneComponent* SceneComponent) override;
    void Remove(SceneComponent* SceneComponent) override;

    bool IsSceneComposite() const override;

    void Draw(DRAW_TYPE type) override;
    void Update(float dt) override;
};
