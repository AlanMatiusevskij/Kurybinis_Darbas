#pragma once

//Standard libraries
#include<fstream>
#include<iostream>
#include<vector>
#include<string>
#include<cmath>
#include<chrono>
#include<bitset>
#include<filesystem>
#include<algorithm>

///////////////////

namespace customcpp{
    /**
     * Returns the -almost- exact number of updates per second.
     * @param _cout `true` if you want to get a console message: `UPS: upsValue`.
    */
    int upsAverage(bool _cout);

///////////////////////////////

    /**
     * Returns a number value represented as a string.
     * @param a number.
     * @return a number as a string.
    */
    std::string intToString(int numb);
    /**
     * Returns an integer value from a given string.
     * @param a string holding a number.
     * @return a number as an int.
    */
    int stringToInt(std::string string);

    /**
     * Returns a number of `double` type represented as a string.
     * @param in a `double` type number.
     * @param precision number of fractional values.
     * @return a `double` type numbers as a string.
    */
    std::string doubleToString(double in, int precision);
    /**
     * Returns a number of `double` type from a given string.
     * Fractional part can be seperated either by a comma or a dot.
     * @param in a string holding a number.
     * @return a number of type `double`.
    */
    double stringToDouble(std::string in);

//////////////////////////////////////////


}