#ifndef IMAGING_CAMERA_H
#define IMAGING_CAMERA_H
#include "core.hpp"
#include "mathematics.hpp"

class Camera {
private:
    float m_near_plane_distance;
    float m_far_plane_distance;
    float m_near_plane_half_width;
    float m_near_plane_half_height;
    float m_fov; // The field of view is measured horizontally.
    float m_aspect_ratio;
public:
    Transform camera_to_world, world_to_camera;

    Camera() {}
    // Aspect ratio is given as height/width.

    Camera(Point position, Point lookat, float fov, float aspect_ratio)
    {
        // Initialize private data.
        // Horizontal field of view is passed in degrees.
        m_fov = (M_PI/180.0) * fov;
        m_aspect_ratio = aspect_ratio;
        m_near_plane_distance = 1; // Generated rays are from the camera origin to points on the near plane.
        m_far_plane_distance = 10; // Nothing past the far plane is culled, so this is fine.
        m_near_plane_half_width = m_near_plane_distance * cos(0.5 * m_fov);
        m_near_plane_half_height = aspect_ratio * m_near_plane_half_width;

        // Initialize public data.
        camera_to_world = Transform::lookat(position, lookat, Vector(0,1,0));
        world_to_camera = camera_to_world.inverse();
    }
    inline const float fov() const {
        return m_fov;
    }
    inline const float aspect_ratio() const {
        return m_aspect_ratio;
    }
    
    inline float imaging_plane_width() const {
        return 2*m_near_plane_half_width;
    }
    inline float imaging_plane_height() const {
        return 2*m_near_plane_half_height;
    }

    inline Point lens_point(float x, float y) const {
        // Return a position on the near quad ("lens") in world space, given x,y in
        // coordinates
        // (0,1) [           ] (1,1)
        //       [           ]
        //       [           ]
        // (0,0) [           ] (1,0).

        // Uncomment to test if the camera has been oriented correctly.
        // std::cout << "X: " << camera_to_world(Point(1,0,0)) << "\n";
        // std::cout << "Y: " << camera_to_world(Point(0,1,0)) << "\n";
        // std::cout << "Z: " << camera_to_world(Point(0,0,1)) << "\n";

        return camera_to_world(Point((2*x - 1)*m_near_plane_half_width,
                                     (2*y - 1)*m_near_plane_half_height,
                                     m_near_plane_distance));
    }
    
    Point position() const {
        return camera_to_world(Point(0,0,0));
    }
    void set_transform(Transform transform) {
        camera_to_world = transform;
        world_to_camera = transform.inverse();
    }
};

#endif // IMAGING_CAMERA_H
