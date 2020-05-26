#ifndef INTERACTION_INPUT_H
#define INTERACTION_INPUT_H

class InputListener {
private:
public:
    virtual void key_callback(int key, int action) = 0;
    virtual void cursor_position_callback(double x, double y) = 0;
    virtual void mouse_button_callback(int button, int action) = 0;
};

#endif // INTERACTION_INPUT_H
