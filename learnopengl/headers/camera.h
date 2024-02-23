#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define CAMERA_MODE_FPS 0
#define CAMERA_MODE_DRONE 1
#define CAMERA_SPEED 2.5f
#define CAMERA_BOOST_SPEED CAMERA_SPEED * 2

const float D_PITCH = 0.0f;
const float D_YAW = 0.0f;
const float D_SPEED = CAMERA_SPEED;
const float D_SENSITIVITY = 0.1f;
const float D_FOV = 45.0f;
const glm::vec3 D_FRONT = glm::vec3(0.0f, 0.0f, -1.0f);

class Camera {
public: 
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 worldUp;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 target;
    float speed;
    float pitch;
    float yaw;
    float fov;
    int mode;

    struct {
        float min = 1.0f;
        float max = 90.0f;
    } zoom;

    enum class Action {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        WORLD_UP,
        WORLD_DOWN
    };

	Camera(glm::vec3 position, glm::vec3 worldUp = glm::vec3(0.0f,1.0f,0.0f), float fov = D_FOV, float speed = D_SPEED, int mode = CAMERA_MODE_FPS, float yaw = D_YAW, float pitch = D_PITCH) {
        this->position = position;
        this->worldUp = worldUp;
        this->yaw = yaw;
        this->pitch = pitch;
        this->fov = fov;
        this->speed = speed;
        this->mode = mode;
        updateVectors();
	}

    glm::mat4 getViewMatrix() const{
        return glm::lookAt(position, position + front, worldUp);
    }

    void processMovement(Action action, float deltaTime) {
        float deltaSpeed = speed * deltaTime;
        float prevy = position.y;

        switch (action)
        {
        case Action::FORWARD:
            position += front * deltaSpeed;
            break;
        case Action::BACKWARD:
            position -= front * deltaSpeed;
            break;
        case Action::LEFT:
            position -= right * deltaSpeed;
            break;
        case Action::RIGHT:
            position += right * deltaSpeed;
            break;
        case Action::WORLD_UP:
            position += worldUp * deltaSpeed;
            break;
        case Action::WORLD_DOWN:
            position -= worldUp * deltaSpeed;
            break;
        }

        if (mode == CAMERA_MODE_FPS) {
            position.y = prevy;
        }
    }

    void processLook(float yaw, float pitch) {
        this->yaw += yaw;
        this->pitch += pitch;
        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;
        updateVectors();
    }

    void processZoom(float change) {
        fov += change;
        if (fov < zoom.min)
            fov = zoom.min;
        if (fov > zoom.max)
            fov = zoom.max;
    }
        
private:
    void updateVectors() {
        glm::vec3 newFront;
        newFront.x = glm::cos(glm::radians(yaw)) * glm::cos(glm::radians(pitch)); // ...* glm::cos(camera.yaw) does not make sense to me. As far as I can tell the same rule would apply to the calculation of y
        newFront.y = glm::sin(glm::radians(pitch));
        newFront.z = glm::sin(glm::radians(yaw)) * glm::cos(glm::radians(pitch));
        front = glm::normalize(newFront);
        right = glm::normalize(glm::cross(front, worldUp));
        up = glm::normalize(glm::cross(right, front));
    }
};

#endif