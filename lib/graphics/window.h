#pragma once

#include <SDL2/SDL.h>

class Window
{
public:
    // Construct a Window object. This does not create a window as
    // create() must be called later.
    explicit Window();

    // Destroy any open windows.
    ~Window();

public:
    // Return the window width.
    int width() const;

    // Return the window height.
    int height() const;

    // Create the window.
    bool create(const char* title);

    // Return the window renderer context.
    SDL_Renderer* renderer() const;

private:
    SDL_Window*   m_window;
    SDL_Renderer* m_renderer;
    int           m_width;
    int           m_height;
};
