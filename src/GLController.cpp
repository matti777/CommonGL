#include <string.h>

#include "GLController.h"
#include "MatrixOperations.h"
#include "CommonFunctions.h"
#include "BaseWidget.h"
#include "Container.h"

GLController::GLController()
    : m_simpleColorProgram(0),
      m_simpleColorMvpLoc(-1),
      m_simpleColorColorLoc(-1),
      m_viewportWidth(-1),
      m_viewportHeight(-1),
      m_fullscreenRectVertexBuffer(0),
      m_fullScreenRect(CommonGL::Rect()),
      m_hasDepthTextureExtension(false),
      m_widgetContext(CommonGL::WidgetContext()),
      m_defaultFrameBuffer(DefaultFramebufferId)
{
}

GLController::~GLController()
{
    DeinitWidgets();
    m_widgets.clear();
}

CommonGL::BaseWidget* GLController::FindWidget(int x, int y)
{
    std::list<CommonGL::BaseWidget*>::iterator iter;
    for ( iter = m_widgets.begin(); iter != m_widgets.end(); iter++ )
    {
        if ( (*iter)->HitTest(x, y) && (*iter)->IsVisible() )
        {
            return *iter;
        }
    }

    return NULL;
}

bool GLController::TouchMoved(const void* /*touch*/, int x, int y)
{
//    LOG_DEBUG("TouchMoved()");

    if ( m_widgetContext.m_pressedWidget != NULL )
    {
        // Check if touch is still inside the widget; if not, clear pressed
        if ( !m_widgetContext.m_pressedWidget->HitTest(x, y) )
        {
            LOG_DEBUG("moved: touch outside, clearing!");
            m_widgetContext.m_pressedWidget = NULL;
        }
        return true;
    }

    return false;
}

bool GLController::TouchStarted(const void* /*touch*/, int x, int y)
{
    LOG_DEBUG("TouchStarted()");

    CommonGL::BaseWidget* widget = FindWidget(x, y);
    if ( widget != NULL )
    {
        // A widget was pressed
        widget->TouchDownInside(x, y);
        if ( widget->m_type == CommonGL::BaseWidget::TypeButton )
        {
            // Only process a new touch for pressable widgets if there is no
            // active pressed one
            if ( m_widgetContext.m_pressedWidget != NULL )
            {
                LOG_DEBUG("start: m_widgetContext.m_pressedWidget != NULL");
                return false;
            }

            LOG_DEBUG("start: press OK");
            m_widgetContext.m_pressedWidget = widget;
        }
        return true;
    }
    else
    {
        LOG_DEBUG("No widget found for touch");
    }

    return false;
}

bool GLController::TouchEnded(const void* /*touch*/, int x, int y)
{
    LOG_DEBUG("TouchEnded()");

    if ( m_widgetContext.m_pressedWidget != NULL )
    {
        if ( m_widgetContext.m_pressedWidget->HitTest(x, y) )
        {
            LOG_DEBUG("end: touch up OK!");
            // Signal the pressed widget about touch up and release it
            m_widgetContext.m_pressedWidget->TouchUpInside(x, y);
            m_widgetContext.m_pressedWidget = NULL;
            return true;
        }
        else
        {
            // Touch up happened outside
            m_widgetContext.m_pressedWidget = NULL;
            return false;
        }
    }

    return false;
}

void GLController::DrawWidgets()
{
    GLuint currentProgram = 0;
    const float noHighlight[] = { 0.0, 0.0, 0.0 };
    const float highlight[] = { 0.2, 0.2, 0.2 };

    std::list<CommonGL::BaseWidget*>::reverse_iterator iter;
    for ( iter = m_widgets.rbegin(); iter != m_widgets.rend(); iter++ )
    {
        CommonGL::BaseWidget* widget = *iter;

        if ( widget->IsVisible() )
        {
            // Set up the widget rendering shader program
            if ( widget->m_type == CommonGL::BaseWidget::TypeButton )
            {
                if ( currentProgram != m_widgetContext.m_textureShaderProgram )
                {
                    glUseProgram(m_widgetContext.m_textureShaderProgram);
                    glUniformMatrix4fv(m_widgetContext.m_textureShaderMvpLoc,
                                       1, GL_FALSE, m_orthoProjectionMatrix);
                    currentProgram = m_widgetContext.m_textureShaderProgram;
                }

                const float* h =
                 ( widget == m_widgetContext.m_pressedWidget ) ?
                            highlight : noHighlight;
                glUniform3fv(m_widgetContext.m_textureShaderHighlightLoc, 1, h);
            }
            else
            {
//                LOG_DEBUG("drawing Container!");
                if ( currentProgram != m_widgetContext.m_colorShaderProgram )
                {
                    glUseProgram(m_widgetContext.m_colorShaderProgram);
                    glUniformMatrix4fv(m_widgetContext.m_colorShaderMvpLoc,
                                       1, GL_FALSE, m_orthoProjectionMatrix);
                    currentProgram = m_widgetContext.m_colorShaderProgram;
                }
            }

            // Render the widget
            widget->Render();
        }
    }
}

