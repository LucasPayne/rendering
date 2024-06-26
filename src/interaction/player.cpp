#include "player.hpp"
#include "interaction.hpp"
#include "gl.hpp"

void Player::lock_altitude()
{
    float altitude_up_cap = M_PI / 2 - 0.05;
    float altitude_down_cap = -(M_PI / 2 - 0.3);
    if (altitude > altitude_up_cap) altitude = altitude_up_cap;
    if (altitude < altitude_down_cap) altitude = altitude_down_cap;
}
void Player::key_callback(int key, int action)
{
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_E) {
            look_with_mouse = !look_with_mouse;
        }
        if (key == GLFW_KEY_X) {
            scrollable_speed = !scrollable_speed;
        }
        if (key == GLFW_KEY_R) {
            position = Point(0,0,0);
            azimuth = 0;
            altitude = 0;
        }
    }
}
void Player::cursor_move_callback(double dx, double dy)
{
    float mouse_sensitivity = 2;
    if (look_with_mouse) {
        azimuth -= dx * mouse_sensitivity;
        altitude += dy * mouse_sensitivity;
        lock_altitude();
    }
}

void Player::update()
{
    float gravitational_acceleration = 9.81;
    if (!flying) velocity.y -= gravitational_acceleration * dt;

    float caz = cos(azimuth);
    float saz = sin(azimuth);
    float cal = cos(altitude);
    float sal = sin(altitude);

    Vector right = Vector(caz, 0, saz);
    Vector forward = Vector(-saz, 0, caz);
    float move_x = 0, move_z = 0;
    if (alt_arrow_key_down(Right)) move_x += speed * dt;
    if (alt_arrow_key_down(Left)) move_x -= speed * dt;
    if (alt_arrow_key_down(Up)) move_z += speed * dt;
    if (alt_arrow_key_down(Down)) move_z -= speed * dt;
    Vector movement = right*move_x + forward*move_z;
    position = position + movement;

    float vertical_flying_speed = speed * 2.0/3.0;
    if (flying) {
        if (space_key_down()) {
            position.y += dt * vertical_flying_speed;
        }
        if (left_shift_key_down()) {
            position.y -= dt * vertical_flying_speed;
        }
    }

    if (!look_with_mouse) {
        float look_speed = 0.8;
        if (arrow_key_down(Right)) azimuth -= look_speed * dt;
        if (arrow_key_down(Left)) azimuth += look_speed * dt;
        if (arrow_key_down(Down)) {
            altitude -= look_speed * dt;
            lock_altitude();
        }
        if (arrow_key_down(Up)) {
            altitude += look_speed * dt;
            lock_altitude();
        }
    }
}


Player::Player(float x, float y, float z, float _azimuth, float _altitude, float _speed)
{
    position = Point(x, y, z);
    azimuth = _azimuth;
    altitude = _altitude;

    flying_mode = PlayerFlyWalk;
    flying = true;
    look_with_mouse = true;
                                     
    speed = _speed;
    min_speed = speed * 0.1;
    max_speed = speed * 10;
    scrollable_speed = true;
}
