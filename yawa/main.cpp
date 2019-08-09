
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

#define Q_( x ) #x
#define QUOTE( x ) Q_ ( x )

#if _DEBUG
#    define GET_DATA( PARAM )                                                                                                      \
        if ( f.count ( QUOTE ( PARAM ) ) ) {                                                                                       \
            f.at ( QUOTE ( PARAM ) ).get_to ( t.PARAM );                                                                           \
        }                                                                                                                          \
        else {                                                                                                                     \
            t.PARAM = {};                                                                                                          \
            std::cout << "not found " QUOTE ( PARAM ) << nl;                                                                       \
        }
#else
#    define GET_DATA( PARAM )                                                                                                      \
        if ( f.count ( QUOTE ( PARAM ) ) ) {                                                                                       \
            f.at ( QUOTE ( PARAM ) ).get_to ( t.PARAM );                                                                           \
        }                                                                                                                          \
        else {                                                                                                                     \
            t.PARAM = {};                                                                                                          \
        }
#endif

struct DisplayDataDarkskyCurrent {
    // darksky.
    // apparentTemperature, cloudCover, dewPoint, humidity, icon, nearestStormDistance, ozone, precipIntensity,
    // precipProbability, pressure, summary, temperature, time, uvIndex, visibility, windBearing, windGust, windSpeed;
    float apparentTemperature, cloudCover, dewPoint, humidity;
    std::string icon;
    int nearestStormBearing;
    float nearestStormDistance, ozone, precipIntensity, precipProbability, pressure;
    std::string summary;
    float temperature;
    std::time_t time;
    int uvIndex;
    float visibility;
    int windBearing;
    float windGust, windSpeed;
    bool isDay;
};

struct DisplayDataDarkskyDay {
    // darksky.
    // apparentTemperatureHigh, apparentTemperatureHighTime, apparentTemperatureLow, apparentTemperatureLowTime,
    // apparentTemperatureMax, apparentTemperatureMaxTime, apparentTemperatureMin, apparentTemperatureMinTime, cloudCover,
    // dewPoint, humidity, icon, moonPhase, ozone, precipIntensity, precipIntensityMax, precipIntensityMaxTime, precipProbability,
    // precipType, pressure, summary, sunriseTime, sunsetTime, temperatureHigh, temperatureHighTime, temperatureLow,
    // temperatureLowTime, temperatureMax, temperatureMaxTime, temperatureMin, temperatureMinTime, time, uvIndex, uvIndexTime,
    // visibility, windBearing, windGust, windGustTime, windSpeed;
    float apparentTemperatureHigh;
    std::time_t apparentTemperatureHighTime;
    float apparentTemperatureLow;
    std::time_t apparentTemperatureLowTime;
    float apparentTemperatureMax;
    std::time_t apparentTemperatureMaxTime;
    float apparentTemperatureMin;
    std::time_t apparentTemperatureMinTime;
    float cloudCover, dewPoint, humidity;
    std::string icon;
    float moonPhase, ozone, precipIntensity, precipIntensityMax;
    std::time_t precipIntensityMaxTime;
    float precipProbability;
    std::string precipType;
    float pressure;
    std::string summary;
    std::time_t sunriseTime, sunsetTime;
    float temperatureHigh;
    std::time_t temperatureHighTime;
    float temperatureLow;
    std::time_t temperatureLowTime;
    float temperatureMax;
    std::time_t temperatureMaxTime;
    float temperatureMin;
    std::time_t temperatureMinTime, time;
    int uvIndex;
    std::time_t uvIndexTime;
    float visibility;
    int windBearing;
    float windGust;
    std::time_t windGustTime;
    float windSpeed;
};

using DisplayDataDarkskyDaily = std::array<DisplayDataDarkskyDay, 8>;

struct DisplayDataDarkskyHour {
    // darksky.
    // apparentTemperature, cloudCover, dewPoint, humidity, icon, ozone, precipIntensity, precipProbability, pressure,
    // summary, temperature, time, uvIndex, visibility, windBearing, windGust, windSpeed;
    float apparentTemperature, cloudCover, dewPoint, humidity;
    std::string icon;
    float ozone, precipIntensity, precipProbability, pressure;
    std::string summary;
    float temperature;
    std::time_t time;
    int uvIndex;
    float visibility;
    int windBearing;
    float windGust, windSpeed;
};

using DisplayDataDarkskyHourly = std::array<DisplayDataDarkskyHour, 169>;

struct DisplayDataDarkskyLocalTime {
    int offset;
    std::string timezone;
};

struct DisplayDataDarksky {
    DisplayDataDarkskyCurrent current;
    DisplayDataDarkskyDaily daily;
    DisplayDataDarkskyHourly hourly;
    DisplayDataDarkskyLocalTime time;
};

