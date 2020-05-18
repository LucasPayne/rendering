#ifndef IMAGING_CAMERA_H
#define IMAGING_CAMERA_H
class Camera {
private:
    int m_resolution_x;
    int m_resolution_y;

    float m_near_plane_distance;
    float m_far_plane_distance;
    float m_near_plane_half_width;
    float m_near_plane_half_height;
    float m_fov; // The field of view is measured horizontally.
    float m_aspect_ratio;

    Point m_position;
public:
    Transform transform;

    Camera() {}
    // Aspect ratio is given as height/width.
    Camera(Point position, Point lookat, float fov, float aspect_ratio)
    {
        // Initialize private data.
        // Field of view is passed in degrees.
        m_fov = (M_PI/180.0) * fov;
        m_position = position;
        m_aspect_ratio = aspect_ratio;
        m_near_plane_distance = 1; // Generated rays are from the camera origin to points on the near plane.
        m_far_plane_distance = 10; // Nothing past the far plane is culled, so this is fine.
        m_near_plane_half_width = cos(0.5 * m_fov);
        m_near_plane_half_height = aspect_ratio * m_near_plane_half_width;

        // Initialize public data.
        transform = Transform::lookat(position, lookat, Vector(0,1,0));
    }
    inline const float fov() const {
        return m_fov;
    }
    inline const float aspect_ratio() const {
        return m_aspect_ratio;
    }
    inline Point lens_point(float x, float y) const {
        // Return a position on the near quad ("lens") in world space, given x,y in
        // coordinates
        // (0,1) [           ] (1,1)
        //       [           ]
        //       [           ]
        // (0,0) [           ] (1,0).
        return transform(Point((2*x - 1)*m_near_plane_half_width,
                               (2*y - 1)*m_near_plane_half_height,
                               m_near_plane_distance));
    }
    inline Point position() const {
        return m_position;
    }
};

#endif // IMAGING_CAMERA_H
