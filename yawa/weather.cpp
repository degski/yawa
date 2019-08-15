
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

#include "globals.hpp"

std::string darksky_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.darksky.net/forecast/";
    constexpr char tag[] = "?units=ca&extend=hourly";
    return { url + g_auth[ "darksky" ] + "/" + loc_.lat + "," + loc_.lng + tag };
}

std::string apixu_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.apixu.com/v1/forecast.json?key=";
    constexpr char tag[] = "&days=10";
    return { url + g_auth[ "apixu" ] + "&q=" + loc_.lat + "," + loc_.lng + tag };
}

json forcast_query_apixu ( place_t const & pd_, fs::path const & file_ ) {
    json const forcast = query_url ( apixu_forcast_query_string ( pd_.location ) );
    std::ofstream o ( file_ );
    o << forcast.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
    return forcast;
}

json forcast_query_darksky ( place_t const & pd_, fs::path const & file_ ) {
    json const forcast = query_url ( darksky_forcast_query_string ( pd_.location ) );
    std::ofstream o ( file_ );
    o << forcast.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
    return forcast;
}

json forcast_load_apixu ( fs::path const & file_ ) {
    json forcast;
    std::ifstream i ( file_ );
    i >> forcast;
    i.close ( );
    return forcast;
}

json forcast_load_darksky ( fs::path const & file_ ) {
    json forcast;
    std::ifstream i ( file_ );
    i >> forcast;
    i.close ( );
    return forcast;
}

void forcast ( ) {
    auto const & current_place = g_geo.places[ g_geo.current ];
    // darksky.
    fs::path const darksky_file = g_app_data_path / ( "darksky_" + current_place.place_country + ".json" );
    if ( not g_data.darksky.update_time ) {
        if ( fs::exists ( darksky_file ) )
            g_data.darksky = forcast_load_darksky ( darksky_file );
        else
            g_data.darksky = forcast_query_darksky ( current_place, darksky_file );
    }
    if ( g_data.darksky.is_stale ( ) )
        g_data.darksky = forcast_query_darksky ( current_place, darksky_file );
    // apixu.
    fs::path const apixu_file = g_app_data_path / ( "apixu_" + current_place.place_country + ".json" );
    if ( not g_data.apixu.update_time ) {
        if ( fs::exists ( apixu_file ) )
            g_data.apixu = forcast_load_apixu ( apixu_file );
        else
            g_data.apixu = forcast_query_apixu ( current_place, apixu_file );
    }
    g_data.normalize_times ( );
    if ( g_data.apixu.is_stale ( ) ) {
        g_data.apixu = forcast_query_apixu ( current_place, apixu_file );
        // normalize apixu times to UTC.
        g_data.normalize_times ( );
    }
}
