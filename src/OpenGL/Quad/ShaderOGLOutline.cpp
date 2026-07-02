
#include <fstream>
#include <sstream>
#include "../../../include/OpenGL/Quad/ShaderOGLOutline.h"
#include "../../../include/OpenGL/ShaderOGLImage.h"
#include "../../../include/Components/Components.h"
#include "../../../include/3D/Image3D.h"
#include "../../../include/Misc/Logging.h"

ShaderOGLOutline::ShaderOGLOutline()
:
    ShaderBaseOpenGL(
        Config::get()->SHADERS_FOLDER + "Outliner.vs",
        Config::get()->SHADERS_FOLDER + "Outliner.fs",
        false
    )
{
}

void ShaderOGLOutline::PrepareMainThread()
{
    ShaderBaseOpenGL::PrepareMainThread();
    LoadUniforms();
    CreateQuadVBO();
    setQuadSize(RENDER_SIZE);
    SetupQuadUniforms(programID);

    glGenVertexArrays(1, &expandVAO);
    expandProgramID = CompileExpandProgram();
    if (expandProgramID) {
        expandModelUniform       = glGetUniformLocation(expandProgramID, "model");
        expandViewUniform        = glGetUniformLocation(expandProgramID, "view");
        expandProjectionUniform  = glGetUniformLocation(expandProgramID, "projection");
        expandThicknessUniform   = glGetUniformLocation(expandProgramID, "outlineThickness");
        expandColorUniform       = glGetUniformLocation(expandProgramID, "lineColor");
        expandCenterUniform      = glGetUniformLocation(expandProgramID, "submeshCenter");
    }

    CreateStencilFBO();
}

void ShaderOGLOutline::LoadUniforms()
{
    textureUniform = glGetUniformLocation(programID, "image");
}

// ---------------------------------------------------------------------------
// Stencil FBO: RGBA color + depth24/stencil8
// ---------------------------------------------------------------------------
void ShaderOGLOutline::CreateStencilFBO()
{
    auto window = Components::get()->Window();
    const int w = window->getWidthRender();
    const int h = window->getHeightRender();

    if (stencilFBO)              { glDeleteFramebuffers(1, &stencilFBO);               stencilFBO = 0; }
    if (stencilColorBuffer)      { glDeleteTextures(1, &stencilColorBuffer);            stencilColorBuffer = 0; }
    if (stencilDepthStencilRBO)  { glDeleteRenderbuffers(1, &stencilDepthStencilRBO);  stencilDepthStencilRBO = 0; }

    // ── color attachment ───────────────────────────────────────────────────
    glGenTextures(1, &stencilColorBuffer);
    glBindTexture(GL_TEXTURE_2D, stencilColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // ── depth+stencil renderbuffer ─────────────────────────────────────────
    glGenRenderbuffers(1, &stencilDepthStencilRBO);
    glBindRenderbuffer(GL_RENDERBUFFER, stencilDepthStencilRBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);

    // ── assemble FBO ───────────────────────────────────────────────────────
    glGenFramebuffers(1, &stencilFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, stencilFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, stencilColorBuffer, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, stencilDepthStencilRBO);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("[ShaderOGLOutline] FBO incomplete (0x%x) — outlines disabled", status);
        glDeleteFramebuffers(1, &stencilFBO);      stencilFBO = 0;
        glDeleteTextures(1, &stencilColorBuffer);  stencilColorBuffer = 0;
        glDeleteRenderbuffers(1, &stencilDepthStencilRBO); stencilDepthStencilRBO = 0;
    } else {
        LOG_MESSAGE("[ShaderOGLOutline] FBO OK (%dx%d)", w, h);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

// ---------------------------------------------------------------------------
// Compile the expand (scaled-mesh) shader from OutlinerExpand.vs/.fs
// ---------------------------------------------------------------------------
GLuint ShaderOGLOutline::CompileExpandProgram()
{
    static const char *vsCode = R"(
#version 330 core
layout(location=0) in vec4 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float outlineThickness;
uniform vec3 submeshCenter;
void main() {
    gl_Position = projection * view * model * vec4(aPos.xyz, 1.0);
    vec4 cc = projection * view * model * vec4(submeshCenter, 1.0);
    vec2 d = (gl_Position.xy / gl_Position.w) - (cc.xy / cc.w);
    float dl = length(d);
    if (dl > 0.001) { d /= dl; } else { d = vec2(1.0, 0.0); }
    gl_Position.xy += d * (outlineThickness * 0.05) * gl_Position.w;
}
)";
    static const char *fsCode = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 lineColor;
void main() { FragColor = vec4(lineColor, 1.0); }
)";
    // (no file I/O needed)

    auto compileShader = [](GLenum type, const char *src, const char *tag) -> GLuint {
        GLuint id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);
        GLint ok; glGetShaderiv(id, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char log[512]; glGetShaderInfoLog(id, 512, nullptr, log);
            LOG_ERROR("[ShaderOGLOutline] %s: %s", tag, log);
            glDeleteShader(id);
            return 0u;
        }
        return id;
    };

    GLuint vs = compileShader(GL_VERTEX_SHADER,   vsCode, "OutlinerExpand.vs");
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fsCode, "OutlinerExpand.fs");
    if (!vs || !fs) { glDeleteShader(vs); glDeleteShader(fs); return 0; }

    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs);
    glAttachShader(prog, fs);
    glLinkProgram(prog);

    GLint ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[512]; glGetProgramInfoLog(prog, 512, nullptr, log);
        LOG_ERROR("[ShaderOGLOutline] Expand link error: %s", log);
        glDeleteProgram(prog); prog = 0;
    }

    glDetachShader(prog, vs); glDeleteShader(vs);
    glDetachShader(prog, fs); glDeleteShader(fs);

    if (prog) {
        LOG_MESSAGE("[ShaderOGLOutline] Expand program compiled OK (ID=%d)", prog);
    } else {
        LOG_ERROR("[ShaderOGLOutline] Expand program FAILED — outlines disabled");
    }
    return prog;
}

