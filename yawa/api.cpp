
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
#include <ctime>

#include <charconv>
#include <chrono>
#include <string_view>
#include <type_traits>

#include <fmt/core.h>

#include <date/date.h>
#include <date/tz.h>

#include "api.hpp"
#include "globals.hpp"

#define Q_( x ) #x
#define QUOTE( x ) Q_ ( x )

struct integer_string_tag {};
struct kmh_tag {};
struct percentage_tag {};
struct real_tag {};
struct real_string_tag {};
struct string_tag {};
struct temperature_tag {};
struct time_tag {};
struct ampm_tag {};

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, integer_string_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        int number = 0;
        f.at ( param ).get_to ( number );
        t = fmt::format ( "{0:}", number );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, kmh_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        float number = 0.0f;
        f.at ( param ).get_to ( number );
        t = fmt::format ( "{0:.1f}", number );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, percentage_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        float number = 0.0f;
        f.at ( param ).get_to ( number );
        number *= 100.0f;
        t = fmt::format ( "{0:.0f}%", number );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, real_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) )
        f.at ( param ).get_to ( t );
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, real_string_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        double number = 0.0;
        f.at ( param ).get_to ( number );
        t = fmt::format ( "{0:.1f}", number );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, string_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) )
        f.at ( param ).get_to ( t );
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, temperature_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        float number = 0.0f;
        f.at ( param ).get_to ( number );
        t = fmt::format ( "{0:.0f}\370C", number );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, time_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) )
        f.at ( param ).get_to ( t );
}

template<typename T>
void add_12 ( T * const b, T * const e ) noexcept {
    int hour;
    std::from_chars ( b, e, hour );
    hour += 12;
    std::to_chars ( b, e, hour );
}

void to_24 ( std::string & am_ ) noexcept {
    if ( 'M' == am_[ 7 ] ) { // Check for not "No moonrise" or "No moonset".
        if ( 'P' == am_[ 6 ] )
            add_12 ( am_.data ( ), am_.data ( ) + 2 );
        am_.resize ( 5 );
    }
}

template<typename Tag, typename T>
typename std::enable_if<std::is_same<Tag, ampm_tag>::value>::type convert ( json const & f, char const param[], T & t ) {
    if ( f.count ( param ) ) {
        f.at ( param ).get_to ( t );
        to_24 ( t );
    }
}

#define GET_API_DATA( PARAM, CONVERSION ) convert<CONVERSION> ( f, QUOTE ( PARAM ), t.PARAM );

