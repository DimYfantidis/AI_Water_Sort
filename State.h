#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <random>
#include <ctime>
#include <iomanip>
#include <cstring>
#include <initializer_list>
#include <array>

#include "Bottle.h"
#include "colors.h"

#define ACTION_NAME_SIZE 2


/*
 *  State class:
 *      The following's class instances represent the different
 *      snapshots of the puzzle. Each instance is supplied with
 *      variables and methods to be used during the execution of
 *      AI algorithms.
 *
 *      The class initially used std::vector<Bottle> (24 bytes)
 *      for storing the bottles in each snapshot of the puzzle.
 *      For optimal memory usage, it has now been changed to a
 *      pointer to Bottle data type and a char for storing its
 *      size, given that number of bottles doesn't exceed 19.
 *      (total: 9 bytes)
 *      (see also: bsize_t)
 *
 *
 *  Class' important methods:
 *
 *  ->  init():
 *          Initializes the state's bottles with N - 2 random colors
 *          (4mL each) in a random sequence.
 *
 *  ->  hashValue():
 *          Returns the hash code of the state for storing and searching
 *          State instances in the closed set of AI algorithms.
 *          (see also: hash_t)
 *
 *  ->  toString():
 *          Returns an std::string as an attempt to represent a snapshot of
 *          the puzzle visually using ASCII characters.
 *
 *  ->  isVictorious():
 *          True if isComplete() (see Bottle.h) returns true for every bottle
 *          of the current state, false otherwise.
 *          If the return value evaluates to true, then the puzzle has reached
 *          the goal state.
 *
 *  ->  getDepth():
 *          Returns the depth of the state-node in the state tree.
 *
 *  ->  pour(State &n, int i, int j):
 *          Attempts to pour from bottle[i] to bottle[j]. If bottle[i].shouldPourTo(bottle[j])
 *          evaluates to true, then a new state is created, stored to n, added to the state tree
 *          and the color of the poured liquid is returned.
 *
 *  ->  expand(std::vector<State *> &):
 *          Returns the set of the child states.
 */


typedef long long hash_t;
typedef char      bsize_t;


__pragma(pack(push, 1))

template <size_t size>
class State
{
    static_assert(size > 2, "Enter a valid amount of bottles (N > 2).");
    static_assert(size < 18, "Number of bottles must be less than 18.");

private:
    bsize_t actionName[ACTION_NAME_SIZE];

    State<size>* prev;

    Bottle bottles[size];

    color_t pour(State<size>* n, int from, int to);   // TRANSITION OPERATOR

public:
    State();

    State(const State<size>& other);

    State(std::initializer_list<Bottle> init);

    ~State() = default;

    void init();

    void setActionName(bsize_t from, bsize_t to);

    void setPrevious(State* p) { prev = p; }

    bsize_t numOfBottles() const { return size; }

    Bottle* getBottles() { return &bottles[0]; }

    State<size>* getPrevious() const { return prev; }

    hash_t hashValue() const;

    std::string getActionName() const;

    std::string toString() const;

    bool isVictorious() const;

    bool hasFreeSpace(int pos) const { return bottles[pos].hasFreeSpace(); }

    int getDepth() const;

    void expand(std::vector<State<size>*>&);

    State<size>* copyWholePath() const;

public:
    State<size>& operator = (const State<size>& other);

    bool operator == (const State<size>& other) const;

    bool operator != (const State<size>& other) const;
};

__pragma(pack(pop))


template <size_t size>
State<size>::State() 
{
    prev = nullptr;
    actionName[0] = '\0';
}

template <size_t size>
State<size>::State(const State<size>& other)
{
    memcpy(bottles, other.bottles, size * BOTTLE_SIZE);
    memcpy(actionName, other.actionName, ACTION_NAME_SIZE * sizeof(char));
    prev = other.prev;
}

template <size_t size>
State<size>::State(std::initializer_list<Bottle> l)
{
    size_t i = 0;

    for (const Bottle& b : l) {
        bottles[i++] = b;
    }
    actionName[0] = '\0';
    prev = nullptr;
}

template <size_t size>
color_t State<size>::pour(State<size>* n, int from, int to)
{
    color_t poured_color = NO_COLOR;

    std::ostringstream oss;

    int before;
    int after;

    n->getBottles()[from].top(before);
    poured_color = n->getBottles()[from].pour(n->getBottles()[to]);
    n->getBottles()[from].top(after);

    n->setActionName((bsize_t)(from + 1), (bsize_t)(to + 1));
    n->setPrevious(this);

    return poured_color;
}

template <size_t size>
void State<size>::init()
{
    std::vector<color_t> colors(size - 2);
    std::vector<int> ml_left(colors.size(), 4);

    static std::mt19937 generator(static_cast<unsigned int>(time(nullptr) + 7));

    std::uniform_int_distribution<size_t> distribution(1, TOTAL_COLORS);
    std::uniform_int_distribution<size_t> color_dist(0, colors.size() - 1);

    size_t i;
    size_t j;
    size_t k;
    color_t c;

    for (i = 0; i < colors.size(); ++i)
    {
        c = static_cast<color_t>(distribution(generator));

        for (j = 0; j < i; ++j) {
            if (c == colors[j]) 
            {
                c = static_cast<color_t>(distribution(generator));
                j = 0;
            }
        }
        colors[i] = c;
    }

    for (k = 0; k < size - 2; ++k) 
    {
        for (i = 0; i < NUM_OF_COLORS; ++i) 
        {
            do {
                j = color_dist(generator);
            } while (!ml_left[j]);
            bottles[k].setColor(i, colors[j]);
            ml_left[j] -= 1;
        }
    }
    actionName[0] = '\0';
}

