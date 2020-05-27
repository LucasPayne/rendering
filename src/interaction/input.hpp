#ifndef INTERACTION_INPUT_H
#define INTERACTION_INPUT_H

class InputListener {
private:
public:
    bool listening;
    virtual void key_callback(int key, int action) {}
    virtual void cursor_position_callback(double x, double y) {}
    virtual void cursor_move_callback(double x, double y) {}
    virtual void mouse_button_callback(int button, int action) {}
};

#endif // INTERACTION_INPUT_H
