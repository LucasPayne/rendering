#ifndef GL_INPUT_H
#define GL_INPUT_H

enum Dir {
    Up, Down, Left, Right,
    NumDirs
};

enum SpecialKeys {
    SpecialKey_LeftShift,
    SpecialKey_RightShift,
    SpecialKey_Space,
    SpecialKey_Enter,
    SpecialKey_Tab,
    NUM_SPECIAL_KEYS
};

void key_callback_arrows_down(GLFWwindow *window, int key,
                int scancode, int action,
                int mods);
bool space_key_down(void);
bool left_shift_key_down(void);
void alt_set_arrow_key_up(int key);
void alt_set_arrow_key_down(int key);
bool alt_arrow_key_down(int key);
void set_arrow_key_up(int key);
void set_arrow_key_down(int key);
bool arrow_key_down(int key);

#endif // GL_INPUT_H
