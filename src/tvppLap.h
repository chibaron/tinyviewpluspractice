#pragma once

#include <regex>
#include "ofMain.h"

struct lap {
    int pilotNo;
	string time;
    float lapTime;
    int lapCount;
    int sessonLapCount;
    int sessonCount;
    bool bestLapf;
    bool best3Lapf;
};

struct lap *newLap(void);