void GLController::PinchGesture(float /*scaleFactor*/, float /*rotationAngle*/)
{
    // no implementation
}

bool GLController::InitWidgets()
{
    if ( m_widgetContext.m_textureShaderProgram > 0 )
    {
        LOG_DEBUG("GLController::InitWidgets(): already initialized!");
        return true;
    }

    // Load the shader program for drawing widgets
    if ( !LoadShaderFromBundle("ImageWidget",
                               &(m_widgetContext.m_textureShaderProgram)) )
    {
        return false;
    }

    // Get uniform locations
    m_widgetContext.m_textureShaderMvpLoc =
            glGetUniformLocation(m_widgetContext.m_textureShaderProgram,
                                 "mvp_matrix");
    m_widgetContext.m_textureShaderTextureLoc =
            glGetUniformLocation(m_widgetContext.m_textureShaderProgram,
                                 "texture");
    m_widgetContext.m_textureShaderHighlightLoc =
            glGetUniformLocation(m_widgetContext.m_textureShaderProgram,
                                 "highlight");

    m_widgetContext.m_colorShaderProgram = m_simpleColorProgram;
    m_widgetContext.m_colorShaderMvpLoc = m_simpleColorMvpLoc;
    m_widgetContext.m_colorShaderColorLoc = m_simpleColorColorLoc;

    // Set up the rest of the widget context
    m_widgetContext.m_viewportWidth = m_viewportWidth;
    m_widgetContext.m_viewportHeight = m_viewportHeight;
    m_widgetContext.m_fullscreenRectVertexBuffer = m_fullscreenRectVertexBuffer;
    m_widgetContext.m_glController = this;

    return true;
}

void GLController::DeinitWidgets()
{
    UnloadShader(m_widgetContext.m_textureShaderProgram);
    m_widgetContext = CommonGL::WidgetContext();
}

void GLController::Add(CommonGL::BaseWidget* widget)
{
    // Attach the widget
    widget->SetContext(&m_widgetContext);

    // Add the widget to the list
    m_widgets.push_front(widget);
}

void GLController::Remove(CommonGL::BaseWidget* widget)
{
    // Detach the widget
    widget->SetContext(NULL);

    // Remove the widget from the list
    m_widgets.remove(widget);
}

bool GLController::InitController()
{
    // general commmon OpenGL setup
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClearStencil(0);
//    glClearDepthf(1.0f);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(true);
    LOG_DEBUG("GLController::InitController() GL error: %d", glGetError());

    if ( !LoadShaderFromBundle("SimpleColor", &m_simpleColorProgram) )
    {
        return false;
    }
    m_simpleColorMvpLoc = glGetUniformLocation(m_simpleColorProgram,
                                               "mvp_matrix");
    m_simpleColorColorLoc = glGetUniformLocation(m_simpleColorProgram,
                                                 "color");
    LOG_GL_ERROR();

    return true;
}

void GLController::ViewportResized(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    // Reset orthographic projection matrix
    MatrixOrthographicProjection(m_orthoProjectionMatrix,
                                 -m_viewportWidth/2, m_viewportWidth/2,
                                 -m_viewportHeight/2, m_viewportHeight/2,
                                 -m_viewportWidth/2, m_viewportWidth/2);

    glViewport(0, 0, m_viewportWidth, m_viewportHeight);

    // Fullscreen rectangle
    m_fullScreenRect.Set(0, 0, m_viewportWidth, m_viewportHeight);

    // Delete existing fader buffer
    glDeleteBuffers(1, &m_fullscreenRectVertexBuffer);

    // adjust x/y according to viewport size so that 0,0 is upper left
    int x = -m_viewportWidth / 2;
    int y = -m_viewportHeight + (m_viewportHeight / 2);

    VertexAttribsCoordsOnly faderVertices[] = {
        { x, y + m_viewportHeight, 0                   },
        { x, y, 0                                      },
        { x + m_viewportWidth, y, 0                    },
        { x + m_viewportWidth, y + m_viewportHeight, 0 }
    };

    glGenBuffers(1, &m_fullscreenRectVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreenRectVertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(faderVertices),
                 faderVertices, GL_STATIC_DRAW);

    // Update the widget context
    m_widgetContext.m_viewportWidth = m_viewportWidth;
    m_widgetContext.m_viewportHeight = m_viewportHeight;
    m_widgetContext.m_fullscreenRectVertexBuffer = m_fullscreenRectVertexBuffer;
}

