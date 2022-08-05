#pragma once

#include "colors.h"

#define NUM_OF_COLORS   4
#define BOTTLE_SIZE     2

#define LSB_MASK    static_cast<color_t>(0b00001111)
#define MSB_MASK    static_cast<color_t>(0b11110000)

#if defined(__GNUC__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#elif defined(_MSC_VER)
#define PACK( __Declaration__ ) __pragma(pack(push, 1)) __Declaration__ __pragma(pack(pop))
#else
#define PACK( __Declaration__ ) __Declaration__
#endif


/*
 *  Bottle class:
 * 
 *      The following class represents a bottle of 4mL capacity.
 *      An array of these objects will be used for the representation
 *      of different snapshots (states) of the puzzle.
 *
 *      The class also provides some degree of encapsulation for easier
 *      and safer code programming.
 *
 * 
 *  Class' methods:
 * 
 *  ->  hasFreeSpace():
 *          True if the bottle is not full, false otherwise.
 * 
 *  ->  isEmpty():
 *          True if completely empty, meaning contents[i] = NO_COLOR
 *          for every i in [0, 4), false otherwise.
 * 
 *  ->  isComplete():
 *          True if empty or full of the same color, false otherwise
 * 
 *  ->  shouldPourTo(const Bottle &):
 *          True if k mL of a continuous colored liquid can be moved from
 *          the of the bottle (without other colors intervening) either in
 *          some empty bottle or a bottle that has liquid of the same color
 *          at its top and has k mL empty space.
 * 
 *  ->  top():
 *          Returns the color at the bottle's top.
 * 
 *  ->  top(int &):
 *          Overloaded function of top() that returns the layer of the
 *          color (equivalent to free space) by reference.
 * 
 *  ->  pour(Bottle &):
 *          Pours liquid from the bottle to the referring bottle.
 *          If successful the color of the poured liquid is returned,
 *          NO_COLOR otherwise.
 * 
 *  ->  getColor(size_t i):
 *          returns contents[i]
 */

__pragma(pack(push, 1))

class Bottle
{
private:
    color_t contents[BOTTLE_SIZE] = {};

public:
    Bottle();

    Bottle(color_t, color_t, color_t, color_t);

    bool hasFreeSpace() const;

    bool isEmpty() const;

    bool isComplete() const;

    bool shouldPourTo(const Bottle&) const;

    color_t top() const;

    color_t top(int&) const;

    color_t pour(Bottle&);

    color_t getColor(size_t) const;

    color_t getByte(size_t) const;

    void setColor(size_t, color_t);

    bool operator == (const Bottle&) const;

    Bottle& operator = (const Bottle&);
};

__pragma(pack(pop))