// ---------------------------------------------------------------------------
// Blend-based outline (no stencil test).
//   Pass 1: expanded mesh → outline color fills entire expanded silhouette in stencilFBO
//   Pass 2: original mesh → GL_ZERO/GL_ZERO clears interior back to (0,0,0,0)
//   Result in stencilFBO: ring pixels colored (alpha=1), interior transparent (0,0,0,0)
//   Then blit stencilFBO → destFBO with alpha blending (ring visible, interior preserves destFBO)
// ---------------------------------------------------------------------------
void ShaderOGLOutline::stencilOutlineMesh(
    GLuint vertexBuf, GLuint uvBuf, GLuint normalBuf, int count,
    const glm::mat4 &model, const Color &c, float thickness, bool clearFirst,
    GLuint destFBO, const glm::vec3 &submeshCenter)
{
    if (!expandProgramID) {
        LOG_ERROR("[Outline] expandProgramID=0 — shader no compiló, outline desactivado");
        return;
    }
    if (!stencilFBO) {
        LOG_ERROR("[Outline] stencilFBO=0 — FBO no creado, outline desactivado");
        return;
    }
    if (count <= 0) return;

    auto render      = Components::get()->Render();
    auto camera      = Components::get()->Camera();
    auto window      = Components::get()->Window();
    auto shaderColor = render->getShaders()->shaderOGLColor;

    glm::mat4 proj = camera->getGLMMat4ProjectionMatrix();
    glm::mat4 view = camera->getGLMMat4ViewMatrix();
    glm::vec3 col  = c.toGLM();
    const int w    = window->getWidthRender();
    const int h    = window->getHeightRender();

    // ── Bind stencilFBO, clear only on first submesh ─────────────────────
    render->ChangeOpenGLFramebuffer(stencilFBO);
    glViewport(0, 0, w, h);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    if (clearFirst) {
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_SCISSOR_TEST);

    glBindVertexArray(expandVAO);
    ShaderBaseOpenGL::setVAOAttributes(vertexBuf, uvBuf, normalBuf);

    // ── Pass 1: draw EXPANDED mesh in outline color (solid filled silhouette) ──
    glDisable(GL_BLEND);
    render->ChangeOpenGLProgram(expandProgramID);
    glUniformMatrix4fv(expandModelUniform,      1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(expandViewUniform,       1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(expandProjectionUniform, 1, GL_FALSE, &proj[0][0]);
    glUniform1f(expandThicknessUniform, thickness);
    glUniform3fv(expandColorUniform, 1, &col[0]);
    glUniform3fv(expandCenterUniform, 1, &submeshCenter[0]);
    glDrawArrays(GL_TRIANGLES, 0, count);

    // ── Pass 2: draw ORIGINAL mesh → zero-out interior (leaving only the ring) ──
    glEnable(GL_BLEND);
    glBlendFuncSeparate(GL_ZERO, GL_ZERO, GL_ZERO, GL_ZERO);
    render->ChangeOpenGLProgram(shaderColor->getProgramID());
    shaderColor->setMat4("projection", proj);
    shaderColor->setMat4("view",       view);
    shaderColor->setMat4("model",      model);
    shaderColor->setVec3("color", glm::vec3(0.0f));
    glDrawArrays(GL_TRIANGLES, 0, count);

    // ── Restore ───────────────────────────────────────────────────────────
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glBindVertexArray(0);

}

void ShaderOGLOutline::blitToFBO(GLuint destFBO)
{
    if (!stencilFBO || !stencilColorBuffer) return;

    auto render = Components::get()->Render();
    auto window = Components::get()->Window();
    const int w = window->getWidthRender();
    const int h = window->getHeightRender();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    render->getShaders()->shaderOGLImage->renderTexture(
        stencilColorBuffer, 0, 0, w, h, w, h, 1.0f, true, destFBO
    );

    glDisable(GL_BLEND);
}

// ---------------------------------------------------------------------------
// Public draw methods
// ---------------------------------------------------------------------------

void ShaderOGLOutline::drawOutline(Mesh3D *m, const Color &c, float borderThickness, GLuint fbo)
{
    bool first = true;
    for (const auto &mm : m->getMeshData()) {
        stencilOutlineMesh(mm.vertexBuffer, mm.uvBuffer, mm.normalBuffer,
                           (int)mm.vertices.size(), m->getModelMatrix(), c, borderThickness, first,
                           fbo, mm.localAabb.getCenter().toGLM());
        first = false;
    }
    blitToFBO(fbo);
}

void ShaderOGLOutline::drawOutlineSubmesh(Mesh3D *m, const std::string &submeshName, const Color &c, float borderThickness, GLuint fbo)
{
    std::string prefix = submeshName;
    const auto dot = submeshName.rfind('.');
    if (dot != std::string::npos) prefix = submeshName.substr(0, dot);

    bool first = true;
    for (const auto &mm : m->getMeshData()) {
        if (mm.name.rfind(prefix, 0) != 0) continue;
        stencilOutlineMesh(mm.vertexBuffer, mm.uvBuffer, mm.normalBuffer,
                           (int)mm.vertices.size(), m->getModelMatrix(), c, borderThickness, first,
                           fbo, mm.localAabb.getCenter().toGLM());
        first = false;
    }
    if (!first) blitToFBO(fbo);
}

void ShaderOGLOutline::drawOutlineImage3D(Image3D *i, const Color &c, float borderThickness, GLuint fbo)
{
    stencilOutlineMesh(i->getVertexBuffer(), i->getUVBuffer(), i->getNormalBuffer(),
                       (int)i->getVertices().size(), i->getModelMatrix(), c, borderThickness, true,
                       fbo, glm::vec3(0.0f));
    blitToFBO(fbo);
}

void ShaderOGLOutline::drawOutline(Mesh3DAnimation *m, Color c, float borderThickness, GLuint fbo)
{
    bool first = true;
    for (const auto &mm : m->getMeshData()) {
        stencilOutlineMesh(mm.feedbackBuffer, mm.uvBuffer, mm.feedbackNormalBuffer,
                           (int)mm.vertices.size(), m->getModelMatrix(), c, borderThickness, first,
                           fbo, mm.localAabb.getCenter().toGLM());
        first = false;
    }
    blitToFBO(fbo);
}

// ---------------------------------------------------------------------------
// Batch API
// ---------------------------------------------------------------------------

void ShaderOGLOutline::clearOutlineBatch()
{
    if (!stencilFBO) return;
    auto render = Components::get()->Render();
    auto window = Components::get()->Window();
    render->ChangeOpenGLFramebuffer(stencilFBO);
    glViewport(0, 0, window->getWidthRender(), window->getHeightRender());
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void ShaderOGLOutline::drawOutlineSubmeshBatch(Mesh3D *m, const std::string &submeshName, const Color &c, float borderThickness)
{
    std::string prefix = submeshName;
    const auto dot = submeshName.rfind('.');
    if (dot != std::string::npos) prefix = submeshName.substr(0, dot);

    bool first = true;
    for (const auto &mm : m->getMeshData()) {
        if (mm.name.rfind(prefix, 0) != 0) continue;
        // clearFirst=false: el stencilFBO ya fue limpiado por clearOutlineBatch()
        stencilOutlineMesh(mm.vertexBuffer, mm.uvBuffer, mm.normalBuffer,
                           (int)mm.vertices.size(), m->getModelMatrix(), c, borderThickness, false,
                           0, mm.localAabb.getCenter().toGLM());
        first = false;
    }
    (void)first;
}

void ShaderOGLOutline::flushOutlines(GLuint destFBO)
{
    blitToFBO(destFBO);
}

void ShaderOGLOutline::Destroy()
{
    ResetQuadMatrix();

    // expandProgramID and expandVAO are size-independent — keep them across resize.
    // Only the stencilFBO resources depend on render resolution; recreate them.
    if (stencilFBO)             { glDeleteFramebuffers(1, &stencilFBO);                 stencilFBO = 0; }
    if (stencilColorBuffer)     { glDeleteTextures(1, &stencilColorBuffer);             stencilColorBuffer = 0; }
    if (stencilDepthStencilRBO) { glDeleteRenderbuffers(1, &stencilDepthStencilRBO);   stencilDepthStencilRBO = 0; }

    if (expandProgramID) {
        CreateStencilFBO();
    }
}
