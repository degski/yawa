
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

// for convenience
using json = nlohmann::json;

#include <curlpp/cURLpp.hpp>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

json forcast_apixu ( std::string const & name_, std::string const & country_ ) {
    json const forcast = query_url ( apixu_forcast_query_string ( place_data ( name_, country_ ).location ) );
    save_to_file ( forcast, "apixu_" + name_ + "_" + country_ );
    return forcast;
}

int main ( ) {

    init ( );
    curlpp::Cleanup myCleanup;

    std::string s;

    json const forcast  = query_url ( darksky_forcast_query_string ( place_data ( "Acharavi", "Greece" ).location ) );

    save_to_file ( forcast, "darksky" );

    json j;

    load_from_file ( j, "darksky" );

    std::cout << j.dump ( 4 ) << nl;


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
            // std::cout << f[ "from" ].get<std::string> ( ) << ' ' << f["location"]["temperature"]["value"].get<std::string> ( ) << nl;
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



int main655676 () {

    auto & db = date::get_tzdb_list ( );

    return EXIT_SUCCESS;

}