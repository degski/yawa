
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

#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <ctime>

#include <array>
#include <string>

#include <nlohmann/json.hpp> //

// for convenience.
using json = nlohmann::json;

struct DisplayDataDarkskyCurrent {
    // darksky.
    std::string apparentTemperature, cloudCover, dewPoint, humidity, icon, nearestStormBearing, nearestStormDistance, ozone,
        precipIntensity, precipProbability, pressure, summary, temperature;
    std::time_t time;
    std::string uvIndex, visibility, windBearing, windGust, windSpeed;
    bool isDay;
};

struct DisplayDataDarkskyDay {
    // darksky.
    std::string apparentTemperatureHigh;
    std::time_t apparentTemperatureHighTime;
    std::string apparentTemperatureLow;
    std::time_t apparentTemperatureLowTime;
    std::string apparentTemperatureMax;
    std::time_t apparentTemperatureMaxTime;
    std::string apparentTemperatureMin;
    std::time_t apparentTemperatureMinTime;
    std::string cloudCover, dewPoint, humidity, icon, moonPhase, ozone, precipIntensity, precipIntensityMax;
    std::time_t precipIntensityMaxTime;
    std::string precipProbability, precipType, pressure, summary;
    std::time_t sunriseTime, sunsetTime;
    std::string temperatureHigh;
    std::time_t temperatureHighTime;
    std::string temperatureLow;
    std::time_t temperatureLowTime;
    std::string temperatureMax;
    std::time_t temperatureMaxTime;
    std::string temperatureMin;
    std::time_t temperatureMinTime, time;
    std::string uvIndex;
    std::time_t uvIndexTime;
    std::string visibility, windBearing, windGust;
    std::time_t windGustTime;
    std::string windSpeed;
};

using DisplayDataDarkskyDaily = std::array<DisplayDataDarkskyDay, 8>;

struct DisplayDataDarkskyHour {
    // darksky.
    std::string apparentTemperature, cloudCover, dewPoint, humidity, icon, ozone, precipIntensity, precipProbability, pressure,
        summary, temperature;
    std::time_t time;
    std::string uvIndex, visibility, windBearing, windGust, windSpeed;
};

using DisplayDataDarkskyHourly = std::array<DisplayDataDarkskyHour, 169>;

struct DisplayDataDarkskyLocalTime {
    std::time_t offset;
    std::string timezone;
};

struct DisplayDataDarksky {
    DisplayDataDarkskyCurrent current;
    DisplayDataDarkskyDaily daily;
    DisplayDataDarkskyHourly hourly;
    DisplayDataDarkskyLocalTime time;
    std::time_t update_time = 0u;

    [[nodiscard]] bool is_stale ( ) noexcept { return ( std::time ( nullptr ) - update_time ) > 3'600u; }
    [[nodiscard]] bool is_updated ( ) noexcept { return not is_stale ( ); }
};

struct DisplayDataApixuDay {
    // apixu.
    std::string sunrise, sunset, moonrise, moonset;
    std::time_t time;
};

using DisplayDataApixuDaily = std::array<DisplayDataApixuDay, 7>;

struct DisplayDataApixu {
    DisplayDataApixuDaily daily;
    std::time_t update_time = 0u;

    [[nodiscard]] bool is_stale ( ) noexcept { return ( std::time ( nullptr ) - update_time ) > ( 3'600u * 24 ); }
    [[nodiscard]] bool is_updated ( ) noexcept { return not is_stale ( ); }
};

void from_json ( json const & j_, DisplayDataDarksky & d_ );
void from_json ( json const & j_, DisplayDataApixu & d_ );

struct DisplayData {
    DisplayDataDarksky darksky;
    DisplayDataApixu apixu;

    void normalize_times ( ) noexcept {
        for ( auto & d : apixu.daily )
            d.time -= darksky.time.offset;
        apixu.update_time -= darksky.time.offset;
    }
};
