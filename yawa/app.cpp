
// MIT License
//
// Copyright (c) 2019, 2020 degski
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

#include <SFML/Extensions.hpp>

#include <globals.hpp>

#include "app.hpp"

#include "weather.hpp"

/*
    The shell creates a button on the taskbar whenever an application creates a window that isn't owned.
    To ensure that the window button is placed on the taskbar, create an unowned window with the WS_EX_APPWINDOW
    extended style. To prevent the window button from being placed on the taskbar, create the unowned window
    with the WS_EX_TOOLWINDOW extended style. As an alternative, you can create a hidden window and make this
    hidden window the owner of your visible window.
*/

#include <Windows.h>

//
//
// WndProc - Window procedure
//
//
LRESULT
CALLBACK
WndProc1 ( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) {

    switch ( uMsg ) {
        case WM_DESTROY: ::PostQuitMessage ( 0 ); break;
        default: return ::DefWindowProc ( hWnd, uMsg, wParam, lParam );
    }

    return 0;
}

HWND make_extended_window ( sf::VideoMode const & vm_ ) {

    HINSTANCE hInstance = ( HINSTANCE ) GetModuleHandle ( NULL );

    WNDCLASSEX wcex{ };

    wcex.cbSize        = sizeof ( wcex );                     // WNDCLASSEX size in bytes
    wcex.style         = CS_HREDRAW | CS_VREDRAW;             // Window class styles
    wcex.lpszClassName = TEXT ( "YAWAWINDOWCLASS" );          // Window class name
    wcex.hbrBackground = ( HBRUSH ) ( COLOR_WINDOW + 1 );     // Window background brush color.
    wcex.hCursor       = LoadCursor ( hInstance, IDC_ARROW ); // Window cursor
    wcex.lpfnWndProc   = WndProc1;                            // Window procedure associated to this window class.
    wcex.hInstance     = hInstance;                           // The application instance.

    // Register window and ensure registration success.
    if ( !RegisterClassEx ( &wcex ) )
        return nullptr;

    // Setup window initialization attributes.
    CREATESTRUCT cs{ };

    cs.cx        = vm_.width;             // Window width
    cs.cy        = vm_.height;            // Window height
    cs.hInstance = hInstance;             // Window instance.
    cs.lpszClass = wcex.lpszClassName;    // Window class name
    cs.lpszName  = TEXT ( "yawa" );       // Window title
    cs.style     = WS_VISIBLE | WS_POPUP; // Window style (sf::Style::None)
    cs.dwExStyle = WS_EX_TOOLWINDOW;      // Extended window style, no taskbar icon.

    // Create the window.
    return ::CreateWindowEx ( cs.dwExStyle, cs.lpszClass, cs.lpszName, cs.style, cs.x, cs.y, cs.cx, cs.cy, cs.hwndParent, cs.hMenu,
                              cs.hInstance, cs.lpCreateParams );
}

App::App ( ) {

    init ( );

    m_context_settings.antialiasingLevel = 8u;

    m_render_window.create ( make_extended_window ( sf::VideoMode ( 1200u, 150u ) ), m_context_settings );
    // m_render_window.create ( sf::VideoMode ( 1200u, 150u ), L"yawa", sf::Style::None, m_context_settings );

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

void App::init ( ) {
    if ( fs::exists ( g_geo_path ) )
        load_geo ( );
    if ( fs::exists ( g_auth_path ) )
        load_auth ( );
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
            ( *m_icon_textures.emplace_hint ( std::end ( m_icon_textures ), std::string{ g_descriptions[ i ] }, Icon{ } ) ).second;
        icon.texture.loadFromImage ( icons, rect );
        icon.texture.setSmooth ( true );
        icon.sprite.setTexture ( icon.texture );
    }
}

#ifndef FMT_USE_GRISU
#    define FMT_USE_GRISU 1
#endif

#include <fmt/chrono.h>
#include <fmt/compile.h>
#include <fmt/core.h>
#include <fmt/format.h>

