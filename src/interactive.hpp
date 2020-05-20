#ifndef INTERACTIVE_H
#define INTERACTIVE_H

void init_gl();
void close_gl();
void init_loop(Renderer *renderer);
void loop(Renderer &renderer);

#endif // INTERACTIVE_H
