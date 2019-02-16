#pragma once

#include <regex>
#include "ofMain.h"

enum LAPS {
	LAP_NONE,
	LAP_BEST,
	LAP_BEST3
};

struct pilotLapTime {
	string time;
    float lapTime;
    int lapCount;
    int sessonCount;
    bool bestLap;
    bool best3Lap;
};

bool pilotLapCount(int pilotNo, float laptime);
int pilotSearch(string name);
int pilotAdd(string name);
void pilotNextSession(int pilotNo);
int pilotNewSession(string name);
int pilotGetSessonLapCount(int pilotNo);
string pilotGetName(int pilotNo);
float pilotGetBestLap(int pilotNo);
int pilotGetSize(void);
void pilotGetBestLapListInit(void);
void pilotGetBest3LapListInit(void);
string pilotGetListName(int index);
float pilotGetListLap(int index);
int pilotGetListLapNo(int index );
float pilotGetList3Lap(int index);
int pilotGetList3LapNo(int index );
int pilotGetAllLapSize(int pilotNo);
pilotLapTime pilotGetAllLap(int pilotNo, int index );

