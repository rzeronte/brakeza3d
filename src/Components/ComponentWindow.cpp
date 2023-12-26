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
    renderFramebuffer();
    SDL_RenderPresent(renderer);
    cleanFrameBuffers();
}

void ComponentWindow::onUpdate()
{
}

void ComponentWindow::postUpdate()
{
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
            SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS
        );
        context = SDL_GL_CreateContext( window );

        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1 );
        SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1 );
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8 );
        SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 8);

        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 4 );
        SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );
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

ShaderOpenGLDOF *ComponentWindow::getShaderOGLDOF() const {
    return shaderOGLDOF;
}

ShaderOpenGLOutliner *ComponentWindow::getShaderOglStencil() const {
    return shaderOGLStencil;
}

ShaderOpenGLColor *ComponentWindow::getShaderOglColor() const {
    return shaderOGLColor;
}

ShaderOpenGLParticles *ComponentWindow::getShaderOglParticles() const {
    return shaderOGLParticles;
}

GLuint ComponentWindow::getSceneFramebuffer() const {
    return sceneFramebuffer;
}

GLuint ComponentWindow::getBackgroundFramebuffer() const {
    return backgroundFramebuffer;
}

GLuint ComponentWindow::getUIFramebuffer() const {
    return uiFramebuffer;
}

GLuint ComponentWindow::getForegroundFramebuffer() const {
    return foregroundFramebuffer;
}


void ComponentWindow::initOpenGL()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    int w = EngineSetup::get()->screenWidth;
    int h = EngineSetup::get()->screenHeight;
    GLenum framebufferStatus;

    glGenFramebuffers(1, &globalFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, globalFramebuffer);

    glGenTextures(1, &globalTexture);
    glBindTexture(GL_TEXTURE_2D, globalTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, globalTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----

    glGenFramebuffers(1, &sceneFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);

    glGenTextures(1, &sceneTexture);
    glBindTexture(GL_TEXTURE_2D, sceneTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneTexture, 0);

    glGenTextures(1, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &backgroundFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFramebuffer);

    glGenTextures(1, &backgroundTexture);
    glBindTexture(GL_TEXTURE_2D, backgroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, backgroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // ----

    glGenFramebuffers(1, &foregroundFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, foregroundFramebuffer);

    glGenTextures(1, &foregroundTexture);
    glBindTexture(GL_TEXTURE_2D, foregroundTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, foregroundTexture, 0);

    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // ----
    glGenFramebuffers(1, &uiFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);

    glGenTextures(1, &uiTexture);
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, uiTexture, 0);
    framebufferStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (framebufferStatus != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error al configurar el framebuffer" << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shaderOGLRender = new ShaderOpenGLRender();
    shaderOGLImage = new ShaderOpenGLImage();
    shaderOGLLine = new ShaderOpenGLLine();
    shaderOGLWireframe = new ShaderOpenGLWireframe();
    shaderOGLShading = new ShaderOpenGLShading();
    shaderOGLPoints = new ShaderOpenGLPoints();
    shaderOGLStencil = new ShaderOpenGLOutliner();
    shaderOGLColor = new ShaderOpenGLColor();
    shaderOGLParticles = new ShaderOpenGLParticles();
    shaderOGLDOF = new ShaderOpenGLDOF();
}

void ComponentWindow::renderFramebuffer()
{
    const int w = EngineSetup::get()->screenWidth;
    const int h = EngineSetup::get()->screenHeight;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shaderOGLImage->renderTexture(backgroundTexture, 0, 0, w, h, 1.0, true, globalFramebuffer);
    shaderOGLImage->renderTexture(sceneTexture, 0, 0, w, h, 1.0, true, globalFramebuffer);

    if (EngineSetup::get()->ENABLE_DEPTH_OF_FIELD) {
        shaderOGLDOF->render(globalTexture, depthTexture);
        shaderOGLImage->renderTexture(shaderOGLDOF->getTextureResult(), 0, 0, w, h, 1.0, false, 0);
    } else{
        shaderOGLImage->renderTexture(globalTexture, 0, 0, w, h, 1.0, true, 0);
    }

    shaderOGLImage->renderTexture(foregroundTexture, 0, 0, w, h, 1.0, true, 0);
    shaderOGLImage->renderTexture(uiTexture, 0, 0, w, h, 1.0, true, 0);
}

void ComponentWindow::cleanFrameBuffers() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, foregroundFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, uiFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, backgroundFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, globalFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
