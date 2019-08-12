
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
#include <string_view>
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

#include "api.hpp"
#include "app.hpp"
#include "geo.hpp"
#include "globals.hpp"
#include "resource.h"
#include "weather.hpp"

#include <date/date.h>
#include <date/tz.h>

#include <nlohmann/json.hpp>

// for convenience.
using json = nlohmann::json;

#include <curlpp/cURLpp.hpp>

#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

// Everything in local time.

void handle_eptr ( std::exception_ptr eptr ) { // Passing by value is ok.
    try {
        if ( eptr )
            std::rethrow_exception ( eptr );
    }
    catch ( const std::exception & e ) {
        std::cout << "Caught exception \"" << e.what ( ) << "\"\n";
    }
}

[[nodiscard]] bool isEscapePressed ( const sf::Event & event_ ) noexcept {
    return sf::Event::KeyPressed == event_.type and sf::Keyboard::Escape == event_.key.code;
}

int main7978 ( ) {

    std::exception_ptr eptr;

    App app;

    try {
        // json fa = forcast_load_apixu ( "Denver", "CO United States" );
        // std::cout << fa.dump ( g_indent ) << nl;

        json fd = forcast_load_darksky ( "Los Angeles", "United States" );
        std::cout << fd.dump ( g_indent ) << nl;
        json fa = forcast_load_apixu ( "Los Angeles", "United States" );
        std::cout << fa.dump ( g_indent ) << nl;

        data.darksky = fd;
        data.apixu   = fa;

        std::cout << data.darksky.hourly[ 0 ].humidity << nl;
        std::cout << data.darksky.time.timezone << nl;
        std::cout << data.darksky.current.apparentTemperature << nl;

        // std::cout << ddad.daily[ 2 ].sunrise << nl;

        /*
        while ( app.is_active ( ) ) {
            app.run ( );
        }
        */
    }
    catch ( ... ) {
        eptr = std::current_exception ( ); // Capture.
    }
    handle_eptr ( eptr );

    return EXIT_SUCCESS;
}

int main684 ( ) { return EXIT_SUCCESS; }

int main65756 ( ) { return EXIT_SUCCESS; }

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

int main6567 ( ) {

    auto & db = date::get_tzdb_list ( );

    return EXIT_SUCCESS;
}

// https://stackoverflow.com/questions/916259/win32-bring-a-window-to-top

// IsWindowVisible(hWnd)

class LastWindow {
    static inline HWND s_last_before_progman;

    static auto window_title ( HWND hWnd_ ) noexcept {
#if GetWindowTextLength == GetWindowTextLengthW
        std::wstring wt ( GetWindowTextLengthW ( hWnd_ ), 0 );
        GetWindowTextW ( hWnd_, wt.data ( ), ( int ) wt.length ( ) );
#else
        std::string wt ( GetWindowTextLengthA ( hWnd_ ), 0 );
        GetWindowTextA ( hWnd_, wt.data ( ), ( int ) wt.length ( ) );
#endif
        return wt;
    }

    static BOOL CALLBACK enum_window_callback ( HWND hWnd_, LPARAM lparam_ ) noexcept {
        if ( TEXT ( "Program Manager" ) != window_title ( hWnd_ ) ) {
            s_last_before_progman = hWnd_;
            return TRUE;
        }
        else {
            return FALSE;
        }
    }

    public:
    static HWND get ( ) noexcept {
        s_last_before_progman = nullptr;
        EnumWindows ( enum_window_callback, NULL );
        return s_last_before_progman;
    }
};

int main56868 ( ) {
    std::cout << LastWindow::get ( ) << std::endl;

    return EXIT_SUCCESS;
}

#include <cpprest/filestream.h>
#include <cpprest/http_client.h>

using namespace utility;              // Common utilities like string conversions
using namespace web;                  // Common features like URIs.
using namespace web::http;            // Common HTTP functionality
using namespace web::http::client;    // HTTP client features
using namespace concurrency::streams; // Asynchronous streams

int main ( int argc, char * argv[] ) {
    auto fileStream = std::make_shared<ostream> ( );

    // Open stream to output file.
    pplx::task<void> requestTask = fstream::open_ostream ( U ( "results.html" ) )
                                       .then ( [=] ( ostream outFile ) {
                                           *fileStream = outFile;

                                           // Create http_client to send the request.
                                           http_client client ( U ( "http://www.bing.com/" ) );

                                           // Build request URI and start the request.
                                           uri_builder builder ( U ( "/search" ) );
                                           builder.append_query ( U ( "q" ), U ( "cpprestsdk github" ) );
                                           return client.request ( methods::GET, builder.to_string ( ) );
                                       } )

                                       // Handle response headers arriving.
                                       .then ( [=] ( http_response response ) {
                                           printf ( "Received response status code:%u\n", response.status_code ( ) );

                                           // Write response body into the file.
                                           return response.body ( ).read_to_end ( fileStream->streambuf ( ) );
                                       } )

                                       // Close the file stream.
                                       .then ( [=] ( size_t ) { return fileStream->close ( ); } );

    // Wait for all the outstanding I/O to complete and handle any exceptions
    try {
        requestTask.wait ( );
    }
    catch ( const std::exception & e ) {
        printf ( "Error exception:%s\n", e.what ( ) );
    }

    return 0;
}
