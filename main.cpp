#include <iostream>
#include <fstream>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <thread>
#include <cstdint>

#include "output_util.h"
#include "State.h"

// Number of Bottles.
#define BOTTLES_N       static_cast<size_t>(11)

// Record current time.
#define READ_TIME()     std::chrono::system_clock::now()

// Difference of two timestamps in milliseconds.
#define MS_DIFF(x, y)   std::chrono::duration_cast<std::chrono::milliseconds>(y - x).count()


// Implementation of Breadth First Search AI algorithm
template <size_t size>
State<size>* BFS(State<size>& initial, uint64_t& examined, uint64_t& memory)
{
    std::queue<State<size>*> frontier;

    std::unordered_set<hash_t> closed;

    std::vector<State<size>*> children;

    State<size>* s;

    auto clearMemory = [&]()
    {
        while (!frontier.empty())
        {
            delete frontier.front();

            frontier.pop();
        }
    };

    frontier.push(new State<size>(initial));
    examined = 0;
    memory = 1;

    while (!frontier.empty())
    {
        if (frontier.size() + closed.size() > memory) {
            memory = frontier.size() + closed.size();
        }

        s = frontier.front();

        frontier.pop();

        if (closed.find(s->hashValue()) == closed.end())
        {
            examined += 1;

            // Goal state reached.
            if (s->isVictorious())
            {
                State<size>* result = s->copyWholePath();

                delete s;

                clearMemory();

                return result;
            }
            closed.insert(s->hashValue());

            s->expand(children);

            for (State<size>* child : children)
            {
                if (closed.find(child->hashValue()) == closed.end())
                    frontier.push(child);
                else
                    delete child;
            }
        }
        else
        {
            delete s;
        }
    }
    clearMemory();

    return nullptr;
}

/*
State<14> startValue()
{
    return 
    { 
        {BLUE, GREEN, CYAN, CYAN}, 
        {PURPLE, PINK, EMERALD, GREY}, 
        {ORANGE, PURPLE, RED, BROWN}, 
        {ORANGE, PINK, RED, ORANGE},
        {GREEN, RED, YELLOW, BLUE},
        {YELLOW, GREEN, BROWN, GREEN}, 
        {BROWN, PURPLE, RED, LIME}, 
        {LIME, PURPLE, PINK, LIME}, 
        {EMERALD, GREY, CYAN, BLUE},
        {BROWN, YELLOW, GREY, EMERALD},
        {GREY, YELLOW, LIME, BLUE},
        {EMERALD, CYAN, PINK, ORANGE}, 
        {NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR}, 
        {NO_COLOR, NO_COLOR, NO_COLOR, NO_COLOR}
    };
}
*/

std::string getSystemTimestamp()
{
    constexpr char DAY[][4] = {
        "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
    };

    std::ostringstream oss;

    time_t t = time(nullptr);

    tm now{};

    localtime_s(&now, &t);

    oss << DAY[now.tm_wday] << " " 
        << now.tm_mday << "/" 
        << now.tm_mon + 1 << "/" 
        << now.tm_year + 1900 << "  -  "
        << (now.tm_hour != 0 ? now.tm_hour % 12 : 12) << ":"
        << std::setw(2) << std::setfill('0') << now.tm_min << (now.tm_hour / 12 ? " PM" : " AM");

    return oss.str();
}

int main()
{
    uint64_t memory = 0;      // Number of total nodes stored (frontier + closed set).
    uint64_t examined = 0;    // Number of nodes examined by the BFS algorithm.
    uint64_t duration;        // Duration of BFS runtime in milliseconds.

    std::ofstream ofs("results.txt", std::ios::out);        // File for exporting metrics and solution's path.
    std::ostream& out = (ofs.is_open() ? ofs : std::cout);  // Unless opened successfully, logging is continued at the command line.

    std::chrono::time_point<std::chrono::system_clock> t0;  // Object for recording start time of BFS.
    std::chrono::time_point<std::chrono::system_clock> t1;  // Object for recording stop time of BFS

    State<BOTTLES_N>  start;    // Initial state.
    State<BOTTLES_N>* solution; // Goal State

    bool s_finished = false;


    // Program descripton.
    std::cout << getSystemTimestamp() << "\n\n";
    std::cout << "> Implementation of Water Sort game AI, by Dimitris Yfantidis." << '\n';
    std::cout << "> Currently Running a game of " << BOTTLES_N << " bottles." << '\n';
    std::cout << "> Total size of each state in memory: " << sizeof(State<BOTTLES_N>) << " bytes.\n\n" << std::endl;


#if defined(_MSC_VER)
    std::thread loadingAnimation
    (
        [&] () 
        {
            size_t q;

            std::string gap(13, ' ');
            
            std::chrono::milliseconds ms(20);


            cmd::removeCursor();

            while (!s_finished) 
            {
                for (q = 0; q < cmd::loading_bar.size() && !s_finished; ++q)
                {
                    std::cout << gap << cmd::loading_bar[q];
                    std::this_thread::sleep_for(ms);
                    std::cout << '\r';
                }
            }
            std::cout << std::string(gap.size() + cmd::loading_bar[0].size(), ' ') << std::endl;
        }
    );
#else
    std::cout << "Calculating solution ..." << std::endl;
#endif


    // Initialization of state's bottles with N - 2 random colors (4mL each), in a random sequence.
    start.init();

    t0 = READ_TIME();
    
    solution = BFS(start, examined, memory);

    t1 = READ_TIME();

    duration = MS_DIFF(t0, t1);


    if (solution != nullptr)
    {
        out << "METRICS FOR " << BOTTLES_N << " BOTTLES:\n"
            << "-> Depth:          \t" << solution->getDepth() << '\n'
            << "-> Total Nodes:    \t" << memory << '\n'
            << "-> Examined Nodes: \t" << examined << '\n'
            << "-> Elapsed Time:   \t" << clockFormat(duration)
            << "\n\n" << std::endl;
    }
    else
    {
        /*  The given problem is not actually unsolvable. If BFS() returns null, then that means
         *  there were collisions of hash values, such that paths to all victorious nodes were blocked.
         *  This scenario is very unlikely, but possible.
         */
        out << "Problem unsolvable" << std::endl;
    }

    // Path between start node and goal node.
    std::vector<State<BOTTLES_N>*> path;

    for (State<BOTTLES_N>* s = solution; s != nullptr; s = s->getPrevious()) {
        path.push_back(s);
    }

    // Logging of solution (visually + transitions) from start to finish.
    for (int i = (int)path.size() - 1; i >= 0; --i) {
        out << path[i]->toString() << "\n\n"
            << std::setw(2) << std::setfill(' ') << path.size() - i 
            << ". " << path[i]->getActionName() << "\n\n" << std::endl;
    }

    if (ofs.is_open()) {
        ofs.close();
    }

    s_finished = true;

#if defined(_MSC_VER)
    loadingAnimation.join();
#endif

    return EXIT_SUCCESS;
}
