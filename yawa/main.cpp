
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

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <random>
#include <sax/iostream.hpp>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace fs = std::filesystem;

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>

#include <fstream>
#include <string>
#include <vector>

#include <cereal/cereal.hpp>

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <cereal/archives/xml.hpp>
#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include "geo.hpp"
#include "globals.hpp"
#include "resource.h"
#include "weather.hpp"

#include <date/date.h>
#include <date/tz.h>

template<typename String>
std::vector<char> loadFile ( String const & filename ) {
    std::vector<char> file;
    if ( std::ifstream is{ filename, std::ios::binary | std::ios::ate } ) {
        file.resize ( static_cast<size_t> ( is.tellg ( ) ) );
        is.seekg ( 0 );
        is.read ( file.data ( ), file.size ( ) );
        file.push_back ( 0 );
    }
    return file;
}

// https://api.met.no/weatherapi/locationforecast/1.9/.json?lat=39.79&lon=19.81&msl=6

void init ( ) {
    if ( fs::exists ( g_places ) )
        load_places ( );
    if ( fs::exists ( g_keys ) )
        load_keys ( );
}

#include <nlohmann/json.hpp>

// for convenience.
using json = nlohmann::json;

#include <curlpp/cURLpp.hpp>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <SFML/Extensions.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <Thor/Graphics.hpp>

json forcast_query_apixu ( std::string const & name_, std::string const & country_ ) {
    json const forcast = query_url ( apixu_forcast_query_string ( place_data ( name_, country_ ).location ) );
    save_to_file ( forcast, "apixu_" + name_ + "_" + country_ );
    return forcast;
}

json forcast_query_darksky ( std::string const & name_, std::string const & country_ ) {
    json const forcast = query_url ( darksky_forcast_query_string ( place_data ( name_, country_ ).location ) );
    save_to_file ( forcast, "darksky_" + name_ + "_" + country_ );
    return forcast;
}

struct App {

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

    App ( ) {

        m_context_settings.antialiasingLevel = 8u;

        m_render_window.create ( sf::VideoMode ( 1200u, 150u ), L"", sf::Style::None, m_context_settings );
        m_render_window.setPosition ( { 360, 30 } );
        m_render_window.setVerticalSyncEnabled ( true );
        // m_render_window.requestFocus ( );
        m_render_window.setMouseCursorGrabbed ( false );
        m_render_window.setMouseCursorVisible ( true );
        // sf::makeWindowSeeThrough ( m_render_window );

        m_render_window_bounds = sf::FloatRect ( 0.0f, 0.0f, m_render_window.getSize ( ).x, m_render_window.getSize ( ).y );

        // Frames.

        m_frame_rate                     = sf::getScreenRefreshRate ( );
        m_frame_duration_as_microseconds = 1'000'000.0f / m_frame_rate;

        // Load fonts.

        sf::loadFromResource ( m_font_bold, __FONT_BOLD__ );
        sf::loadFromResource ( m_font_regular, __FONT_REGULAR__ );
        sf::loadFromResource ( m_font_mono, __FONT_MONO__ );

        // Load icons.

        construct_icons_map ( );

        m_render_window.clear ( sf::Color::Red );
        m_render_window.display ( );
    }

    void construct_icons_map ( ) {
        sf::Image icons;
        sf::loadFromResource ( icons, __ICONS__ );
        Icon::icon_size.x = static_cast<sf::Int32> ( icons.getSize ( ).x ) / descriptions_size ( );
        Icon::icon_size.y = static_cast<sf::Int32> ( icons.getSize ( ).y );
        sf::IntRect rect{
            0,
            0,
            Icon::icon_size.x,
            Icon::icon_size.y,
        };
        for ( int i = 0; i < descriptions_size ( ); ++i, rect.left += rect.width ) {
            Icon & icon = ( *m_icon_textures.emplace_hint ( std::end ( m_icon_textures ), std::string{ g_descriptions[ i ] }, Icon{} ) ).second;
            icon.texture.loadFromImage ( icons, rect );
            icon.texture.setSmooth ( true );
            icon.sprite.setTexture ( icon.texture );
        }
    }

    bool is_active ( ) const noexcept { return m_render_window.isOpen ( ); }

    void run ( ) noexcept {
        poll_events ( );
        // update_state ( );
        render_objects ( );
    }

    void poll_events ( ) noexcept {
        if ( m_render_window.pollEvent ( m_event ) ) {
            if ( sf::Event::Closed == m_event.type or
                 ( sf::Event::KeyPressed == m_event.type and sf::Keyboard::Escape == m_event.key.code ) ) {
                m_render_window.close ( );
            }
        }
    }

