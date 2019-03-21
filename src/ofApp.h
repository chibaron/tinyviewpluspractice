#pragma once

#include <regex>
#include "ofMain.h"
#include "ofxTrueTypeFontUC.h"
#include "ofxOsc.h"
#include "ofxAruco.h"
#include "ofxZxing.h"
#include "highlyreliablemarkers.h"

/* ---------- definitions ---------- */

// system
#define APP_INFO        "Tiny View Plus Practice v0.0.2"
#define COLOR_YELLOW    255,215,0
#define COLOR_CHANNEL   160,194,56
#define COLOR_BEST      201,58,64
#define COLOR_BEST3     0,116,191
#define COLOR_PILOT     248,128,23
#define COLOR_LAP_GREEN 50, 200, 50
#define COLOR_ALERT     255,0,0
#define LINEHEIGHT      3
#define LABEL_HEIGHT    15
#define LABEL_STEP      (LABEL_HEIGHT+4)
#define LAP_HEIGHT      21
#define LAP_STEP        (LAP_HEIGHT +7)
#define FRAME_RATE      60
#define VERTICAL_SYNC   true
#define FONT_P_FILE     "system/mplus-1p-bold.ttf"
#define FONT_M_FILE     "system/mplus-1m-bold.ttf"
// channel
#define CAMERA_MAXNUM   4
// common
#define COMMON_AREA     400
#define COMMON_WIDTH    (COMMON_AREA-20)
#define COMMON_POSX     (width - COMMON_AREA/2)
// AR lap timer
#define SND_BEEP_FILE   "system/beep.wav"
#define SND_BEST_FILE   "system/beepBest.wav"
#define SND_SET_FILE    "system/beepSet.wav"
#define ARAP_MKR_FILE   "system/marker.xml"
#define ARAP_MNUM_THR   2
#define SPCH_SLOT_NUM   8

#define ARAP_MAX_MNLAP  100
#define ARAP_SESSON_TIMEOUT  900
#define QR_CYCLE        16


#define HELP_MESSAGE    APP_INFO"\n\n"\
                        "Keyboard shortcuts:\n"\
                        "[A] lap time (1~100sec)\n"\
                        "[M] minimum lap time (1~100sec)\n"\
                        "[S] sesson timeout (1~900sec)\n"\
                        "[Tab] change Tab\n"\
                        "[Up,Down,Right,Left] \n"\
                        "[.] Exit application"



class ofApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();
    void keyPressed( int key );
    void keyReleased( int key );
    void mouseMoved( int x, int y );
    void mouseDragged( int x, int y, int button );
    void mousePressed( int x, int y, int button );
    void mouseReleased( int x, int y, int button );
    void mouseEntered( int x, int y );
    void mouseExited( int x, int y );
    void windowResized( int w, int h );
    void dragEvent( ofDragInfo dragInfo );
    void gotMessage( ofMessage msg );
};
