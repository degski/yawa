
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

#include <filesystem>
#include <fstream>
#include <limits>
#include <sax/iostream.hpp>
#include <sstream>
#include <string>

#include "geo.hpp"

std::string load_file ( std::string const & filename ) {
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

void save_places ( ) {
    json const j = g_geo;
    std::ofstream o ( g_places );
    o << j.dump ( 4 ) << std::endl;
    o.flush ( );
    o.close ( );
}
void load_places ( ) {
    json j;
    std::ifstream i ( g_places );
    i >> j;
    i.close ( );
    g_geo = j.get<geo_t> ( );
}

void save_keys ( ) {
    json const j = g_auth;
    std::ofstream o ( g_keys );
    o << j.dump ( 4 ) << std::endl;
    o.flush ( );
    o.close ( );
}
void load_keys ( ) {
    json j;
    std::ifstream i ( g_keys );
    i >> j;
    i.close ( );
    g_auth = j.get<auth_t> ( );
}

std::string to_query_string ( std::string const & str_ ) {
    std::string s{ str_ };
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

[[nodiscard]] std::string darksky_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.darksky.net/forecast/";
    constexpr char tag[] = "?units=si&extend=hourly";
    std::string s;
    s = { url + g_auth[ "darksky" ] + "/" + loc_.lat + "," + loc_.lng + tag };
    return s;
}

[[nodiscard]] std::string apixu_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.apixu.com/v1/forecast.json?key=";
    constexpr char tag[] = "&days=10";
    std::string s;
    s = { url + g_auth[ "apixu" ] + "&q=" + loc_.lat + "," + loc_.lng + tag };
    return s;
}

place_t const & place_data ( std::string const & place_, std::string const & country_ ) {
    static place_t const not_found{ { "not_found", "not_found" }, "not_found", "not_found", "not_found" };
    std::string place_country = to_query_string ( place_, country_ );
    auto it                   = g_geo.places.find ( place_country );
    if ( std::cend ( g_geo.places ) == it ) {
        json const location_query_result = query_url ( location_query_string ( place_country ) );
        if ( "OK" == location_query_result[ "status" ] ) {
            auto const location = location_query_result[ "results" ][ 0 ][ "geometry" ][ "location" ];
            place_t data{ { to_string ( location[ "lat" ].get<float> ( ) ), to_string ( location[ "lng" ].get<float> ( ) ) },
                          "",
                          place_,
                          country_ };
            data.elevation =
                to_string ( query_url ( elevation_query_string ( data.location ) )[ 0 ][ "statistics" ][ "elevation" ][ "value" ]
                                .get<int> ( ) );
            place_t const & rv = g_geo.places.emplace ( std::move ( place_country ), std::move ( data ) ).first->second;
            save_places ( );
            return rv;
        }
        else
            return not_found;
    }
    else
        return it->second;
}
