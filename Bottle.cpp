#include <iostream>
#include "Bottle.h"


Bottle::Bottle() 
{
    contents[0] = NO_COLOR;
    contents[1] = NO_COLOR;
}

Bottle::Bottle(color_t top, color_t second, color_t third, color_t bottom)
{
    bool flag = true;
    int i;

    contents[0] = top << 4;
    contents[0] |= second;

    contents[1] = third << 4;
    contents[1] |= bottom;

    for (i = NUM_OF_COLORS - 1; i >= 0 && (flag || getColor(i) == NO_COLOR); --i)
    {
        if (getColor(i) == NO_COLOR) {
            flag = false;
        }
    }

    if (i >= 0) {
        exit(-1);
    }
}

void Bottle::setColor(size_t i, color_t c)
{
    size_t t = i / 2;

    if (i % 2 == 0)
    {
        contents[t] &= LSB_MASK;
        contents[t] |= (c << 4);
    }
    else
    {
        contents[t] &= MSB_MASK;
        contents[t] |= c;
    }
}

color_t Bottle::getColor(size_t i) const
{
    if (i % 2 == 0) {
        return contents[i / 2] >> 4;
    }
    return contents[i / 2] & LSB_MASK;
}

color_t Bottle::getByte(size_t i) const {
    return contents[i];
}

bool Bottle::hasFreeSpace() const {
    return getColor(0) == NO_COLOR;
}

bool Bottle::isComplete() const
{
    if (this->isEmpty()) {
        return true;
    }
    if (getColor(0) == NO_COLOR) {
        return false;
    }
    for (int i = 1; i < 4; ++i) {
        if (getColor(i) != getColor(i - 1)) {
            return false;
        }
    }
    return true;
}

bool Bottle::shouldPourTo(const Bottle& other) const
{
    color_t c;
    int l1;
    int l2;
    int continuous_ml = 0;

    if (this->isEmpty() || !other.hasFreeSpace()) {
        return false;
    }
    if (other.isEmpty()) {
        return true;
    }
    if ((c = this->top(l1)) != other.top(l2)) {
        return false;
    }
    for (int i = l1; i < 4 && getColor(i) == c; ++i) {
        continuous_ml += 1;
    }
    if (continuous_ml > l2) {
        return false;
    }
    return true;
}

color_t Bottle::top() const
{
    for (size_t i = 0; i < NUM_OF_COLORS; ++i)
    {
        if (getColor(i) != NO_COLOR) {
            return getColor(i);
        }
    }
    return NO_COLOR;
}

color_t Bottle::top(int& i) const
{
    for (i = 0; i < NUM_OF_COLORS; ++i) {
        if (getColor(i) != NO_COLOR) {
            return getColor(i);
        }
    }
    return NO_COLOR;
}

color_t Bottle::pour(Bottle& to)
{
    int i;
    int pos1;
    int pos2;

    color_t color_to_be_poured = this->top(pos1);
    color_t top_of_other = to.top(pos2);

    if (top_of_other == NO_COLOR)
    {
        for (i = 0; ((pos1 + i) < NUM_OF_COLORS) && (getColor(pos1 + i) == color_to_be_poured) && to.hasFreeSpace(); ++i) {
            setColor(pos1 + i, NO_COLOR);
            to.setColor(3 - i, color_to_be_poured);
        }
    }
    else
    {
        if (top_of_other != color_to_be_poured) {
            return NO_COLOR;
        }
        for (i = 0; ((pos1 + i) < NUM_OF_COLORS) && (getColor(pos1 + i) == color_to_be_poured) && to.hasFreeSpace(); ++i) {
            setColor(pos1 + i, NO_COLOR);
            to.setColor(pos2 - i - 1, color_to_be_poured);
        }
    }
    return color_to_be_poured;
}

bool Bottle::operator == (const Bottle& other) const
{
    return contents[0] == other.contents[0] &&
        contents[1] == other.contents[1];
}

bool Bottle::isEmpty() const {
    return getColor(3) == NO_COLOR;
}

Bottle& Bottle::operator = (const Bottle& other) 
{
    if (this != &other)
    {
        contents[0] = other.contents[0];
        contents[1] = other.contents[1];
    }
    return *this;
}