
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

/// Makes easier to expand the expressions
#define EXPAND( ... ) __VA_ARGS__

#define Q( x ) #x
#define QUOTE( x ) Q ( x )

struct DisplayDataCurrent {
    // darksky.
    std::string apparentTemperature, cloudCover, dewPoint, humidity, icon, nearestStormDistance, ozone, precipIntensity,
        precipProbability, pressure, summary, temperature, time, uvIndex, visibility, windBearing, windGust, windSpeed;
    bool is_day;
};

#define CONCAT( A1, A2, A3, A4 ) ( A1##A2##A3##A4 )

#define CONCAT2( A1, A2 ) ( A1##A2 )
#define CONCAT3( A1, A2, A3 ) ( A1##A2##A3 )

#define GET_DARKSY_CURRENTLY( PARAM ) j_.at ( QUOTE ( currently ) ).at ( QUOTE ( PARAM ) ).get_to ( d.PARAM )

//  CONCAT2 ( ., get_to ( CONCAT2 ( d., PARAM ) ) )

// .get_to ( d.##PARAM##)

// .get_to ( d.PARAM )

//  EXPAND ( j_.at ( \\"currently\\" ).at ( \\", PARAM, \\" ) ).get_to ( d., PARAM, ) )

inline void from_json ( json const & j_, DisplayDataCurrent & d ) { GET_DARKSY_CURRENTLY ( apparentTemperature ); }

int main ( ) { std::cout << QUOTE ( apparentTemperature ) << nl; }

struct DisplayDataHourly {
    // darksky.
    std::string apparentTemperature, cloudCover, dewPoint, humidity, icon, ozone, precipIntensity, precipProbability, pressure,
        summary, temperature, time, uvIndex, visibility, windBearing, windGust, windSpeed;
};

struct DisplayDataDaily {
    // darksky.
    std::string apparentTemperatureHigh, apparentTemperatureHighTime, apparentTemperatureLow, apparentTemperatureLowTime,
        apparentTemperatureMax, apparentTemperatureMaxTime, apparentTemperatureMin, apparentTemperatureMinTime, cloudCover,
        dewPoint, humidity, icon, moonPhase, ozone, precipIntensity, precipIntensityMax, precipIntensityMaxTime, precipProbability,
        precipType, pressure, summary, sunriseTime, sunsetTime, temperatureHigh, temperatureHighTime, temperatureLow,
        temperatureLowTime, temperatureMax, temperatureMaxTime, temperatureMin, temperatureMinTime, time, uvIndex, uvIndexTime,
        visibility, windBearing, windGust, windGustTime, windSpeed;
    std::array<DisplayDataHourly, 24> hourly_data;
    // apixu.
    std::string sunrise, sunset, moonrise, moonset;
};

struct DisplayData {
    DisplayDataCurrent current_data;
    std::array<DisplayDataDaily, 7> daily_data;
};

int main56867 ( ) {
    /*

    init ( );
    curlpp::Cleanup myCleanup;

    json forcast_apixu{ forcast_query_apixu ( "acharavi", "greece" ) };
    std::cout << forcast_apixu.dump ( g_indent ) << nl;

    json forcast_darksky{ forcast_query_darksky ( "acharavi", "greece" ) };
    std::cout << forcast_darksky.dump ( g_indent ) << nl;

    json forcast_apixu{ load_from_file ( "apixu_acharavi_greece" ) };
    std::cout << forcast_apixu.dump ( g_indent ) << nl;



    json forcast_darksky{ load_from_file ( "darksky_acharavi_greece" ) };
    std::cout << forcast_darksky.dump ( g_indent ) << nl;

    std::cout << sf::systemTime ( ) << nl;
    */
    App app;

    while ( app.is_active ( ) ) {
        app.run ( );
    }

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

int main655676 ( ) {
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
        GetWindowTextW ( hWnd_, wt.data ( ), wt.length ( ) );
#else
        std::string wt ( GetWindowTextLengthA ( hWnd_ ), 0 );
        GetWindowTextA ( hWnd_, wt.data ( ), wt.length ( ) );
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
