
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

std::string darksky_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.darksky.net/forecast/";
    constexpr char tag[] = "?units=si&extend=hourly";
    std::string s;
    s = { url + g_auth[ "darksky" ] + "/" + loc_.lat + "," + loc_.lng + tag };
    return s;
}

std::string apixu_forcast_query_string ( location_t const & loc_ ) {
    constexpr char url[] = "https://api.apixu.com/v1/forecast.json?key=";
    constexpr char tag[] = "&days=10";
    std::string s;
    s = { url + g_auth[ "apixu" ] + "&q=" + loc_.lat + "," + loc_.lng + tag };
    return s;
}

json forcast_query_apixu ( std::string const & name_, std::string const & country_ ) {
    auto const & pd    = place_data ( name_, country_ );
    json const forcast = query_url ( apixu_forcast_query_string ( pd.location ) );
    std::ofstream o ( g_app_data_path / ( "apixu_" + pd.place_country + ".json" ) );
    o << forcast.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
    return forcast;
}

json forcast_query_darksky ( std::string const & name_, std::string const & country_ ) {
    auto const & pd    = place_data ( name_, country_ );
    json const forcast = query_url ( darksky_forcast_query_string ( pd.location ) );
    std::ofstream o ( g_app_data_path / ( "darksky_" + pd.place_country + ".json" ) );
    o << forcast.dump ( g_indent ) << std::endl;
    o.flush ( );
    o.close ( );
    return forcast;
}

json forcast_apixu ( std::string const & name_, std::string const & country_ ) {
    auto const & pd = place_data ( name_, country_ );
    json forcast;
    std::ifstream i ( g_app_data_path / ( "apixu_" + pd.place_country + ".json" ) );
    i >> forcast;
    i.close ( );
    return forcast;
}

json forcast_darksky ( std::string const & name_, std::string const & country_ ) {
    auto const & pd = place_data ( name_, country_ );
    json forcast;
    std::ifstream i ( g_app_data_path / ( "darksky_" + pd.place_country + ".json" ) );
    i >> forcast;
    i.close ( );
    return forcast;
}

/*
json darksky ( std::string const & name_, std::string const & country_ ) {
    place_data ( name_, country_ )
        .

        fs::path const file = g_app_data_path / ( "apixu_" + name_ + "_" + country_ + ".json" );
    if ( fs::exists ( file ) ) {
        json forcast;
        std::ifstream i ( g_app_data_path / ( "darksky_" + name_ + "_" + country_ + ".json" ) );
        i >> forcast;
        i.close ( );
        return forcast;
    }
    else {
    }
}
*/
