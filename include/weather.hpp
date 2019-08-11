
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

#include <map>
#include <string>

#include "globals.hpp"

// https://api.met.no/weatherapi/weathericon/1.1/documentation
inline constexpr char const * g_weather_icon[]{ "Empty",
                                                "Sun",
                                                "LightCloud",
                                                "PartlyCloud",
                                                "Cloud",
                                                "LightRainSun",
                                                "LightRainThunderSun",
                                                "SleetSun",
                                                "SnowSun",
                                                "LightRain",
                                                "Rain",
                                                "RainThunder",
                                                "Sleet",
                                                "Snow",
                                                "SnowThunder",
                                                "Fog",
                                                "SleetSunThunder",
                                                "SnowSunThunder",
                                                "LightRainThunder",
                                                "SleetThunder",
                                                "DrizzleThunderSun",
                                                "RainThunderSun",
                                                "LightSleetThunderSun",
                                                "HeavySleetThunderSun",
                                                "LightSnowThunderSun",
                                                "HeavySnowThunderSun",
                                                "DrizzleThunder",
                                                "LightSleetThunder",
                                                "HeavySleetThunder",
                                                "LightSnowThunder",
                                                "HeavySnowThunder",
                                                "DrizzleSun",
                                                "RainSun",
                                                "LightSleetSun",
                                                "HeavySleetSun",
                                                "LightSnowSun",
                                                "HeavySnowSun",
                                                "Drizzle",
                                                "LightSleet",
                                                "HeavySleet",
                                                "LightSnow",
                                                "HeavySnow" };

#include "descriptions.inl"

inline constexpr int descriptions_size ( ) noexcept {
    return static_cast<int> ( sizeof ( g_descriptions ) / sizeof ( g_descriptions[ 0 ] ) );
}

[[nodiscard]] json forcast_query_apixu ( std::string const & name_, std::string const & country_ );
[[nodiscard]] json forcast_query_darksky ( std::string const & name_, std::string const & country_ );

[[nodiscard]] json forcast_query_apixu ( place_t const & pd_ );
[[nodiscard]] json forcast_query_darksky ( place_t const & pd_ );

[[nodiscard]] json forcast_load_apixu ( std::string const & name_, std::string const & country_ );
[[nodiscard]] json forcast_load_darksky ( std::string const & name_, std::string const & country_ );

[[nodiscard]] json forcast_load_apixu ( place_t const & pd_ );
[[nodiscard]] json forcast_load_darksky ( place_t const & pd_ );

// https://api.met.no/weatherapi/locationforecast/1.9/.json?lat=39.79&lon=19.81&msl=6