template <size_t size>
void State<size>::setActionName(bsize_t from, bsize_t to)
{
    actionName[0] = from;
    actionName[1] = to;
}

template <size_t size>
hash_t State<size>::hashValue() const
{
    size_t i, j;
    hash_t hash;

    hash = 1125899906842597LL;

    for (i = 0; i < size; ++i)
    {
        for (j = 0; j < BOTTLE_SIZE; ++j) {
            hash = 31 * hash + static_cast<hash_t>(bottles[i].getByte(j));
        }
    }
    return hash;
}

template <size_t size>
std::string State<size>::getActionName() const
{
    if (!actionName[0]) {
        return { "No action committed (initial state)" };
    }

    color_t moved_color;

    std::ostringstream oss;

    State<size> previous(*prev);

    int before;
    int after;

    previous.getBottles()[actionName[0] - 1].top(before);
    moved_color = previous.getBottles()[actionName[0] - 1].pour(previous.getBottles()[actionName[1] - 1]);
    previous.getBottles()[actionName[0] - 1].top(after);

    oss << "Poured " << std::setw(2) << std::setfill(' ') << static_cast<int>(after - before)
        << " mL of " << COLOR_STR[moved_color]
        << " from bottle " << std::setw(2) << std::setfill(' ') << static_cast<int>(actionName[0])
        << " to bottle " << std::setw(2) << std::setfill(' ') << static_cast<int>(actionName[1]);

    return oss.str();
}

template <size_t size>
std::string State<size>::toString() const
{
    std::ostringstream oss;

    color_t c;

    size_t i;

    for (i = 0; i < size; ++i) {
        oss << (i < 9 ? "   [" : "  [") << (int)(i + 1) << "]      ";
    }

    oss << '\n';

    for (i = 0; i < 4; ++i) 
    {
        for (size_t k = 0; k < size; ++k) 
        {
            c = bottles[k].getColor(i);
            oss << "|" << (c != NO_COLOR ? COLOR_STR[c] : "       ") << "|   ";
        }
        oss << '\n';
    }
    for (i = 0; i < size; ++i) {
        oss << "---------   ";
    }

    return oss.str();
}

template <size_t size>
bool State<size>::isVictorious() const
{
    for (size_t i = 0; i < size; ++i) {
        if (!bottles[i].isComplete()) {
            return false;
        }
    }
    return true;
}

template <size_t size>
int State<size>::getDepth() const
{
    if (prev == nullptr) {
        return 0;
    }
    return 1 + prev->getDepth();
}

template <size_t size>
void State<size>::expand(std::vector<State<size>*>& children)
{
    State* child;

    bsize_t i;
    bsize_t j;

    children.clear();

    for (i = 0; i < numOfBottles(); ++i)
    {
        for (j = 0; j < numOfBottles(); ++j)
        {
            if (i == j) continue;

            if (bottles[i].shouldPourTo(bottles[j]))
            {
                child = new State(*this);

                pour(child, i, j);

                children.push_back(child);
            }
        }
    }
}

template <size_t size>
State<size>* State<size>::copyWholePath() const
{
    auto* s = new State(*this);

    for (State* t = s; t->getPrevious() != nullptr; t = t->getPrevious()) {
        t->setPrevious(new State(*t->getPrevious()));
    }
    return s;
}

template <size_t size>
State<size>& State<size>::operator = (const State<size>& other)
{
    if (this != &other)
    {
        for (size_t i = 0; i < size; ++i) {
            bottles[i] = other.bottles[i];
        }
        prev = other.prev;
        memcpy(actionName, other.actionName, ACTION_NAME_SIZE * sizeof(char));
    }
    return *this;
}

template <size_t size>
bool State<size>::operator == (const State<size>& other) const
{
    if (this != &other)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (bottles[i].getColor(0) != other.bottles[i].getColor(0)
                || bottles[i].getColor(1) != other.bottles[i].getColor(1)
                || bottles[i].getColor(2) != other.bottles[i].getColor(2)
                || bottles[i].getColor(3) != other.bottles[i].getColor(3)
                ) {
                return false;
            }
        }
    }
    return true;
}

template <size_t size>
bool State<size>::operator != (const State<size>& other) const
{
    if (this != &other)
    {
        for (size_t i = 0; i < size; ++i)
        {
            if (bottles[i].getColor(0) != other.bottles[i].getColor(0)
                || bottles[i].getColor(1) != other.bottles[i].getColor(1)
                || bottles[i].getColor(2) != other.bottles[i].getColor(2)
                || bottles[i].getColor(3) != other.bottles[i].getColor(3)
                ) {
                return true;
            }
        }
    }
    return false;
}
