#include "../../headers/Render/Engine.h"
#include "../../headers/Render/Drawable.h"
#include "../../headers/Objects/BSPEntity3D.h"
#include <chrono>
#include <iostream>
#include "LinearMath/btTransform.h"
#include "../../headers/Brakeza3D.h"
#include "../../headers/Render/Maths.h"
#include "../../headers/EngineBuffers.h"
#include "../../headers/Physics/BillboardBody.h"
#include <OpenCL/opencl.h>
#include <SDL_image.h>

#define MAX_SOURCE_SIZE (0x100000)

Engine::Engine()
{
}


void Engine::updateGUI()
{
    ImGui::NewFrame();

    Brakeza3D *brakeza3D = Brakeza3D::get();

    //bool open = true;
    //ImGui::ShowDemoWindow(&open);

    /*brakeza3D->getGUIManager()->draw(
            brakeza3D->getDeltaTime(),
            brakeza3D->finish,
            brakeza3D->getSceneObjects(),
            brakeza3D->getComponentCamera(),
            brakeza3D->tiles, brakeza3D->tilesWidth,
            visibleTriangles.size(),
            brakeza3D->getComponentWeapons()
    );*/

    ImGui::Render();
}

void Engine::updateWindow()
{
    Brakeza3D *brakeza3D = Brakeza3D::get();

    EngineBuffers::getInstance()->flipVideoBufferToSurface(Brakeza3D::get()->screenSurface);

    brakeza3D->updateFPS();

    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplSDL2_NewFrame( brakeza3D->window );

    updateGUI();

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

    SDL_GL_SwapWindow(brakeza3D->window);

    brakeza3D->screenTexture = SDL_CreateTextureFromSurface( brakeza3D->renderer, brakeza3D->screenSurface);
    SDL_RenderCopy( brakeza3D->renderer, brakeza3D->screenTexture, NULL, NULL);
    //SDL_FillRect( brakeza3D->screenSurface, NULL, Color::green() );
}



void Engine::resolveCollisions()
{

}

void Engine::Close()
{
/*    TTF_CloseFont( Brakeza3D::get()->fontDefault );
    TTF_CloseFont( Brakeza3D::get()->fontSmall );
    TTF_CloseFont( Brakeza3D::get()->fontBig );
    TTF_CloseFont( Brakeza3D::get()->fontMedium );*/

    SDL_DestroyWindow(Brakeza3D::get()->window );
    SDL_Quit();

    printf("\r\nBrakeza3D exit, good bye ;)");
}

