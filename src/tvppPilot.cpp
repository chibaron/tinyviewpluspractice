#pragma once

#include <regex>
#include "ofMain.h"

#include "tvppPilot.h"


struct timeView {
	string time;
    float lapTime;
    int lapCount;
    int sessonCount;
};

class pilot {
public:
    pilot(string strPilotName);
    bool lapCount(float laptime);
    void nextSession(void);
    float best3Lap(void);

	string name;
	vector<struct timeView> allLaps;
	int sessonCount;
    int sessonLapCount;
	float bestLap;
	int bestLapNo;
	float best3lap[3];
	int best3LapNo;
};
vector<class pilot> pilots;
vector<class pilot> tmpList;

/////////////////////////////////////////////////////////////
pilot::pilot(string strPilotName){
        name = strPilotName;
        bestLap = 999.99;
		best3lap[0] = 999.99;
		best3lap[1] = 999.99;
		best3lap[2] = 999.99;
		bestLapNo = 0;
		best3LapNo = 0;
        sessonCount = 1;
    	sessonLapCount = 0;
}

/////////////////////////////////////////////////////////////
bool pilot::lapCount(float laptime)
{
    bool newbest = false;

    timeView tv;
    tv.time = ofGetTimestampString("%H:%M:%S");
    tv.lapTime = laptime;
    tv.lapCount = ++sessonLapCount;
    tv.sessonCount = sessonCount;
    allLaps.push_back(tv);
    // bestlap
    if (bestLap > laptime){
        bestLap = laptime;
		bestLapNo = allLaps.size();
        newbest = true;
    }
    // best3lap
	if (allLaps.size() >= 3) {
        int idx = allLaps.size() - 3;
        if ((allLaps[idx].sessonCount == allLaps[idx+1].sessonCount) && (allLaps[idx].sessonCount == allLaps[idx+2].sessonCount)) {
            if ( best3Lap() > ( allLaps[idx].lapTime + allLaps[idx+1].lapTime + allLaps[idx+2].lapTime)){
                best3lap[0] = allLaps[idx].lapTime;
                best3lap[1] = allLaps[idx+1].lapTime;
                best3lap[2] = allLaps[idx+2].lapTime;
                best3LapNo = idx + 1;
            }
        }
    }
	return newbest;
}

/////////////////////////////////////////////////////////////
void pilot::nextSession(void){
        if (sessonLapCount > 0){
        	sessonCount++;
        	sessonLapCount = 0;
        }
}

/////////////////////////////////////////////////////////////
float pilot::best3Lap(void){
        return best3lap[0] + best3lap[1] + best3lap[2];
}

/////////////////////////////////////////////////////////////
bool compareBestLap(pilot& left, pilot& right){
    return left.bestLap < right.bestLap ;
}

/////////////////////////////////////////////////////////////
bool compareBest3Lap(pilot& left, pilot& right){
    return ( left.best3Lap() < right.best3Lap() );
}



/////////////////////////////////////////////////////////////
bool pilotLapCount(int pilotNo, float laptime)
{
    return pilots[pilotNo].lapCount(laptime);
}

/////////////////////////////////////////////////////////////
int pilotSearch(string name)
{
    for(int no=0; no<pilots.size(); no++){
        if (pilots[no].name == name){
            return no;
        }
    }
    return -1;
}

/////////////////////////////////////////////////////////////
int pilotAdd(string name)
{
    class pilot newPilot(name);
    pilots.push_back(newPilot);
    return pilots.size() -1;
}

/////////////////////////////////////////////////////////////
void pilotNextSession(int pilotNo)
{
    pilots[pilotNo].nextSession();
}


/////////////////////////////////////////////////////////////
int pilotNewSession(string name)
{
    class pilot newPilot(name);
    pilots.push_back(newPilot);
    return pilots.size();
}


/////////////////////////////////////////////////////////////
int pilotGetSessonLapCount(int pilotNo)
{
    return pilots[pilotNo].sessonLapCount;
}

/////////////////////////////////////////////////////////////
string pilotGetName(int pilotNo)
{
    return pilots[pilotNo].name;
}

/////////////////////////////////////////////////////////////
float pilotGetBestLap(int pilotNo)
{
    return pilots[pilotNo].bestLap;
}

/////////////////////////////////////////////////////////////
int pilotGetSize(void)
{
    return pilots.size();
}

/////////////////////////////////////////////////////////////
void pilotGetBestLapListInit(void)
{
    tmpList = pilots;
	std::sort(tmpList.begin(), tmpList.end(), compareBestLap);;
}

/////////////////////////////////////////////////////////////
void pilotGetBest3LapListInit(void)
{
    tmpList = pilots;
	std::sort(tmpList.begin(), tmpList.end(), compareBest3Lap);;
}

/////////////////////////////////////////////////////////////
string pilotGetListName(int index)
{
    return tmpList[index].name;
}
    
/////////////////////////////////////////////////////////////
float pilotGetListLap(int index)
{
    return tmpList[index].bestLap;
}

/////////////////////////////////////////////////////////////
int pilotGetListLapNo(int index )
{
    return tmpList[index].bestLapNo;
}


/////////////////////////////////////////////////////////////
float pilotGetList3Lap(int index)
{
    return tmpList[index].best3Lap();
}

/////////////////////////////////////////////////////////////
int pilotGetList3LapNo(int index )
{
    return tmpList[index].best3LapNo;
}

/////////////////////////////////////////////////////////////
int pilotGetAllLapSize(int pilotNo)
{
    return pilots[pilotNo].allLaps.size();
}

/////////////////////////////////////////////////////////////
pilotLapTime pilotGetAllLap(int pilotNo, int index )
{
    pilotLapTime lap;
	lap.time = pilots[pilotNo].allLaps[index].time;
	lap.lapTime = pilots[pilotNo].allLaps[index].lapTime;
	lap.lapCount = pilots[pilotNo].allLaps[index].lapCount;
	lap.sessonCount = pilots[pilotNo].allLaps[index].sessonCount;
    lap.bestLap = (pilots[pilotNo].allLaps[index].lapTime == pilots[pilotNo].bestLap);
	lap.best3Lap = (pilots[pilotNo].best3LapNo <= lap.lapCount) && ((pilots[pilotNo].best3LapNo + 2) >= lap.lapCount);
    return lap;
}


