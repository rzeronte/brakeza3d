#include <SDL2/SDL.h>
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Render/Logging.h"

ComponentWindow::ComponentWindow()
:
    window(nullptr),
    renderer(nullptr),
    screenSurface(nullptr),
    screenTexture(nullptr),
    applicationIcon(IMG_Load(std::string(EngineSetup::get()->ICONS_FOLDER + EngineSetup::get()->iconApplication).c_str())),
    fontDefault(nullptr)
{
    this->initWindow();
    this->initFontsTTF();
}

void ComponentWindow::onStart()
{
}

void ComponentWindow::preUpdate()
{
}

void ComponentWindow::clearVideoBuffers()
{
    BUFFERS->clearDepthBuffer();
    BUFFERS->clearVideoBuffer();
}

void ComponentWindow::renderToWindow()
{
    SDL_RenderPresent(renderer);
    SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
}

void ComponentWindow::onUpdate()
{
}

void ComponentWindow::postUpdate()
{
    SDL_UpdateTexture(screenTexture, nullptr, BUFFERS->videoBuffer, screenSurface->pitch);
}

void ComponentWindow::onEnd()
{
    TTF_CloseFont(fontDefault);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(screenTexture);
    SDL_FreeSurface(screenSurface);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();

    Logging::Message("Brakeza3D exit, good bye ;)");
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentWindow::initWindow() {
    Logging::Log("Initializating ComponentWindow...");

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
        SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_MOD);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            EngineSetup::get()->screenWidth,
            EngineSetup::get()->screenHeight
        );

        SDL_SetWindowIcon(this->window, applicationIcon);
    }
}

void ComponentWindow::initFontsTTF()
{
    Logging::Log("Initializating TTF...");

    if (TTF_Init() < 0) {
        Logging::Log(TTF_GetError());
        exit(-1);
    }
    std::string pathFont = SETUP->FONTS_FOLDER + "TroubleFont.ttf";
    Logging::Log("Loading FONT: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 50);

    if (!fontDefault)  {
        Logging::Log(TTF_GetError());
        exit(-1);
    }
}

SDL_Window *ComponentWindow::getWindow() const {
    return window;
}

SDL_Renderer *ComponentWindow::getRenderer() const {
    return renderer;
}

SDL_Texture *ComponentWindow::getScreenTexture() const {
    return screenTexture;
}

TTF_Font *ComponentWindow::getFontDefault() const {
    return fontDefault;
}
