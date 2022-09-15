#pragma once

#include <string>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

#ifdef _WIN32

#include <windows.h>

namespace cmd
{
    void removeCursor()
    {
        HANDLE hStdOut = NULL;

        CONSOLE_CURSOR_INFO curInfo;
        
        hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

        GetConsoleCursorInfo(hStdOut, &curInfo);

        curInfo.bVisible = FALSE;

        SetConsoleCursorInfo(hStdOut, &curInfo);
    }
}
#else
namespace cmd
{
    void removeCursor() {}
}
#endif


// Auxiliary function for printing a duration in milliseconds in (00h : 00m : 00s : 000ms) format.
std::string clockFormat(unsigned long long milliseconds)
{
    std::ostringstream oss;

    unsigned long long seconds;
    unsigned long long minutes;
    unsigned long long hours;

    seconds = milliseconds / 1000;
    milliseconds %= 1000;
    minutes = seconds / 60;
    seconds %= 60;
    hours = minutes / 60;
    minutes %= 60;

    oss << std::setw(2) << std::setfill('0') << hours << "h : "
        << std::setw(2) << std::setfill('0') << minutes << "m : "
        << std::setw(2) << std::setfill('0') << seconds << "s : "
        << std::setw(3) << std::setfill('0') << milliseconds << "ms";

    return oss.str();
}
namespace cmd 
{
    std::vector<std::string> loading_bar =
    {
                "[                                      ]",
                "[#                                     ]",
                "[##                                    ]",
                "[###                                   ]",
                "[####                                  ]",
                "[#####                                 ]",
                "[######                                ]",
                "[#######                               ]",
                "[########                              ]",
                "[#########                             ]",
                "[##########                            ]",
                "[###########                           ]",
                "[############                          ]",
                "[#############                         ]",
                "[ #############                        ]",
                "[  #############                       ]",
                "[   #############                      ]",
                "[    #############                     ]",
                "[     #############                    ]",
                "[      #############                   ]",
                "[       #############                  ]",
                "[        #############                 ]",
                "[         #############                ]",
                "[          #############               ]",
                "[           #############              ]",
                "[            #############             ]",
                "[             #############            ]",
                "[              #############           ]",
                "[               #############          ]",
                "[                #############         ]",
                "[                 #############        ]",
                "[                  #############       ]",
                "[                   #############      ]",
                "[                    #############     ]",
                "[                     #############    ]",
                "[                      #############   ]",
                "[                       #############  ]",
                "[                        ############# ]",
                "[                         #############]",
                "[                          ############]",
                "[                           ###########]",
                "[                            ##########]",
                "[                             #########]",
                "[                              ########]",
                "[                               #######]",
                "[                                ######]",
                "[                                 #####]",
                "[                                  ####]",
                "[                                   ###]",
                "[                                    ##]",
                "[                                     #]",
    };

    void setLoadingBarChar(char c)
    {
        for (std::string& snapshot : loading_bar) {
            std::replace(snapshot.begin(), snapshot.end(), '#', c);
        }
    }
}