inline void from_json ( json const & j_, DisplayDataDarksky & d_ ) {
    {
        auto const & f = j_.at ( "currently" );
        auto & t       = d_.current;
        GET_DATA ( apparentTemperature )
        GET_DATA ( cloudCover )
        GET_DATA ( dewPoint )
        GET_DATA ( humidity )
        GET_DATA ( icon )
        GET_DATA ( nearestStormBearing )
        GET_DATA ( nearestStormDistance )
        GET_DATA ( ozone )
        GET_DATA ( precipIntensity )
        GET_DATA ( precipProbability )
        GET_DATA ( pressure )
        GET_DATA ( summary )
        GET_DATA ( temperature )
        GET_DATA ( time )
        GET_DATA ( uvIndex )
        GET_DATA ( visibility )
        GET_DATA ( windBearing )
        GET_DATA ( windGust )
        t.windGust *= 3.6f; // mps -> kmh.
        GET_DATA ( windSpeed )
        t.windSpeed *= 3.6f; // mps -> kmh.
    }
    {
        int i = 0;
        for ( auto const & f : j_.at ( "daily" ).at ( "data" ) ) {
            auto & t = d_.daily[ i ];
            GET_DATA ( apparentTemperatureHigh )
            GET_DATA ( apparentTemperatureHighTime )
            GET_DATA ( apparentTemperatureLow )
            GET_DATA ( apparentTemperatureLowTime )
            GET_DATA ( apparentTemperatureMax )
            GET_DATA ( apparentTemperatureMaxTime )
            GET_DATA ( apparentTemperatureMin )
            GET_DATA ( apparentTemperatureMinTime )
            GET_DATA ( cloudCover )
            GET_DATA ( dewPoint )
            GET_DATA ( humidity )
            GET_DATA ( icon )
            GET_DATA ( moonPhase )
            GET_DATA ( ozone )
            GET_DATA ( precipIntensity )
            GET_DATA ( precipIntensityMax )
            GET_DATA ( precipIntensityMaxTime )
            GET_DATA ( precipProbability )
            GET_DATA ( precipType )
            GET_DATA ( pressure )
            GET_DATA ( summary )
            GET_DATA ( sunriseTime )
            GET_DATA ( sunsetTime )
            GET_DATA ( temperatureHigh )
            GET_DATA ( temperatureHighTime )
            GET_DATA ( temperatureLow )
            GET_DATA ( temperatureLowTime )
            GET_DATA ( temperatureMax )
            GET_DATA ( temperatureMaxTime )
            GET_DATA ( temperatureMin )
            GET_DATA ( temperatureMinTime )
            GET_DATA ( time )
            GET_DATA ( uvIndex )
            GET_DATA ( uvIndexTime )
            GET_DATA ( visibility )
            GET_DATA ( windBearing )
            GET_DATA ( windGust )
            t.windGust *= 3.6f; // mps -> kmh.
            GET_DATA ( windGustTime )
            GET_DATA ( windSpeed )
            t.windSpeed *= 3.6f; // mps -> kmh.
            ++i;
        }
    }
    {
        int i = 0;
        for ( auto const & f : j_.at ( "hourly" ).at ( "data" ) ) {
            auto & t = d_.hourly[ i ];
            GET_DATA ( apparentTemperature )
            GET_DATA ( cloudCover )
            GET_DATA ( dewPoint )
            GET_DATA ( humidity )
            GET_DATA ( icon )
            GET_DATA ( ozone )
            GET_DATA ( precipIntensity )
            GET_DATA ( precipProbability )
            GET_DATA ( pressure )
            GET_DATA ( summary )
            GET_DATA ( temperature )
            GET_DATA ( time )
            GET_DATA ( uvIndex )
            GET_DATA ( visibility )
            GET_DATA ( windBearing )
            GET_DATA ( windGust )
            t.windGust *= 3.6f; // mps -> kmh.
            GET_DATA ( windSpeed )
            t.windSpeed *= 3.6f; // mps -> kmh.
            ++i;
        }
    }
    {
        auto const & f = j_;
        auto & t       = d_.time;
        GET_DATA ( offset )
        t.offset *= 3'600;
        GET_DATA ( timezone )
        t.offset = date::make_zoned ( d_.time.timezone, std::chrono::system_clock::now ( ) ).get_info ( ).offset.count ( ) / 3'600;
    }
}

struct DisplayDataApixuDay {
    // apixu.
    // sunrise, sunset, moonrise, moonset
    std::string sunrise, sunset, moonrise, moonset;
    std::time_t time;
};

using DisplayDataApixuDaily = std::array<DisplayDataApixuDay, 7>;

inline void from_json ( json const & j_, DisplayDataApixuDaily & d_ ) {
    int i = 0;
    for ( auto const & f_ : j_.at ( "forecast" ).at ( "forecastday" ) ) {
        auto const & f = f_.at ( "astro" );
        auto & t       = d_[ i ];
        GET_DATA ( sunrise )
        GET_DATA ( sunset )
        GET_DATA ( moonrise )
        GET_DATA ( moonset )
        f_.at ( "date_epoch" ).get_to ( t.time );
        ++i;
    }
}

// Everything in local time.

void handleEptr ( std::exception_ptr eptr ) { // Passing by value is ok.
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

int main ( ) {

    std::exception_ptr eptr;

    App app;

    try {
        json fa = forcast_apixu ( "Chicago", "United States" );
        std::cout << fa.dump ( g_indent ) << nl;

        json fd = forcast_darksky ( "Chicago", "United States" );
        std::cout << fd.dump ( g_indent ) << nl;

        DisplayDataDarksky ddc = fd;

        DisplayDataApixuDaily ddad = fa;

        std::cout << ddc.hourly[ 0 ].humidity << nl;
        std::cout << ddc.time.timezone << nl;

        std::cout << ddad[ 2 ].sunrise << nl;

        /*
        while ( app.is_active ( ) ) {
            app.run ( );
        }
        */
    }
    catch ( ... ) {
        eptr = std::current_exception ( ); // Capture.
    }
    handleEptr ( eptr );

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
