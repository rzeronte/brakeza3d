#include <SDL2/SDL.h>
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Render/Logging.h"

ComponentWindow::ComponentWindow() {
    this->initWindow();
    this->initFontsTTF();
}

void ComponentWindow::onStart() {
    Logging::Log("ComponentWindow onStart", "ComponentWindow");
}

void ComponentWindow::preUpdate() {
    BUFFERS->clearDepthBuffer();
    BUFFERS->clearVideoBuffer();
}

void ComponentWindow::onUpdate() {
}

void ComponentWindow::postUpdate() {
    //SDL_UpdateTexture(this->screenTexture, nullptr, BUFFERS->videoBuffer, screenSurface->pitch);
    //SDL_RenderCopy(renderer, this->screenTexture, nullptr, nullptr);
    //SDL_RenderPresent(renderer);

    SDL_GL_SwapWindow( this->window );
    SDL_UpdateTexture( screenTexture, NULL, BUFFERS->videoBuffer, screenSurface->pitch );
    SDL_RenderCopy( renderer, screenTexture, NULL, NULL);
}

void ComponentWindow::onEnd() {
    TTF_CloseFont(fontDefault);
    TTF_CloseFont(fontSmall);
    TTF_CloseFont(fontBig);
    TTF_CloseFont(fontMedium);
    SDL_DestroyWindow(window);
    SDL_Quit();

    std::cout << std::endl << "Brakeza3D exit, good bye ;)" << std::endl;
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentWindow::initWindow() {
    Logging::Log("Initializating ComponentWindow...", "INFO");

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    } else {
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

        //Create window
        window = SDL_CreateWindow(
                SETUP->ENGINE_TITLE.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SETUP->screenWidth,
                SETUP->screenHeight,
                SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_MAXIMIZED
        );

        if (window == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        } else {
            contextOpenGL = SDL_GL_CreateContext(window);
            screenSurface = SDL_CreateRGBSurface(0, SETUP->screenWidth, SETUP->screenHeight, 32, 0, 0, 0, 0);

            SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_NONE);
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

            screenTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                                              SDL_TEXTUREACCESS_STREAMING,
                                              EngineSetup::get()->screenWidth,
                                              EngineSetup::get()->screenHeight
            );

            SDL_GL_SetSwapInterval(1); // Enable vsync
            SDL_GL_SwapWindow(window);
        }
    }
}

void ComponentWindow::initFontsTTF() {
    Logging::Log("Initializating TTF...", "INFO");

    // global font
    if (TTF_Init() < 0) {
        Logging::Log(TTF_GetError(), "INFO");
    } else {
        std::string pathFont = SETUP->FONTS_FOLDER + "octin.ttf";
        Logging::Log("Loading FONT: " + pathFont, "INFO");

        fontDefault = TTF_OpenFont(pathFont.c_str(), 50);
        fontSmall = TTF_OpenFont(pathFont.c_str(), 25);
        fontMedium = TTF_OpenFont(pathFont.c_str(), 70);
        fontBig = TTF_OpenFont(pathFont.c_str(), 200);

        if (!fontDefault) Logging::Log(TTF_GetError(), "INFO");
        if (!fontSmall) Logging::Log(TTF_GetError(), "INFO");
        if (!fontMedium) Logging::Log(TTF_GetError(), "INFO");
        if (!fontBig) Logging::Log(TTF_GetError(), "INFO");
    }
}

