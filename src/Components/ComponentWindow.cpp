//
// Created by darkhead on 8/1/20.
//

#include <SDL.h>
#include "../../headers/Components/ComponentWindow.h"
#include "../../headers/EngineSetup.h"
#include "../../headers/EngineBuffers.h"
#include "../../imgui/examples/imgui_impl_opengl2.h"
#include "../../imgui/examples/imgui_impl_sdl.h"

ComponentWindow::ComponentWindow()
{
    this->initWindow();
    this->initFontsTTF();

}

void ComponentWindow::onStart() {
    std::cout << "ComponentWindow onStart" << std::endl;

}

void ComponentWindow::preUpdate() {
    EngineBuffers *buffers = EngineBuffers::getInstance();

    buffers->clearDepthBuffer();
    buffers->clearVideoBuffer();
}

void ComponentWindow::onUpdate()
{
}

void ComponentWindow::postUpdate()
{
    EngineBuffers::getInstance()->flipVideoBufferToSurface( this->screenSurface );

    SDL_GL_SwapWindow( this->window );

    screenTexture = SDL_CreateTextureFromSurface( renderer, screenSurface );
    SDL_RenderCopy( renderer, screenTexture, NULL, NULL);

}

void ComponentWindow::onEnd()
{
    TTF_CloseFont( fontDefault );
    TTF_CloseFont( fontSmall );
    TTF_CloseFont( fontBig );
    TTF_CloseFont( fontMedium );
    SDL_DestroyWindow( window );
    SDL_Quit();

    std::cout << std::endl << "Brakeza3D exit, good bye ;)" << std::endl;
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentWindow::initWindow() {

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

        //Create window
        window = SDL_CreateWindow(
                EngineSetup::getInstance()->ENGINE_TITLE.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                EngineSetup::getInstance()->screenWidth,
                EngineSetup::getInstance()->screenHeight,
                SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE
        );
        // | SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE

        if (window == NULL) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        } else {
            contextOpenGL = SDL_GL_CreateContext(window);
            screenSurface = SDL_CreateRGBSurface(0, EngineSetup::getInstance()->screenWidth, EngineSetup::getInstance()->screenHeight, 32, 0, 0, 0, 0);
            SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_NONE);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED );

            SDL_GL_SetSwapInterval(1); // Enable vsync

            //ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
            //ImGui_ImplOpenGL2_Init();

            //ImGuiIO& io = ImGui::GetIO();
            //io.WantCaptureMouse = false;
            //io.WantCaptureKeyboard = false;

            // Setup style
            //ImGui::StyleColorsDark();
            //ImGuiStyle& style = ImGui::GetStyle();
            //style.FrameBorderSize = 1.0f;


            SDL_GL_SwapWindow(window);
        }
    }
}

void ComponentWindow::initFontsTTF()
{
    //Logging::getInstance()->Log("Initializating TTF...", "INFO");

    // global font
    if (TTF_Init() < 0) {
        //Logging::getInstance()->Log(TTF_GetError(), "INFO");
    } else {
        std::string pathFont = EngineSetup::getInstance()->FONTS_FOLDER + "octin.ttf";
        //Logging::getInstance()->Log("Loading FONT: " + pathFont, "INFO");

        fontDefault = TTF_OpenFont( pathFont.c_str(), 50 );
        fontSmall   = TTF_OpenFont( pathFont.c_str(), 25 );
        fontMedium  = TTF_OpenFont( pathFont.c_str(), 70 );
        fontBig     = TTF_OpenFont( pathFont.c_str(), 200 );

        //if (!fontDefault) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        //if (!fontSmall) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        //if (!fontMedium) Logging::getInstance()->Log(TTF_GetError(), "INFO");
        //if (!fontBig) Logging::getInstance()->Log(TTF_GetError(), "INFO");
    }
}

