#pragma once

#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
private:
    glm::vec3 mWorldUp;
    glm::vec3 mPosition;
    glm::vec3 mFront;
    glm::vec3 mUp;
    glm::vec3 mRight;

    glm::vec3 mVelocity;

    float mMoveSpeed;
    float mLookSpeed;
    float mPitch;
    float mYaw;
    float mPlayerHeight;
    void updateVectors();

public:
    Camera();

    void Move(float dx, float dy, float dt);

    void Move(float height, float dt);

    void Rotate(float dx, float dy, float dt);

    void SetHeight(float height);

    float GetHeight();

    void SetPitch(float pitch);

    float GetPitch();

    glm::vec3 GetPosition();

    glm::vec3 GetTarget();

    glm::vec3 GetUp();
};