    void update_state ( ) noexcept {}

    void render_objects ( ) noexcept {
        m_render_window.clear ( sf::Color::Red );
        m_render_window.draw ( m_icon_textures [ "wi-alien" ].sprite );
        m_render_window.display ( );
    }
};

int main ( ) {

    /*

    init ( );
    curlpp::Cleanup myCleanup;

    json forcast_apixu{ forcast_query_apixu ( "acharavi", "greece" ) };
    std::cout << forcast_apixu.dump ( 4 ) << nl;

    json forcast_darksky{ forcast_query_darksky ( "acharavi", "greece" ) };
    std::cout << forcast_darksky.dump ( 4 ) << nl;

    json forcast_apixu{ load_from_file ( "apixu_acharavi_greece" ) };
    std::cout << forcast_apixu.dump ( 4 ) << nl;



    json forcast_darksky{ load_from_file ( "darksky_acharavi_greece" ) };
    std::cout << forcast_darksky.dump ( 4 ) << nl;

    std::cout << sf::systemTime ( ) << nl;
    */
    App app;

    while ( app.is_active ( ) ) {
        app.run ( );
    }

    return EXIT_SUCCESS;
}

int main684 ( ) {

    using namespace date;
    using namespace std::chrono;

    init ( );
    curlpp::Cleanup myCleanup;

    std::string s;

    load_from_file_bin ( s, "y:/tmp/", "darksky" );

    json const fc = json::parse ( s );

    std::cout << fc.dump ( 4 ) << nl;

    /*

    for ( auto const & f : fc[ "product" ][ "time" ] ) {
        if ( f[ "from" ] == f[ "to" ] ) {
            std::cout << f.dump ( ) << nl;
            // std::cout << f[ "from" ].get<std::string> ( ) << ' ' << f["location"]["temperature"]["value"].get<std::string> ( ) <<
    nl;
        }
    }

    */

    return EXIT_SUCCESS;
}

int main7868767 ( ) {

    init ( );
    curlpp::Cleanup myCleanup;

    std::stringstream ss;

    try {
        // Our request to be sent.
        curlpp::Easy request;

        // Set the URL.
        request.setOpt<curlpp::options::WriteStream> ( &ss );
        request.setOpt<curlpp::options::Encoding> ( "deflate" );

        request.setOpt<curlpp::options::Url> (
            "https://api.met.no/weatherapi/locationforecast/1.9/.json?lat=39.79&lon=19.81&msl=6" );

        // Send request and get a result.
        // By default the result goes to standard output.
        request.perform ( );
    }
    catch ( curlpp::RuntimeError & e ) {
        std::cout << e.what ( ) << std::endl;
    }
    catch ( curlpp::LogicError & e ) {
        std::cout << e.what ( ) << std::endl;
    }

    std::string s = ss.str ( );

    save_to_file_bin ( s, "y:/tmp/", "darksky" );

    return EXIT_SUCCESS;
}

/*
{
    "features" : [ {

        "geometry" : {

            "coordinates" : [ 34.9497949, 29.5569348 ],
            "type" : "Point"
        },

        "properties" : {
            "country" : "Israel",
            "extent" : [ 34.9226651, 29.5719897, 34.9724286, 29.5360916 ],
            "name" : "Eilat",
            "osm_id" : 563096441,
            "osm_key" : "place",
            "osm_type" : "W",
            "osm_value" : "city",
            "postcode" : "88112",
            "state" : "South District"
        },

        "type" : "Feature"
    } ],
    "type" : "FeatureCollection"
}
*/

#include <iostream>
#include <locale>

int main5 ( ) {
    std::locale::global ( std::locale ( "en_US" ) );
    std::wcout.imbue ( std::locale ( ) );
    std::wcout << "In US English UTF-8 locale:\n";
    auto & f         = std::use_facet<std::ctype<wchar_t>> ( std::locale ( ) );
    std::wstring str = L"grüßEN";
    // std::wstring str = L"HELLo, wORLD!";
    std::wcout << "Lowercase form of the string '" << str << "' is ";
    f.toupper ( &str[ 0 ], &str[ 0 ] + str.size ( ) );
    std::wcout << "'" << str << "'\n";
    return EXIT_SUCCESS;
}

int main655676 ( ) {

    auto & db = date::get_tzdb_list ( );

    return EXIT_SUCCESS;
}
