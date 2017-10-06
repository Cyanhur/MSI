#ifndef EVENEMENTS_H
#define EVENEMENTS_H

#include "graphic.h"
#include "microcam.h"

struct Input{
    std::map<int,int> key;
    int mouseX, mouseY;
    int mouseRelX, mouseRelY;
    char mouseButtons[8];
    bool quit;
};

void managEvents(Input *in, MicroCam* camera, Graphic* window);
void updatEvents(Input *in);

#endif // EVENEMENTS_H
