
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

#pragma once

#include <map>

#include <SFML/Graphics.hpp>

#include <curlpp/cURLpp.hpp>

#include "resource.h"

struct App {

    curlpp::Cleanup cleanup;

    // Draw stuff.

    sf::ContextSettings m_context_settings;
    sf::RenderWindow m_render_window;
    sf::FloatRect m_render_window_bounds;

    // Frames.

    sf::Int32 m_frame_rate;
    float m_frame_duration_as_microseconds;

    // Resources.

    sf::Font m_font_bold, m_font_regular, m_font_mono;

    struct Icon {
        sf::Texture texture;
        sf::Sprite sprite;
        static inline sf::Vector2i icon_size;
    };

    std::map<std::string, Icon> m_icon_textures;

    sf::Event m_event;

    sf::Color const m_color = sf::Color ( 108, 112, 95, 255 );

    App ( );

    static void init ( );
    void construct_icons_map ( );

    inline bool is_active ( ) const noexcept { return m_render_window.isOpen ( ); }

    inline void run ( ) noexcept {
        poll_events ( );
        // update_state ( );
        render_objects ( );
    }

    inline void poll_events ( ) noexcept {
        if ( m_render_window.pollEvent ( m_event ) ) {
            if ( sf::Event::Closed == m_event.type or
                 ( sf::Event::KeyPressed == m_event.type and sf::Keyboard::Escape == m_event.key.code ) ) {
                m_render_window.close ( );
            }
        }
    }

    inline void update_state ( ) noexcept {}

    inline void render_objects ( ) noexcept {
        m_render_window.clear ( sf::Color::Transparent );
        auto & s = m_icon_textures[ "day-sunny" ].sprite;
        m_render_window.draw ( s );
        m_render_window.display ( );
    }
};
