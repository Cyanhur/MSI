#include "parameters.h"
#include "graphic.h"
#include "evenements.h"
#include "microcam.h"
#include "coils.h"

using namespace std;

int main(int argc, char** argv)
{
    printf("---Initialisation SDL---\n");
    if(SDL_Init(SDL_INIT_VIDEO)<0){
        printf(">Erreur d' initialisation SDL : %s \n",SDL_GetError());
    } else {printf(">Ok");}

    //Initialisation camera
    printf("\n\n---Initialisation Camera---");
    //Webcam camera;
    MicroCam camera;
    camera.start();
    SDL_Delay(2000);

    //Initialisation video
    printf("\n\n---Initialisation Inferface Graphique---");
    Graphic window;
    window.start();
    SDL_Delay(2000);

    //Initialisation event manager
    printf("\n\n---Création du Gestionnaire d'Evenement---\n");
    Input in;
    in.quit = false;
    memset(&in.mouseButtons,0,sizeof(in.mouseButtons));
    SDL_Delay(1000);

    //Initialisation s826
    printf("\n\n---Initialisation Control Bobine---\n");
    Coils helmholtz;
    helmholtz.start();
    SDL_Delay(2000);

    printf("\n\n---Début boucle principale---\n");
    while(!in.key[SDLK_ESCAPE] && !in.quit){
        updatEvents(&in);
        managEvents(&in, &camera, &window);
        if(mmcArena.n > 0)printf("N : %d\n", mmcArena.n);
        SDL_Delay(10);
    }
    printf("\n---Fin boucle principale---\n\n");
    //Close
    helmholtz.close();
    while(helmholtz.isRunning()){SDL_Delay(10);}
    window.close();
    while(window.isRunning()){SDL_Delay(10);}
    //camera.close();
    //while(camera.isRunning()){usleep(10);}
    SDL_Quit();
    return 0;
}
