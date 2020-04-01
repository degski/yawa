
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

#include <charconv>
#include <filesystem>
#include <fstream>
#include <limits>
#include <locale>
#include <sax/iostream.hpp>
#include <sax/utf8conv.hpp>
#include <sstream>
#include <string>
#include <string_view>

#include "globals.hpp"
#include "api.hpp"
#include "geo.hpp"

// Date has to be in format '2019-08-16'.
std::string ipgeolocation_astronomy_query_string ( location_t const & loc_, std::string const & date_ ) {
    constexpr char url[] = "https://api.ipgeolocation.io/astronomy?apiKey=";
    return { url + g_auth[ "ipgeolocation" ] + "&lat=" + loc_.lat + "&long=" + loc_.lng + "&date=" + date_ };
}

std::string ipgeolocation_timezone_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.ipgeolocation.io/timezone?apiKey=";
    return { url + g_auth[ "ipgeolocation" ] + "&lat=" + loc_.lat + "&long=" + loc_.lng };
}

std::string load_file ( std::wstring const & filename ) {
    std::string str;
    if ( std::ifstream is{ filename, std::ios::ate } ) {
        std::size_t const size = static_cast<std::size_t> ( is.tellg ( ) );
        str.resize ( size + 1 );
        is.seekg ( 0 );
        is.read ( str.data ( ), size ); // C++17 only.
        str.back ( ) = 0;
    }
    return str;
}

void save_geo ( ) {
    json const j = g_geo;
    std::ofstream o ( g_geo_path );
    o << j.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
}
void load_geo ( ) {
    json j;
    std::ifstream i ( g_geo_path );
    i >> j;
    i.close ( );
    g_geo = j.get<geo_t> ( );
}

void save_auth ( ) {
    json const j = g_auth;
    std::ofstream o ( g_auth_path );
    o << j.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
}
void load_auth ( ) {
    json j;
    std::ifstream i ( g_auth_path );
    i >> j;
    i.close ( );
    g_auth = j.get<auth_t> ( );
}

std::string to_query_string ( std::string const & str_ ) {
    // std::locale::global ( std::locale ( "en_US.UTF-8" ) );
    auto & facet = std::use_facet<std::ctype<std::string::value_type>> ( std::locale ( ) );
    std::string s{ str_ };
    facet.tolower ( s.data ( ), s.data ( ) + s.length ( ) );
    std::replace ( std::begin ( s ), std::end ( s ), ' ', '+' );
    return s;
}

std::string to_query_string ( std::string const & place_, std::string const & country_ ) {
    std::string s;
    s.reserve ( place_.length ( ) + 1 + country_.length ( ) );
    s.append ( to_query_string ( place_ ) );
    s.push_back ( '+' );
    s.append ( to_query_string ( country_ ) );
    return s;
}

std::string to_place_country_string ( std::string const & place_country_query_ ) {
    std::string s{ place_country_query_ };
    std::replace ( std::begin ( s ), std::end ( s ), '+', '_' );
    return s;
}

[[nodiscard]] std::string location_query_string ( std::string const & name_country_ ) {
    constexpr char url[] = "http://www.datasciencetoolkit.org/maps/api/geocode/json?sensor=false&address=";
    std::string s;
    s = { url + name_country_ };
    return s;
}

[[nodiscard]] std::string elevation_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "http://www.datasciencetoolkit.org/coordinates2statistics/";
    constexpr char tag[] = "?statistics=elevation";
    std::string s;
    s = { url + loc_.lat + "," + loc_.lng + tag };
    return s;
}

place_t const & place_data ( std::string const & place_, std::string const & country_ ) {
    static place_t const not_found{ { "not_found", "not_found" }, "not_found", "not_found", "not_found" };
    std::string place_country_query = to_query_string ( place_, country_ );
    auto it                         = g_geo.places.find ( place_country_query );
    if ( std::cend ( g_geo.places ) == it ) {
        json const location_query_result = query_url ( location_query_string ( place_country_query ) );
        if ( not std::strncmp ( "OK", location_query_result[ "status" ].get<std::string_view> ( ).data ( ), 2u ) ) {
            auto const location = location_query_result[ "results" ][ 0 ][ "geometry" ][ "location" ];
            place_t data{ { to_string ( location[ "lat" ].get<float> ( ) ), to_string ( location[ "lng" ].get<float> ( ) ) },
                          "",
                          place_,
                          country_,
                          to_place_country_string ( place_country_query ) };
            data.elevation =
                to_string ( query_url ( elevation_query_string ( data.location ) )[ 0 ][ "statistics" ][ "elevation" ][ "value" ]
                                .get<int> ( ) );
            place_t const & rv = g_geo.places.emplace ( std::move ( place_country_query ), std::move ( data ) ).first->second;
            save_geo ( );
            return rv;
        }
        else
            return not_found;
    }
    else
        return it->second;
}

json get_query_astro ( place_t const & pd_, fs::path const & file_, std::string const & date_ ) {
    json const a = query_url ( ipgeolocation_astronomy_query_string ( pd_.location, date_ ) );
    std::ofstream o ( file_ );
    o << a.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
    return a;
}

// Date in format '2019-08-18'.
void astro ( std::string const & date_ ) {
    auto const & current_place = g_geo.places[ g_geo.current ];
    fs::path const astro_file =
        g_app_data_path / ( L"astro_" + sax::utf8_to_utf16 ( current_place.place_country + '_' + date_ ) + L".json" );
    if ( fs::exists ( astro_file ) )
        g_astro = load ( astro_file );
    else
        g_astro = get_query_astro ( current_place, astro_file, date_ );
}
