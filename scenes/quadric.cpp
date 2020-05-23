#include "core.hpp"
#include <math.h>
#include <iostream>

Scene *make_scene() {
    Scene *scene = new Scene();

    Quadric *q = new Quadric(Transform::translate(0,0,10));
    Point points[9];
    float heights[3] = { -0.5, 0, 0.5 };
    float shifts[3] = { 0.10232, 0.314, 0.454333 };
    for (int i = 0; i < 3; i++) {
        float h = heights[i];
        float r = sqrt(1 - h*h);
        for (int j = 0; j < (i <= 2 ? 3 : 2); j++) {
            float theta = j * 2*M_PI/3 + shifts[i];
            if (theta > 2*M_PI) theta -= 2*M_PI;
            float c = cos(theta);
            float s = sin(theta);
            points[3*i + j] = Point(c*r, s*r, h);
        }
    }
    points[8] = Point(0,1,0);

    for (int i = 0; i < 10; i++) {
        std::cout << q->coefficients[i] << "\n";
    }

    q->pass_through_9_points(points);
    scene->add_primitive(q);

    scene->add_primitive(new Sphere(Transform::translate(-2,0,10), 1));
    scene->add_primitive(new Sphere(Transform::translate(2,0,10), 1));

    scene->add_light(new PointLight(Point(0,1,0), 10.f*RGB(1,1,1)));

    return scene;
}
