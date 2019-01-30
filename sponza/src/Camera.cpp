#include <sponza/sponza.hpp>

using namespace sponza;

Vector3 Camera::getPosition() const
{
    return position;
}

void Camera::setPosition(Vector3 p)
{
    position = p;
}

Vector3 Camera::getDirection() const
{
    return direction;
}

void Camera::setDirection(Vector3 d)
{
    direction = d;
}

float Camera::getVerticalFieldOfViewInDegrees() const
{
    return vertical_field_of_view_degrees;
}

void Camera::setVerticalFieldOfViewInDegrees(float d)
{
    vertical_field_of_view_degrees = d;
}

float Camera::getNearPlaneDistance() const
{
    return near_plane_distance;
}

void Camera::setNearPlaneDistance(float n)
{
    near_plane_distance = n;
}

float Camera::getFarPlaneDistance() const
{
    return far_plane_distance;
}

void Camera::setFarPlaneDistance(float f)
{
    far_plane_distance = f;
}

Vector3 Camera::getLinearVelocity() const
{
    return translation_speed;
}

void Camera::setLinearVelocity(Vector3 v)
{
    translation_speed = v;
}

Vector2 Camera::getRotationalVelocity() const
{
    return rotation_speed;
}

void Camera::setRotationalVelocity(Vector2 v)
{
    rotation_speed = v;
}
