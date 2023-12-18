#define GL_GLEXT_PROTOTYPES

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include "../../include/Components/ComponentWindow.h"
#include "../../include/Render/Logging.h"
#include "../../include/Misc/Tools.h"
#include "../../include/OpenGL/ShaderOpenGLImage.h"
#include "../../include/OpenGL/ShaderOpenGLRender.h"

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

void ComponentWindow::renderToWindow()
{
    SDL_GL_SwapWindow(window);
    SDL_RenderPresent(renderer);
    //SDL_RenderCopy(renderer, screenTexture, nullptr, nullptr);
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
}

void ComponentWindow::onSDLPollEvent(SDL_Event *event, bool &finish) {

}

void ComponentWindow::initWindow() {
    Logging::Message("Initializating ComponentWindow...");

    Logging::Message("Available video drivers:");

    for( int i = 0; i < SDL_GetNumRenderDrivers(); ++i ){
        SDL_RendererInfo rendererInfo = {};
        SDL_GetRenderDriverInfo( i, &rendererInfo );
        Logging::Message("Driver rendering: %s", rendererInfo.name);
    }

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
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
            SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED
        );
        context = SDL_GL_CreateContext( window );

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 3 );
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 3 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE );
        if (window == nullptr) {
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            exit(-1);
        }

#ifdef WIN32
        //SDL_SetHint(SDL_HINT_RENDER_DRIVER, "direct3d11");
#endif

        screenSurface = SDL_CreateRGBSurface(0, SETUP->screenWidth, SETUP->screenHeight, 32, 0, 0, 0, 0);
        SDL_SetSurfaceBlendMode(screenSurface, SDL_BLENDMODE_MOD);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
        initOpenGL();

        /*screenTexture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_RGBA32,
            SDL_TEXTUREACCESS_STREAMING,
            EngineSetup::get()->screenWidth,
            EngineSetup::get()->screenHeight
        );*/


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

    std::string pathFont = SETUP->FONTS_FOLDER + "TheLastCall-Regular.ttf";
    Logging::Log("Loading FONT: %s", pathFont.c_str());

    fontDefault = TTF_OpenFont(pathFont.c_str(), 35);

    if (!fontDefault)  {
        Logging::Log(TTF_GetError());
        exit(-1);
    }

    std::string pathAlternativeFont = SETUP->FONTS_FOLDER + "DisposableDroidBB.ttf";
    Logging::Log("Loading FONT: %s", pathAlternativeFont.c_str());

    fontAlternative = TTF_OpenFont(pathAlternativeFont.c_str(), 65);

    if (!fontAlternative)  {
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

TTF_Font *ComponentWindow::getFontDefault() {
    return fontDefault;
}

TTF_Font *ComponentWindow::getFontAlternative() {
    return fontAlternative;
}

void ComponentWindow::initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    shaderOGLRender = new ShaderOpenGLRender(
        "../shaders/Render.vertexshader",
        "../shaders/Render.fragmentshader"
    );

    shaderOGLImage = new ShaderOpenGLImage(
        "../shaders/Image.vertexshader",
        "../shaders/Image.fragmentshader"
    );

    shaderOGLLine = new ShaderOpenGLLine(
        "../shaders/Line.vertexshader",
        "../shaders/Line.fragmentshader"
    );

    shaderOGLWireframe = new ShaderOpenGLWireframe(
        "../shaders/Wireframe.vertexshader",
        "../shaders/Wireframe.fragmentshader"
    );

    shaderOGLShading = new ShaderOpenGLShading(
        "../shaders/Shading.vertexshader",
        "../shaders/Shading.fragmentshader"
    );

    shaderOGLPoints = new ShaderOpenGLPoints(
        "../shaders/Points.vertexshader",
        "../shaders/Points.fragmentshader"
    );

    shaderOGLStencil = new ShaderOpenGLOutliner(
        "../shaders/Outliner.vertexshader",
        "../shaders/Outliner.fragmentshader"
    );

    shaderOGLColor = new ShaderOpenGLColor(
        "../shaders/Color.vertexshader",
        "../shaders/Color.fragmentshader"
    );

    shaderCustomOGLParticles = new ShaderCustomOpenGLParticles("../shaders/Particles.shader");
}

ShaderOpenGLImage *ComponentWindow::getShaderOGLImage() const {
    return shaderOGLImage;
}

ShaderOpenGLRender *ComponentWindow::getShaderOGLRender() const {
    return shaderOGLRender;
}

ShaderOpenGLLine *ComponentWindow::getShaderOGLLine() const {
    return shaderOGLLine;
}

ShaderOpenGLWireframe *ComponentWindow::getShaderOglWireframe() const {
    return shaderOGLWireframe;
}

ShaderOpenGLShading *ComponentWindow::getShaderOglShading() const {
    return shaderOGLShading;
}

ShaderOpenGLPoints *ComponentWindow::getShaderOGLPoints() const {
    return shaderOGLPoints;
}

ShaderCustomOpenGLParticles *ComponentWindow::getShaderCustomOGLParticles() const {
    return shaderCustomOGLParticles;
}

ShaderOpenGLOutliner *ComponentWindow::getShaderOglStencil() const {
    return shaderOGLStencil;
}

ShaderOpenGLColor *ComponentWindow::getShaderOglColor() const {
    return shaderOGLColor;
}