[[nodiscard]] bool is_leap_year ( int const y_ ) noexcept { return ( ( y_ % 4 == 0 ) and ( y_ % 100 != 0 ) ) or ( y_ % 400 == 0 ); }

// Returns the number of days for the given m_ (month) in the given y_ (year).
[[nodiscard]] int days_month ( int const y_, int const m_ ) noexcept {
    return m_ != 2 ? 30 + ( ( m_ + ( m_ > 7 ) ) % 2 ) : 28 + is_leap_year ( y_ );
}

[[nodiscard]] int day_week ( int y_, int m_, int d_ ) noexcept {
    // calendar_system = 1 for Gregorian Calendar, 0 for Julian Calendar
    constexpr int const calendar_system = 1;
    if ( m_ < 3 )
        m_ += 12, y_ -= 1;
    return ( d_ + ( m_ << 1 ) + ( 6 * ( m_ + 1 ) / 10 ) + y_ + ( y_ >> 2 ) - ( y_ / 100 ) + ( y_ / 400 ) + calendar_system ) % 7;
}

[[nodiscard]] int first_weekday ( int const y_, int const m_ ) noexcept { return day_week ( y_, m_, 1 ); }

// Get the month day for the n_-th [ 1, 5 ] day_week w_.
[[nodiscard]] int weekday_day ( int const n_, int const y_, int const m_, int const w_, int const fwd_ ) noexcept {
    assert ( n_ >= 0 );
    assert ( n_ <= 5 );
    int const day = 1 + ( 7 - fwd_ + w_ ) % 7 + ( n_ - 1 ) * 7;
    return n_ < 5 ? day : day > days_month ( y_, m_ ) ? day - 7 : day;
}

[[nodiscard]] int year_weeks ( int const y_, int const m_, int const d_, int const fwd_ ) noexcept { // normal counting.
    assert ( d_ <= days_month ( y_, m_ ) );
    constexpr short const cum_dim[ 12 ] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 };
    return ( ( ( cum_dim[ m_ - 1 ] + d_ + ( ( m_ > 2 ) * ( ( ( y_ % 4 == 0 ) and ( y_ % 100 != 0 ) ) or ( y_ % 400 == 0 ) ) ) ) -
               weekday_day ( 0, y_, 1, 0, fwd_ ) ) /
             7 ) +
           1;
}

[[nodiscard]] std::string calendar ( int const y_, int const m_ ) noexcept {
    constexpr char const * const month_of_the_year[ 12 ] = {
        "January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"
    };
    std::string s;
    s.reserve ( 192 );
    s.append ( ( 20 - std::strlen ( month_of_the_year[ m_ - 1 ] ) ) / 2, ' ' );
    s.append ( fmt::format ( "{} {}\n  # Su Mo Tu We Th Fr Sa\n", month_of_the_year[ m_ - 1 ], y_ ) );
    int f = first_weekday ( y_, m_ ), w = year_weeks ( y_, m_, 1, f );
    // first line.
    s.append ( fmt::format ( "{:3}", w++ ) );
    s.append ( 3 * f, ' ' );
    int c = 1;
    for ( ; f < 7; ++f )
        s.append ( fmt::format ( "{:3}", c++ ) );
    s.push_back ( '\n' );
    int const l = days_month ( y_, m_ );
    // middle lines.
    while ( c <= ( l - 7 ) ) {
        s.append ( fmt::format ( "{:3}", w++ ) );
        for ( int i = 0; i < 7; ++i, ++c )
            s.append ( fmt::format ( "{:3}", c ) );
        s.push_back ( '\n' );
    }
    // last line (iff applicable).
    if ( c <= l ) {
        s.append ( fmt::format ( "{:3}", w ) );
        do {
            s.append ( fmt::format ( "{:3}", c++ ) );
        } while ( c <= l );
        s.push_back ( '\n' );
    }
    return s;
}
