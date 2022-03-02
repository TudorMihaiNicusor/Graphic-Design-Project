#include "Camera.hpp"

namespace gps {

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) {
        //TODO
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraUpDirection = cameraUp;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() {
        //TODO
        glm::mat4 viewMatrix = glm::lookAt(this->cameraPosition, this->cameraPosition + this->cameraFrontDirection, this->cameraUpDirection);
        return viewMatrix;
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) {
        //TODO
        switch (direction)
        {
        case gps::MOVE_FORWARD:
            this->cameraPosition += speed * this->cameraFrontDirection;
            break;
        case gps::MOVE_BACKWARD:
            this->cameraPosition -= speed * this->cameraFrontDirection;
            break;
        case gps::MOVE_RIGHT:
            cameraPosition += glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
            break;
        case gps::MOVE_LEFT:
            cameraPosition -= glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraUpDirection)) * speed;
            break;
        default:
            break;
        }

    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) {
        //TODO
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(direction);
    }

    glm::vec3 Camera::getCameraPosition() {
        return this->cameraPosition;
    }

    glm::vec3 Camera::getCameraTarget() {
        return this->cameraTarget;
    }

    void Camera::setCameraPosition(glm::vec3 cameraPos) {
        this->cameraPosition = cameraPos;
    }
}