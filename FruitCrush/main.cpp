#include <iostream>
#include "SDL2/SDL.h"
#include <math.h>

#include "GameScene.h"

int main(int argc, char **argv)
{
    try
    {
        SDLRenderer ren;
        GameScene game(ren);
        
        game.runEventLoop();
    }
    catch(RendererException &re)
    {
        std::cout << re.error << std::endl;
        return 1;
    }
    
    return 0;
}
