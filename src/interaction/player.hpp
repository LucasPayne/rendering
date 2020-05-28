#ifndef INTERACTION_PLAYER_H
#define INTERACTION_PLAYER_H
#include "core.hpp"
#include "gl.hpp"
#include "mathematics.hpp"

enum FlyingModes {
    PlayerFly,
    PlayerFlyWalk, // This allows toggling.
    PlayerWalk,
};
class Player : public InputListener {
public:
    Point position;

    Vector velocity;
    uint8_t flying_mode;
    bool look_with_mouse; // Otherwise, look with the regular keys.
    // The azimuth is measured anti-clockwise.
    // It is the angle (1,0,0) makes with its image under the orientation matrix.
    float azimuth;
    // Altitude is measured upward. It is the angle that (0,0,1) makes with its image under the orientation matrix.
    float altitude;

    bool flying;
    float toggle_flying_timer;

    float scrollable_speed; // Option to use the scroll wheel for changing walking and flying speed.

    float speed;
    float min_speed;
    float max_speed;

    Player(float x, float y, float z, float azimuth, float altitude, float speed);

    void update();
    void lock_altitude();

    Transform get_transform() const {
        float caz = cos(azimuth);
        float saz = sin(azimuth);
        float cal = cos(altitude);
        float sal = sin(altitude);
        Vector right = Vector(caz, 0, saz);
        Vector straight_forward = Vector(-saz, 0, caz);
        Vector straight_up = glm::cross(straight_forward, right);
        Vector forward = cal*straight_forward + sal*straight_up;
        Vector up = glm::cross(forward, right);

        // mat4x4 m(right.x,right.y,right.z,0,
        //          up.x,up.y,up.z,0,
        //          forward.x,forward.y,forward.z,0,
        //          0,0,0,1);
        // return Transform::translate(position) * Transform(m, glm::transpose(m));
        mat4x4 m(right.x,right.y,right.z,0,
                 up.x,up.y,up.z,0,
                 forward.x,forward.y,forward.z,0,
                 position.x,position.y,position.z,1);
        return Transform(m);
    }
    // Input event callback functions, implementation of virtual methods in InputListener base class.
    void key_callback(int key, int action);
    void cursor_move_callback(double dx, double dy);
    // void cursor_position_callback(double x, double y);
    // void mouse_button_callback(int button, int action);
private:
};


// void PlayerController_mouse_move_listener(Logic *g, float x, float y, float dx, float dy);
// void PlayerController_key_listener(Logic *g, int key, int action, int mods);
// void PlayerController_scroll_listener(Logic *g, float dy);
// void PlayerController_update(Logic *g);
// PlayerController *Player_create_default(float x, float y, float z, float azimuth, float altitude);

#endif // INTERACTION_PLAYER_H
