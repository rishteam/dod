#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System/Clock.hpp>
//
#include "Rish/Core/Core.h"
#include "Rish/Platform/SFMLWindow.h"

#include "Rish/Events/ApplicationEvent.h"
#include "Rish/Events/KeyEvent.h"
#include "Rish/Events/MouseEvent.h"

namespace rl {

namespace {

ImTextureID convertGLTextureHandleToImTextureID(GLuint glTextureHandle)
{
    ImTextureID textureID = (ImTextureID)NULL;
    std::memcpy(&textureID, &glTextureHandle, sizeof(GLuint));
    return textureID;
}
GLuint convertImTextureIDToGLTextureHandle(ImTextureID textureID)
{
    GLuint glTextureHandle;
    std::memcpy(&glTextureHandle, &textureID, sizeof(GLuint));
    return glTextureHandle;
}

// Rendering callback
void RenderDrawLists(ImDrawData* draw_data) {
    ImGui::GetDrawData();
    if (draw_data->CmdListsCount == 0) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    assert(io.Fonts->TexID !=
           (ImTextureID)NULL);  // You forgot to create and set font texture

    // scale stuff (needed for proper handling of window resize)
    int fb_width =
        static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
    int fb_height =
        static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
    if (fb_width == 0 || fb_height == 0) {
        return;
    }
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

#ifdef GL_VERSION_ES_CL_1_1
    GLint last_program, last_texture, last_array_buffer,
        last_element_array_buffer;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
#else
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
#endif

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

#ifdef GL_VERSION_ES_CL_1_1
    glOrthof(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
#else
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
#endif

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    for (int n = 0; n < draw_data->CmdListsCount; ++n) {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const unsigned char* vtx_buffer =
            (const unsigned char*)&cmd_list->VtxBuffer.front();
        const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();

        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert),
                        (void*)(vtx_buffer + offsetof(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert),
                          (void*)(vtx_buffer + offsetof(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert),
                       (void*)(vtx_buffer + offsetof(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); ++cmd_i) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            } else {
                GLuint textureHandle =
                    convertImTextureIDToGLTextureHandle(pcmd->TextureId);
                glBindTexture(GL_TEXTURE_2D, textureHandle);
                glScissor((int)pcmd->ClipRect.x,
                          (int)(fb_height - pcmd->ClipRect.w),
                          (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                          (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                               GL_UNSIGNED_SHORT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
#ifdef GL_VERSION_ES_CL_1_1
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glDisable(GL_SCISSOR_TEST);
#else
    glPopAttrib();
#endif
}

// clipboard
std::string s_clipboardText;
void setClipboardText(void * /*userData*/, const char *text)
{
    sf::Clipboard::setString(sf::String::fromUtf8(text, text + std::strlen(text)));
}
const char *getClipboardText(void * /*userData*/)
{
    std::basic_string<sf::Uint8> tmp = sf::Clipboard::getString().toUtf8();
    s_clipboardText = std::string(tmp.begin(), tmp.end());
    return s_clipboardText.c_str();
}

bool s_mousePressed[5] = {false};
// mouse cursors
bool s_mouseCursorLoaded[ImGuiMouseCursor_COUNT];
sf::Cursor *s_mouseCursors[ImGuiMouseCursor_COUNT];
void loadMouseCursor(ImGuiMouseCursor imguiCursorType, sf::Cursor::Type sfmlCursorType)
{
    s_mouseCursors[imguiCursorType] = new sf::Cursor();
    s_mouseCursorLoaded[imguiCursorType] =
        s_mouseCursors[imguiCursorType]->loadFromSystem(sfmlCursorType);
}
void updateMouseCursor(sf::Window &window)
{
    ImGuiIO &io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
    {
        ImGuiMouseCursor cursor = ImGui::GetMouseCursor();
        if (io.MouseDrawCursor || cursor == ImGuiMouseCursor_None)
        {
            window.setMouseCursorVisible(false);
        }
        else
        {
            window.setMouseCursorVisible(true);

            sf::Cursor &c = s_mouseCursorLoaded[cursor]
                                ? *s_mouseCursors[cursor]
                                : *s_mouseCursors[ImGuiMouseCursor_Arrow];
            window.setMouseCursor(c);
        }
    }
}

// Font
sf::Texture *s_fontTexture=nullptr;
// owning pointer to internal font atlas which is used if user
// doesn't set custom sf::Texture.
void UpdateFontTexture()
{
    ImGuiIO &io = ImGui::GetIO();
    unsigned char *pixels;
    int width, height;

    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

    sf::Texture &texture = *s_fontTexture;
    texture.create(width, height);
    texture.update(pixels);

    io.Fonts->TexID =
        convertGLTextureHandleToImTextureID(texture.getNativeHandle());
}
sf::Texture &GetFontTexture() { return *s_fontTexture; }

bool s_windowHasFocus = false;
} // end namespace

#define SFML_EVENT_CALLBACK(x) SFMLEventDispatcher::SFMLEventCallback x

/**
 * @brief Window create function for SFMLWindow
 *
 * @param title Title
 * @param width Width
 * @param height Height
 * @return Window* Pointer to allocated SFMLWindow
 */
Window* Window::Create(const std::string &title, uint32_t width, uint32_t height)
{
    return new SFMLWindow(title, width, height);
}

SFMLWindow::SFMLWindow(const std::string &title, const uint32_t width, const uint32_t height)
    : Window(title, width, height),
    m_SFMLWindow(sf::VideoMode(width, height), title)
{
    // ImGui::SFML::Init(m_SFMLWindow);

    // Add SFML Event mapping
    SFML_EVENT_CALLBACK(closeEvent) = [&](const sf::Event &e) {
        RL_UNUSED(e);
        WindowCloseEvent windowClose;
        m_eventCallback(windowClose);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Closed, closeEvent);

    // Resize event
    SFML_EVENT_CALLBACK(resizeEvent) = [&](const sf::Event &e) {
        WindowResizeEvent resize(e.size.width, e.size.height);
        m_eventCallback(resize);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::Resized, resizeEvent);

    // key pressed event
    SFML_EVENT_CALLBACK(keyPressedEvent) = [&](const sf::Event &e) {
        KeyPressedEvent keyPress(e.key.code, false);
        RL_CORE_TRACE("{}", keyPress.toString());
        m_eventCallback(keyPress);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::KeyPressed, keyPressedEvent);

    // key released event
    SFML_EVENT_CALLBACK(keyReleasedEvent) = [&](const sf::Event &e) {
        KeyReleasedEvent keyRelease(e.key.code);
        m_eventCallback(keyRelease);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::KeyReleased, keyReleasedEvent);

    // key typed event
    SFML_EVENT_CALLBACK(keyTypedEvent) = [&](const sf::Event &e) {
        if(e.text.unicode < ' ' || e.text.unicode == 127)
            return;
        KeyTypedEvent keyTyped(e.text.unicode);
        m_eventCallback(keyTyped);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::TextEntered, keyTypedEvent);

    // mouse moved event
    SFML_EVENT_CALLBACK(mouseMovedEvent) = [&](const sf::Event &e) {
        MouseMovedEvent m(static_cast<float>(e.mouseMove.x), static_cast<float>(e.mouseMove.y));
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseMoved, mouseMovedEvent);

    // mouse scrolled event
    SFML_EVENT_CALLBACK(mouseScrolledEvent) = [&](const sf::Event &e) {
        MouseScrolledEvent mc;
        // position
        mc.x = static_cast<float>(e.mouseWheelScroll.x);
        mc.y = static_cast<float>(e.mouseWheelScroll.y);
        // wheel direction
        if(e.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
            mc.yOffset = e.mouseWheelScroll.delta;
        else if(e.mouseWheelScroll.wheel == sf::Mouse::HorizontalWheel)
            mc.xOffset = e.mouseWheelScroll.delta;
        m_eventCallback(mc);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseWheelScrolled, mouseScrolledEvent);

    // mouse button pressed
    SFML_EVENT_CALLBACK(mouseButtonPressedEvent) = [&](const sf::Event &e) {
        MouseButtonPressedEvent m(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseButtonPressed, mouseButtonPressedEvent);

    // mouse button released
    SFML_EVENT_CALLBACK(mouseButtonReleaseEvent) = [&](const sf::Event &e) {
        MouseButtonReleasedEvent m(e.mouseButton.button, e.mouseButton.x, e.mouseButton.y);
        m_eventCallback(m);
    };
    m_SFMLEventDispatcher.addListener(sf::Event::MouseButtonReleased, mouseButtonReleaseEvent);
}

SFMLWindow::~SFMLWindow()
{
}

void SFMLWindow::onUpdate()
{
    RL_CORE_ASSERT(m_eventCallback, "Event Callback is not ready");

    m_SFMLWindow.clear(m_clearColor);
    //
    sf::Event event;
    while(m_SFMLWindow.pollEvent(event))
    {
        // ImGui::SFML::ProcessEvent(event);
        m_SFMLEventDispatcher.handleEvent(event);
    }
    // ImGui::SFML::Update(m_SFMLWindow, m_clock.restart());
}

void SFMLWindow::onDraw()
{
    // m_SFMLWindow.pushGLStates();
    // ImGui::SFML::Render(m_SFMLWindow);
    // m_SFMLWindow.popGLStates();
    //
    m_SFMLWindow.display();
}

// Adapt from imgui-SFML
void SFMLWindow::initImGui()
{
    // ImGui::SFML::Init(m_SFMLWindow);
    ImGuiContext *ctx = ImGui::CreateContext();
    ImGui::SetCurrentContext(ctx);
    ImGuiIO &io = ImGui::GetIO();

    // tell ImGui which features we support
    // io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.BackendPlatformName = "imgui_impl_sfml";

    // init keyboard mapping
    io.KeyMap[ImGuiKey_Tab] = sf::Keyboard::Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = sf::Keyboard::Left;
    io.KeyMap[ImGuiKey_RightArrow] = sf::Keyboard::Right;
    io.KeyMap[ImGuiKey_UpArrow] = sf::Keyboard::Up;
    io.KeyMap[ImGuiKey_DownArrow] = sf::Keyboard::Down;
    io.KeyMap[ImGuiKey_PageUp] = sf::Keyboard::PageUp;
    io.KeyMap[ImGuiKey_PageDown] = sf::Keyboard::PageDown;
    io.KeyMap[ImGuiKey_Home] = sf::Keyboard::Home;
    io.KeyMap[ImGuiKey_End] = sf::Keyboard::End;
    io.KeyMap[ImGuiKey_Insert] = sf::Keyboard::Insert;
    io.KeyMap[ImGuiKey_Delete] = sf::Keyboard::Delete;
    io.KeyMap[ImGuiKey_Backspace] = sf::Keyboard::BackSpace;
    io.KeyMap[ImGuiKey_Space] = sf::Keyboard::Space;
    io.KeyMap[ImGuiKey_Enter] = sf::Keyboard::Return;
    io.KeyMap[ImGuiKey_Escape] = sf::Keyboard::Escape;
    io.KeyMap[ImGuiKey_A] = sf::Keyboard::A;
    io.KeyMap[ImGuiKey_C] = sf::Keyboard::C;
    io.KeyMap[ImGuiKey_V] = sf::Keyboard::V;
    io.KeyMap[ImGuiKey_X] = sf::Keyboard::X;
    io.KeyMap[ImGuiKey_Y] = sf::Keyboard::Y;
    io.KeyMap[ImGuiKey_Z] = sf::Keyboard::Z;

    // Set window size
    auto size = m_SFMLWindow.getSize();
    io.DisplaySize = ImVec2(size.x, size.y);

    // clipboard
    io.SetClipboardTextFn = setClipboardText;
    io.GetClipboardTextFn = getClipboardText;

    // load mouse cursors
    for (int i = 0; i < ImGuiMouseCursor_COUNT; ++i)
        s_mouseCursorLoaded[i] = false;

    loadMouseCursor(ImGuiMouseCursor_Arrow, sf::Cursor::Arrow);
    loadMouseCursor(ImGuiMouseCursor_TextInput, sf::Cursor::Text);
    loadMouseCursor(ImGuiMouseCursor_ResizeAll, sf::Cursor::SizeAll);
    loadMouseCursor(ImGuiMouseCursor_ResizeNS, sf::Cursor::SizeVertical);
    loadMouseCursor(ImGuiMouseCursor_ResizeEW, sf::Cursor::SizeHorizontal);
    loadMouseCursor(ImGuiMouseCursor_ResizeNESW, sf::Cursor::SizeBottomLeftTopRight);
    loadMouseCursor(ImGuiMouseCursor_ResizeNWSE, sf::Cursor::SizeTopLeftBottomRight);
    loadMouseCursor(ImGuiMouseCursor_Hand, sf::Cursor::Hand);

    // delete previously created texture
    if (s_fontTexture)
        delete s_fontTexture;
    s_fontTexture = new sf::Texture;

    if (m_loadDefaultFont)
    {
        // this will load default font automatically
        // No need to call AddDefaultFont
        UpdateFontTexture();
    }

    s_windowHasFocus = m_SFMLWindow.hasFocus();
}

void SFMLWindow::shutdownImGui()
{
    // ImGui::SFML::Shutdown();
    ImGui::GetIO().Fonts->TexID = (ImTextureID)NULL;

    // if internal texture was created, we delete it
    if (s_fontTexture)
    {
        delete s_fontTexture;
        s_fontTexture = nullptr;
    }

    for (int i = 0; i < ImGuiMouseCursor_COUNT; ++i)
    {
        if (s_mouseCursorLoaded[i])
        {
            delete s_mouseCursors[i];
            s_mouseCursors[i] = nullptr;

            s_mouseCursorLoaded[i] = false;
        }
    }

    ImGui::DestroyContext();
}

void SFMLWindow::updateImGui()
{
    // ImGui::SFML::Update(m_SFMLWindow, m_clock.restart());

    // Update OS/hardware mouse cursor if imgui isn't drawing a software cursor
    updateMouseCursor(m_SFMLWindow);

    // if (!s_mouseMoved)
    // {
    //     if (sf::Touch::isDown(0))
    //         s_touchPos = sf::Touch::getPosition(0, window);

    //     Update(s_touchPos, static_cast<sf::Vector2f>(target.getSize()), dt);
    // }
    // else
    // {
    //     Update(sf::Mouse::getPosition(window),
    //            static_cast<sf::Vector2f>(target.getSize()), dt);
    // }

    // Hide OS mouse cursor if imgui is drawing it
    if (ImGui::GetIO().MouseDrawCursor)
        m_SFMLWindow.setMouseCursorVisible(false);

    ImGuiIO &io = ImGui::GetIO();
    // Set window size
    auto size = m_SFMLWindow.getSize();
    io.DisplaySize = ImVec2(size.x, size.y);
    // Set time
    sf::Time dt = m_clock.restart();
    io.DeltaTime = dt.asSeconds();

    if (s_windowHasFocus)
    {
        if (io.WantSetMousePos)
        {
            sf::Vector2i mousePos(static_cast<int>(io.MousePos.x),
                                  static_cast<int>(io.MousePos.y));
            sf::Mouse::setPosition(mousePos);
        }
        // else
        // {
        //     io.MousePos = mousePos;
        // }
        for (unsigned int i = 0; i < 5; i++)
        {
            io.MouseDown[i] = /* s_touchDown[i] || sf::Touch::isDown(i) || */
                              s_mousePressed[i] ||
                              sf::Mouse::isButtonPressed((sf::Mouse::Button)i);
            s_mousePressed[i] = false;
            // s_touchDown[i] = false;
        }
    }

    // Update Ctrl, Shift, Alt, Super state
    io.KeyCtrl = io.KeysDown[sf::Keyboard::LControl] ||
                 io.KeysDown[sf::Keyboard::RControl];
    io.KeyAlt =
        io.KeysDown[sf::Keyboard::LAlt] || io.KeysDown[sf::Keyboard::RAlt];
    io.KeyShift =
        io.KeysDown[sf::Keyboard::LShift] || io.KeysDown[sf::Keyboard::RShift];
    io.KeySuper = io.KeysDown[sf::Keyboard::LSystem] ||
                  io.KeysDown[sf::Keyboard::RSystem];

    assert(io.Fonts->Fonts.Size > 0); // You forgot to create and set up font
                                      // atlas (see createFontTexture)

    ImGui::NewFrame();
}

void SFMLWindow::renderImGui()
{
    ImGui::EndFrame();
    //
    m_SFMLWindow.pushGLStates();
        m_SFMLWindow.resetGLStates();
        ImGui::Render();
        RenderDrawLists(ImGui::GetDrawData());
    m_SFMLWindow.popGLStates();
}
}
