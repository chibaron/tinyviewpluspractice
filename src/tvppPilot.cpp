#include <regex>
#include "ofMain.h"
#include "tvppPilot.h"



class pilot
{
public:
    pilot( string strPilotName );
    void newLapCount(struct lap *lap);
    void nextSession( void );
    float bestLap( void );
    float best3Lap( void );

    string name;
	vector<struct lap*> allLaps;
	int lapCount;
    int sessonCount;
    int sessonLapCount;
	int bestLapIndex;
	int best3LapIndex;
};
vector<class pilot> pilots;
vector<class pilot> tmpList;

/////////////////////////////////////////////////////////////
pilot::pilot( string strPilotName )
{
    name = strPilotName;
        lapCount = 0;
    sessonCount = 1;
    sessonLapCount = 0;
    bestLapIndex = -1;
    best3LapIndex = -1;
}

/////////////////////////////////////////////////////////////
void pilot::newLapCount(struct lap *lap)
{
    lap->lapCount = ++lapCount;
    lap->sessonLapCount = ++sessonLapCount;
    lap->sessonCount = sessonCount;
    allLaps.push_back(lap);
    // bestlap
    if (bestLap() > allLaps[allLaps.size()-1]->lapTime){
        if (bestLapIndex >= 0){
            allLaps[bestLapIndex]->bestLapf = false;
        }
        bestLapIndex = allLaps.size() - 1;
        allLaps[bestLapIndex]->bestLapf = true;
    }
    // best3lap
    if ( allLaps.size() >= 3 ) {
        int idx = allLaps.size() - 3;
        if ((allLaps[idx]->sessonCount == allLaps[idx+1]->sessonCount) && (allLaps[idx]->sessonCount == allLaps[idx+2]->sessonCount)) {
            float tmplap = allLaps[idx]->lapTime + allLaps[idx+1]->lapTime + allLaps[idx+2]->lapTime;
            if ( best3Lap() > tmplap ){
                if ( best3LapIndex >= 0 ){
                    allLaps[best3LapIndex]->best3Lapf = false;
                    allLaps[best3LapIndex+1]->best3Lapf = false;
                    allLaps[best3LapIndex+2]->best3Lapf = false;
                }
                best3LapIndex = idx;
                allLaps[best3LapIndex]->best3Lapf = true;
                allLaps[best3LapIndex+1]->best3Lapf = true;
                allLaps[best3LapIndex+2]->best3Lapf = true;
            }
        }
    }
}

/////////////////////////////////////////////////////////////
void pilot::nextSession( void )
{
    if ( sessonLapCount > 0 ) {
        sessonCount++;
        sessonLapCount = 0;
    }
}

/////////////////////////////////////////////////////////////
float pilot::bestLap(void){
    if (bestLapIndex < 0){
        return 999.99;
    }
    return allLaps[bestLapIndex]->lapTime;
}

/////////////////////////////////////////////////////////////
float pilot::best3Lap(void){
        if (best3LapIndex < 0){
            return 999.99;
        }
        return allLaps[best3LapIndex]->lapTime + allLaps[best3LapIndex+1]->lapTime + allLaps[best3LapIndex+2]->lapTime;
}

/////////////////////////////////////////////////////////////
bool compareBestLap( pilot& left, pilot& right )
{
    return left.bestLap() < right.bestLap();
}

/////////////////////////////////////////////////////////////
bool compareBest3Lap( pilot& left, pilot& right )
{
    return ( left.best3Lap() < right.best3Lap() );
}



/////////////////////////////////////////////////////////////
void pilotLapCount(int pilotNo, struct lap *lap)
{
    pilots[pilotNo].newLapCount(lap);
}

/////////////////////////////////////////////////////////////
int pilotSearch( string name )
{
    for( int no = 0; no < pilots.size(); no++ ) {
        if ( pilots[no].name == name ) {
            return no;
        }
    }
    return -1;
}

/////////////////////////////////////////////////////////////
int pilotAdd( string name )
{
    class pilot newPilot( name );
    pilots.push_back( newPilot );
    return pilots.size() - 1;
}

/////////////////////////////////////////////////////////////
void pilotNextSession( int pilotNo )
{
    pilots[pilotNo].nextSession();
}


/////////////////////////////////////////////////////////////
int pilotNewSession( string name )
{
    class pilot newPilot( name );
    pilots.push_back( newPilot );
    return pilots.size();
}


/////////////////////////////////////////////////////////////
int pilotGetSessonLapCount( int pilotNo )
{
    return pilots[pilotNo].sessonLapCount;
}

/////////////////////////////////////////////////////////////
string pilotGetName( int pilotNo )
{
    return pilots[pilotNo].name;
}

/////////////////////////////////////////////////////////////
float pilotGetBestLap( int pilotNo )
{
	return pilots[pilotNo].bestLap();
}

/////////////////////////////////////////////////////////////
int pilotGetSize( void )
{
    return pilots.size();
}

/////////////////////////////////////////////////////////////
void pilotGetBestLapListInit( void )
{
    tmpList = pilots;
    std::sort( tmpList.begin(), tmpList.end(), compareBestLap );;
}

/////////////////////////////////////////////////////////////
void pilotGetBest3LapListInit( void )
{
    tmpList = pilots;
    std::sort( tmpList.begin(), tmpList.end(), compareBest3Lap );;
}

/////////////////////////////////////////////////////////////
string pilotGetListName( int index )
{
    return tmpList[index].name;
}

/////////////////////////////////////////////////////////////
float pilotGetListLap( int index )
{
	return tmpList[index].bestLap();
}

/////////////////////////////////////////////////////////////
int pilotGetListLapNo( int index )
{
    if (tmpList[index].bestLapIndex < 0){
        return 0;
    }
    return tmpList[index].bestLapIndex+1;
}


/////////////////////////////////////////////////////////////
float pilotGetList3Lap( int index )
{
    return tmpList[index].best3Lap();
}

/////////////////////////////////////////////////////////////
int pilotGetList3LapNo( int index )
{
    if (tmpList[index].best3LapIndex < 0){
        return 0;
    }
    return tmpList[index].best3LapIndex+1;
}

/////////////////////////////////////////////////////////////
int pilotGetAllLapSize( int pilotNo )
{
    return pilots[pilotNo].allLaps.size();
}

/////////////////////////////////////////////////////////////
struct lap *pilotGetAllLap( int pilotNo, int index )
{
    return pilots[pilotNo].allLaps[index];
}