void from_json ( json const & j_, DisplayDataDarksky & d_ ) {
    {
        auto const & f = j_.at ( "currently" );
        auto & t       = d_.current;
        GET_API_DATA ( apparentTemperature, temperature_tag )
        GET_API_DATA ( cloudCover, percentage_tag )
        GET_API_DATA ( dewPoint, temperature_tag )
        GET_API_DATA ( humidity, percentage_tag )
        GET_API_DATA ( icon, string_tag )
        GET_API_DATA ( nearestStormBearing, integer_string_tag )
        GET_API_DATA ( nearestStormDistance, integer_string_tag )
        GET_API_DATA ( ozone, real_string_tag )
        GET_API_DATA ( precipIntensity, real_string_tag )
        GET_API_DATA ( precipProbability, percentage_tag )
        GET_API_DATA ( pressure, real_string_tag )
        GET_API_DATA ( summary, string_tag )
        GET_API_DATA ( temperature, temperature_tag )
        GET_API_DATA ( time, time_tag )
        GET_API_DATA ( uvIndex, integer_string_tag )
        GET_API_DATA ( visibility, real_string_tag )
        GET_API_DATA ( windBearing, integer_string_tag )
        GET_API_DATA ( windGust, kmh_tag )
        GET_API_DATA ( windSpeed, kmh_tag )
    }
    {
        int i = 0;
        for ( auto const & f : j_.at ( "daily" ).at ( "data" ) ) {
            auto & t = d_.daily[ i ];
            GET_API_DATA ( apparentTemperatureHigh, temperature_tag )
            GET_API_DATA ( apparentTemperatureHighTime, time_tag )
            GET_API_DATA ( apparentTemperatureLow, temperature_tag )
            GET_API_DATA ( apparentTemperatureLowTime, time_tag )
            GET_API_DATA ( apparentTemperatureMax, temperature_tag )
            GET_API_DATA ( apparentTemperatureMaxTime, time_tag )
            GET_API_DATA ( apparentTemperatureMin, temperature_tag )
            GET_API_DATA ( apparentTemperatureMinTime, time_tag )
            GET_API_DATA ( cloudCover, percentage_tag )
            GET_API_DATA ( dewPoint, temperature_tag )
            GET_API_DATA ( humidity, percentage_tag )
            GET_API_DATA ( icon, string_tag )
            GET_API_DATA ( moonPhase, percentage_tag )
            GET_API_DATA ( ozone, real_string_tag )
            GET_API_DATA ( precipIntensity, real_string_tag )
            GET_API_DATA ( precipIntensityMax, real_string_tag )
            GET_API_DATA ( precipIntensityMaxTime, time_tag )
            GET_API_DATA ( precipProbability, percentage_tag )
            GET_API_DATA ( precipType, string_tag )
            GET_API_DATA ( pressure, real_string_tag )
            GET_API_DATA ( summary, string_tag )
            GET_API_DATA ( sunriseTime, time_tag )
            GET_API_DATA ( sunsetTime, time_tag )
            GET_API_DATA ( temperatureHigh, temperature_tag )
            GET_API_DATA ( temperatureHighTime, time_tag )
            GET_API_DATA ( temperatureLow, temperature_tag )
            GET_API_DATA ( temperatureLowTime, time_tag )
            GET_API_DATA ( temperatureMax, temperature_tag )
            GET_API_DATA ( temperatureMaxTime, time_tag )
            GET_API_DATA ( temperatureMin, temperature_tag )
            GET_API_DATA ( temperatureMinTime, time_tag )
            GET_API_DATA ( time, time_tag )
            GET_API_DATA ( uvIndex, integer_string_tag )
            GET_API_DATA ( uvIndexTime, time_tag )
            GET_API_DATA ( visibility, real_string_tag )
            GET_API_DATA ( windBearing, integer_string_tag )
            GET_API_DATA ( windGust, kmh_tag )
            GET_API_DATA ( windGustTime, time_tag )
            GET_API_DATA ( windSpeed, kmh_tag )
            ++i;
        }
    }
    {
        int i = 0;
        for ( auto const & f : j_.at ( "hourly" ).at ( "data" ) ) {
            auto & t = d_.hourly[ i ];
            GET_API_DATA ( apparentTemperature, temperature_tag )
            GET_API_DATA ( cloudCover, percentage_tag )
            GET_API_DATA ( dewPoint, temperature_tag )
            GET_API_DATA ( humidity, percentage_tag )
            GET_API_DATA ( icon, string_tag )
            GET_API_DATA ( ozone, real_string_tag )
            GET_API_DATA ( precipIntensity, real_string_tag )
            GET_API_DATA ( precipProbability, percentage_tag )
            GET_API_DATA ( pressure, real_string_tag )
            GET_API_DATA ( summary, string_tag )
            GET_API_DATA ( temperature, temperature_tag )
            GET_API_DATA ( time, time_tag )
            GET_API_DATA ( uvIndex, integer_string_tag )
            GET_API_DATA ( visibility, real_string_tag )
            GET_API_DATA ( windBearing, integer_string_tag )
            GET_API_DATA ( windGust, kmh_tag )
            GET_API_DATA ( windSpeed, kmh_tag )
            ++i;
        }
    }
    {
        auto const & f = j_;
        auto & t       = d_.time;
        GET_API_DATA ( offset, time_tag )
        t.offset *= 3'600;
        GET_API_DATA ( timezone, string_tag )
        // t.offset = date::make_zoned ( d_.time.timezone, std::chrono::system_clock::now ( ) ).get_info ( ).offset.count ( );
    }
    d_.update_time = d_.current.time;
}

void from_json ( json const & j_, DisplayDataApixu & d_ ) {
    {
        int i = 0;
        for ( auto const & f_ : j_.at ( "forecast" ).at ( "forecastday" ) ) {
            auto const & f = f_.at ( "astro" );
            auto & t       = d_.daily[ i ];
            GET_API_DATA ( sunrise, ampm_tag )
            GET_API_DATA ( sunset, ampm_tag )
            GET_API_DATA ( moonrise, ampm_tag )
            GET_API_DATA ( moonset, ampm_tag )
            f_.at ( "date_epoch" ).get_to ( t.time );
            ++i;
        }
    }
    d_.update_time = j_.at ( "location" ).at ( "localtime_epoch" ).get<std::time_t> ( );
}

/*

{
  "location": {
    "latitude": 39.79222,
    "longitude": 19.81449
  },
  "date": "2019-08-17",
  "sunrise": "06:54",
  "sunset": "20:35",
  "solar_noon": "13:45",
  "day_length": "13:41",
  "sun_altitude": -33.85443500329109,
  "sun_distance": 151491900.79337415,
  "sun_azimuth": 22.097752758545425,
  "moonrise": "21:54",
  "moonset": "08:25",
  "moon_altitude": 38.54675424071512,
  "moon_distance": 405896.39789071336,
  "moon_azimuth": 182.30506383152255,
  "moon_parallactic_angle": 13.822913692056744
}


struct DisplayDataAstro { // from ipgeolocation.
    std::time_t time;
    std::string sunrise, sunset, moonrise, moonset, day_length;
    double sun_distance, moon_distance;
};

*/

void from_json ( json const & j_, DisplayDataAstro & d_ ) {
    d_.time        = date_to_epoch ( j_.at ( "date" ).get<std::string_view> ( ) );
    auto const & f = j_;
    auto & t       = d_;
    GET_API_DATA ( sunrise, string_tag )
    GET_API_DATA ( sunset, string_tag )
    GET_API_DATA ( moonrise, string_tag )
    GET_API_DATA ( moonset, string_tag )
    GET_API_DATA ( day_length, string_tag )
    GET_API_DATA ( sun_distance, real_tag )
    GET_API_DATA ( moon_distance, real_tag )
}
