
// MIT License
//
// Copyright (c) 2019 degski
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <string>

#include <SFML/Extensions.hpp>

#include "app.hpp"

App::App ( ) {

    init ( );

    m_context_settings.antialiasingLevel = 8u;

    m_render_window.create ( sf::VideoMode ( 1200u, 150u ), L"yawa", sf::Style::None, m_context_settings );
    m_render_window.setPosition ( { 360, 30 } );
    m_render_window.setFramerateLimit ( 6u );
    m_render_window.setActive ( false );
    m_render_window.setMouseCursorGrabbed ( false );
    m_render_window.setMouseCursorVisible ( true );

    sf::makeWindowSeeThrough ( m_render_window );
    sf::moveWindowBottom ( m_render_window );

    m_render_window_bounds = sf::FloatRect ( 0.0f, 0.0f, static_cast<float> ( m_render_window.getSize ( ).x ),
                                             static_cast<float> ( m_render_window.getSize ( ).y ) );

    // Frames.

    m_frame_rate                     = sf::getScreenRefreshRate ( );
    m_frame_duration_as_microseconds = 1'000'000.0f / m_frame_rate;

    // Load fonts.

    sf::loadFromResource ( m_font_bold, __FONT_BOLD__ );
    sf::loadFromResource ( m_font_regular, __FONT_REGULAR__ );
    sf::loadFromResource ( m_font_mono, __FONT_MONO__ );

    // Load icons.

    construct_icons_map ( );

    m_render_window.clear ( sf::Color::Transparent );
    m_render_window.display ( );
}

void App::construct_icons_map ( ) {
    sf::Image icons;
    sf::loadFromResource ( icons, __ICONS__ );
    Icon::icon_size.x = static_cast<sf::Int32> ( icons.getSize ( ).x );
    Icon::icon_size.y = static_cast<sf::Int32> ( icons.getSize ( ).y );
    // Change to white.
    for ( int y = 0; y < Icon::icon_size.y; ++y ) {
        for ( int x = 0; x < Icon::icon_size.x; ++x ) {
            auto alpha = icons.getPixel ( x, y ).a;
            if ( alpha )
                icons.setPixel ( x, y, sf::Color ( m_color.r, m_color.g, m_color.b, alpha ) );
        }
    }
    Icon::icon_size.x /= descriptions_size ( );
    sf::IntRect rect{
        0,
        0,
        Icon::icon_size.x,
        Icon::icon_size.y,
    };
    for ( int i = 0; i < descriptions_size ( ); ++i, rect.left += rect.width ) {
        Icon & icon =
            ( *m_icon_textures.emplace_hint ( std::end ( m_icon_textures ), std::string{ g_descriptions[ i ] }, Icon{} ) ).second;
        icon.texture.loadFromImage ( icons, rect );
        icon.texture.setSmooth ( true );
        icon.sprite.setTexture ( icon.texture );
    }
}
