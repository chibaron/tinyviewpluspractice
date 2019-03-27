#pragma once

#include <regex>
#include "ofMain.h"

#define CHANNEL_WIDTH     400
#define CAMERA_RATIO    1.3333
#define CAMERA_WIDTH    640
#define CAMERA_HEIGHT   480

enum CHANNEL_STATE {
    WAIT_DEFAULT,
    WAIT_PILOT,
    ACTIVE_LAP
};

class tvppChannel
{
public:
    bool initSet( int idx, int deviceID );
    bool newSesson( int no, float elapsedTime );
    ofPixels getImage( void );
    bool update( float elapsedTime );
    void draw( float elapsedTime );
    void window( int iPosX, int iPosY, int iWidth, int iHeight );

    enum CHANNEL_STATE state;
    int defaultPilotNo;
    int index;
    int pilotNo;
    ofPixels image;
    // AR lap timer
    ofxAruco aruco;
    int foundMarkerNum;
    int foundValidMarkerNum;
    bool enoughValidMarkers;
    float prevElapsedSec;
    vector<struct lap*> tvew;
    int flickerCount;
    int flickerValidCount;
    bool drawImage;
    int drawLapTime;

    // drawSetting
    int posX;
    int posY;
    int height;
    int width;
    int chWidth;
    int nameLen;
    int imageX;
    int imageY;
    int imageHeight;
    int imageWidth;
    int imageNameX;
    int imageNameY;
    int imageMarkerX;
    int imageMarkerY;
    int imageTimeX;
    int imageTimeY;
    int lineX;
    int lineY;
    int lapNameX;
    int lapNameY;
    int lapTimeX;
    int lapTimeY;
    int lapLineMax;

//    ofVideoGrabber grabber;

};

