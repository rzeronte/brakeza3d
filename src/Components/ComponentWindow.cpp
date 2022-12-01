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

void ComponentWindow::onUpdate()
{
    SDL_RenderPresent(renderer);
    SDL_RenderClear(renderer);
}

void ComponentWindow::postUpdate()
{
    SDL_UpdateTexture(screenTexture, nullptr, BUFFERS->videoBuffer, screenSurface->pitch );
    SDL_RenderCopy(renderer,screenTexture, nullptr, nullptr);
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
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(-1);
    } else {
        //Create window
        window = SDL_CreateWindow(
                SETUP->ENGINE_TITLE.c_str(),
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                SETUP->screenWidth,
                SETUP->screenHeight,
                SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
        );

        if (window == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }

        screenSurface = SDL_CreateRGBSurface(0, SETUP->screenWidth, SETUP->screenHeight, 32, 0, 0, 0, 0);

        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");

        SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_MOD);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_TARGET,
            EngineSetup::get()->screenWidth,
            EngineSetup::get()->screenHeight
        );
    }
}

void ComponentWindow::initFontsTTF()
{
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
