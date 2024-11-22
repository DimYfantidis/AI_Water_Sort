#pragma once

/*
 *  Header file containing definitions for:
 *  ->  Data type representing the different colors of the liquids.
 *  ->  Colors' code values.
 *  ->  Absence of color code value.
 *  ->  Array containing the different colors as strings, each one indexed
 *      with it's matched code.
 *  ->  Size of aforementioned array.
 */

typedef unsigned char color_t;

#define NO_COLOR    static_cast<color_t>(0b00000000)  // 0

#define BLACK       static_cast<color_t>(0b00000001)  // 1
#define BROWN       static_cast<color_t>(0b00000010)  // 2
#define GREY        static_cast<color_t>(0b00000011)  // 3
#define YELLOW      static_cast<color_t>(0b00000100)  // 4
#define CYAN        static_cast<color_t>(0b00000101)  // 5
#define MAGENTA     static_cast<color_t>(0b00000110)  // 6
#define LIME        static_cast<color_t>(0b00000111)  // 7
#define PURPLE      static_cast<color_t>(0b00001000)  // 8
#define GREEN       static_cast<color_t>(0b00001001)  // 9
#define EMERALD     static_cast<color_t>(0b00001010)  // 10
#define WHITE       static_cast<color_t>(0b00001011)  // 11
#define BLUE        static_cast<color_t>(0b00001100)  // 12
#define ORANGE      static_cast<color_t>(0b00001101)  // 13
#define PINK        static_cast<color_t>(0b00001110)  // 14
#define RED         static_cast<color_t>(0b00001111)  // 15


static const char* COLOR_STR[] = { "",
        "BLACK  ", "BROWN  ", "GREY   ", "YELLOW ", "CYAN   ",
        "MAGENTA", "LIME   ", "PURPLE ", "GREEN  ", "EMERALD",
        "WHITE  ", "BLUE   ", "ORANGE ", "PINK   ", "RED    "
};

constexpr size_t TOTAL_COLORS = (sizeof(COLOR_STR) / sizeof(COLOR_STR[0])) - 1 ;
