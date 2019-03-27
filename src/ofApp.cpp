#include "ofApp.h"
#ifdef TARGET_WIN32
#include <regex>
#include <sapi.h>
#include <atlcomcli.h>
#include <iostream>
#include <cstring>
#include <string>
#include <algorithm>
#include <cctype>
#endif /* TARGET_WIN32 */

#include "tvppSpeech.h"
#include "tvppPilot.h"
#include "tvppChannel.h"

#define DRAW_DELAY  10;

void bindCameras();
void setViewParams();
void changeMinLap();
void changeSetLap();
void changeSessionTimeout();

ofSoundPlayer beepSound, bestSound, setSound;
ofxTrueTypeFontUC myFontLabel, myFontLap;
ofVideoGrabber grabber[4];

int myFontLapWidth;
int minLapTime = 3;
int setLapTime = 10;
int sessonTimeout = 120;

string helpMessage;
int drawDealy;
int width;
int height;
float elapsedTime;

int commonPosX;
int bestlapTitleX;
int bestlapTitleY;
int bestlapLineX;
int bestlapLineY;
int bestlapTimeX;
int bestlapTimeY;
int bestlapNameX;
int bestlapNameY;
int best3lapTitleX;
int best3lapTitleY;
int best3lapLineX;
int best3lapLineY;
int best3lapTimeX;
int best3lapTimeY;
int best3lapNameX;
int best3lapNameY;

int pilotResultTitleY;
int pilotResultLineX;
int pilotResultLineY;
int pilotResultIndexX;
int pilotResultIndexY;
int pilotResultTimeX;
int pilotResultTimeY;
int pilotResultNo;
int bestLapLineMax;
int cursorTab;
int cursorLine;
int cameraNum;

tvppChannel channel[CAMERA_MAXNUM];

/////////////////// QR Code //////////////////////////////////

#ifdef TARGET_WIN32
//--------------------------------------------------------------
string utf8ToAnsi( string utf8 )
{
    int ulen, alen;
    wchar_t* ubuf;
    char* abuf;
    string ansi;

    // utf8 -> wchar
    ulen = MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), utf8.size() + 1, NULL, 0 );
    ubuf = new wchar_t[ulen];
    MultiByteToWideChar( CP_UTF8, 0, utf8.c_str(), utf8.size() + 1, ubuf, ulen );
    // wchar -> ansi
    alen = WideCharToMultiByte( CP_ACP, 0, ubuf, -1, NULL, 0, NULL, NULL );
    abuf = new char[alen];
    WideCharToMultiByte( CP_ACP, 0, ubuf, ulen, abuf, alen, NULL, NULL );
    ansi = abuf;

    delete[] ubuf;
    delete[] abuf;
    return ansi;
}

#endif /* TARGET_WIN32 */
// QRCode
class detectQrcode : public ofThread
{
public:
    void start( void ) {
        startThread();
        return;
    }
    bool startDecode( ofPixels pixels ) {
        if ( !waitNewQrcode ) {
            return false;
        }
        qr_pixels = pixels;
        condition.signal();
        return true;
    }
    string getString( void ) {
        lock();
        string retString = qrString;
        qrString = "";
        unlock();
        return retString;
    }
private:
    void threadedFunction() {
        while( isThreadRunning() ) {
            lock();
            waitNewQrcode = true;
            condition.wait( mutex );
            waitNewQrcode = false;
            unlock();
            ofxZxing::Result zxres;
            zxres = ofxZxing::decode( qr_pixels, true );
            if ( zxres.getFound() ) {
                lock();
                qrString = zxres.getText();
#ifdef TARGET_WIN32
                qrString = utf8ToAnsi( qrString );
#endif /* TARGET_WIN32 */
                unlock();
            }
        }
    }
    ofPixels qr_pixels;
    string qrString;
    Poco::Condition condition;
    bool waitNewQrcode;
};
detectQrcode myDetectQrcode;






//--------------------------------------------------------------
void ofApp::setup()
{
    // system
    ofSetEscapeQuitsApp( false );
    // path
    ofDirectory dir;
    if ( dir.doesDirectoryExist( "../data" ) == false ) {
        // macOS binary release
        ofSetDataPathRoot( "../Resources/data" );
    }
    // help
    helpMessage = ofToString( HELP_MESSAGE );

    // screen
    ofSetWindowTitle( "Tiny View Plus Practice" );
    ofBackground( 0, 0, 0 );
    ofSetBackgroundAuto( false );
    ofSetVerticalSync( VERTICAL_SYNC );
    ofSetFrameRate( FRAME_RATE );
    myFontLabel.load( FONT_P_FILE, LABEL_HEIGHT );
    myFontLap.load( FONT_M_FILE, LAP_HEIGHT );
    myFontLapWidth = myFontLap.stringWidth( "A" );
    // camera
    bindCameras();
    setViewParams();
    beepSound.load( SND_BEEP_FILE );
    bestSound.load( SND_BEST_FILE );
    setSound.load( SND_SET_FILE );

    myDetectQrcode.start();
    cursorTab = -1;

}


//--------------------------------------------------------------
void ofApp::update()
{
    static int qrIndex = 0;
    static int qrIndexDecode = 0;
    static int qrUpdCount = 0;

    // timer
    elapsedTime = ofGetElapsedTimef();
    for( int idx = 0; idx < cameraNum; idx++ ) {
        if( channel[idx].update( elapsedTime ) ) {
            drawDealy = DRAW_DELAY;
        }
    }
    // QR Code
    string decodeString = myDetectQrcode.getString();
    if ( decodeString != "" ) {
        int pilotNo;
        if( ( pilotNo = pilotSearch( decodeString ) ) < 0 ) {
            pilotNo = pilotAdd( decodeString );
        }
        if ( channel[qrIndexDecode].newSesson( pilotNo, elapsedTime ) ) {
            pilotNextSession( pilotNo );
            speakFunc( PILOT_ENTRY, decodeString, 0 );
        }
    }
    if ( ++qrUpdCount >= QR_CYCLE ) {
        qrUpdCount = 0;
        if ( myDetectQrcode.startDecode( channel[qrIndex].getImage() ) ) {
            qrIndexDecode = qrIndex;
            qrIndex = ( qrIndex + 1 ) % cameraNum;
        }
    }
}


void drawBestLap()
{

    int idx;
    ofSetColor( 0, 0, 0 );
    ofDrawRectangle( commonPosX - COMMON_WIDTH / 2, 0, COMMON_AREA, height / 3 );
    ofSetColor( 255, 255, 255 );
    myFontLap.drawString( "BestLap", commonPosX + bestlapTitleX, bestlapTitleY );
    // line
    if ( cursorTab == 0 ) {
        ofSetColor( COLOR_BEST );
        ofDrawRectangle( commonPosX + bestlapLineX, bestlapLineY - LINEHEIGHT, COMMON_WIDTH, LINEHEIGHT * 3 );
        ofSetColor( 255, 255, 255 );
        ofDrawRectangle( commonPosX + bestlapLineX + 10, bestlapLineY, COMMON_WIDTH - 20, LINEHEIGHT );
        cursorLine = std::min( cursorLine, ( int )( pilotGetSize() - bestLapLineMax ) );
        cursorLine = std::max( cursorLine, 0 );
        idx = cursorLine;
    } else {
        ofSetColor( COLOR_BEST );
        ofDrawRectangle( commonPosX + bestlapLineX, bestlapLineY, COMMON_WIDTH, LINEHEIGHT );
        idx = 0;
    }
    pilotGetBestLapListInit();
    ofSetColor( 255, 255, 255 );
    for( int line = 0; line < bestLapLineMax && idx < pilotGetSize(); line++ ) {
        std::stringstream strPilot, strLapTime;
        strLapTime << std::setw( 6 ) << std::setfill( ' ' ) << std::fixed << setprecision( 2 ) << pilotGetListLap( idx );
        strLapTime << "(";
        strLapTime << pilotGetListLapNo( idx );
        strLapTime << ")";
        strPilot << pilotGetListName( idx );
        myFontLap.drawString( strLapTime.str(), commonPosX + bestlapTimeX, bestlapTimeY + LAP_STEP * ( line + 1 ) );
        myFontLap.drawString( strPilot.str(), commonPosX + bestlapNameX, bestlapNameY + LAP_STEP * ( line + 1 ) );
        idx++;
    }
}

void drawBest3Lap()
{

    int idx;
    ofSetColor( 0, 0, 0 );
    ofDrawRectangle( commonPosX - COMMON_WIDTH / 2, height / 3, COMMON_AREA, height / 3 );
    ofSetColor( 255, 255, 255 );
    myFontLap.drawString( "Best3Lap", commonPosX + best3lapTitleX, best3lapTitleY );
    // line
    if ( cursorTab == 1 ) {
        ofSetColor( COLOR_BEST3 );
        ofDrawRectangle( commonPosX +    best3lapLineX, best3lapLineY - LINEHEIGHT, COMMON_WIDTH, LINEHEIGHT * 3 );
        ofSetColor( 255, 255, 255 );
        ofDrawRectangle( commonPosX + best3lapLineX + 10, best3lapLineY, COMMON_WIDTH - 20, LINEHEIGHT );
        cursorLine = std::min( cursorLine, ( int )( pilotGetSize() - bestLapLineMax ) );
        cursorLine = std::max( cursorLine, 0 );
        idx = cursorLine;
    } else {
        ofSetColor( COLOR_BEST3 );
        ofDrawRectangle( commonPosX + best3lapLineX, best3lapLineY, COMMON_WIDTH, LINEHEIGHT );
        idx = 0;
    }
    pilotGetBest3LapListInit();
    ofSetColor( 255, 255, 255 );
    for( int line = 0; line < bestLapLineMax && idx < pilotGetSize(); line++ ) {
        std::stringstream strPilot, strLapTime;
        strLapTime << std::setw( 7 ) << std::setfill( ' ' ) << std::fixed << setprecision( 2 ) << pilotGetList3Lap( idx );
        strLapTime << "(";
        strLapTime << pilotGetList3LapNo( idx );
        strLapTime << ")";
        strPilot << pilotGetListName( idx );
        myFontLap.drawString( strLapTime.str(), commonPosX + best3lapTimeX, best3lapTimeY + LAP_STEP * ( line + 1 ) );
        myFontLap.drawString( strPilot.str(), commonPosX + best3lapNameX, best3lapNameY + LAP_STEP * ( line + 1 ) );
        idx++;
    }
}

void drawPilotResult()
{

    int idx;
    ofSetColor( 0, 0, 0 );
    ofDrawRectangle( commonPosX - COMMON_WIDTH / 2, ( height / 3 ) * 2, COMMON_AREA, height / 3 );
    ofSetColor( 255, 255, 255 );
    myFontLap.drawString( pilotGetName( pilotResultNo ), commonPosX - ( myFontLap.stringWidth( pilotGetName( pilotResultNo ) ) / 2 ), pilotResultTitleY );
    // line
    ofSetColor( COLOR_PILOT );
    if ( cursorTab == 2 ) {
        ofDrawRectangle( commonPosX + pilotResultLineX, pilotResultLineY - LINEHEIGHT, COMMON_WIDTH, LINEHEIGHT * 3 );
        ofSetColor( 255, 255, 255 );
        ofDrawRectangle( commonPosX + pilotResultLineX + 10, pilotResultLineY, COMMON_WIDTH - 20, LINEHEIGHT );
        cursorLine = std::max( cursorLine, bestLapLineMax - 1 );
        cursorLine = std::min( cursorLine, pilotGetAllLapSize( pilotResultNo ) - 1 );
        idx = cursorLine;
    } else {
        ofDrawRectangle( commonPosX + pilotResultLineX, pilotResultLineY, COMMON_WIDTH, LINEHEIGHT );
        idx = pilotGetAllLapSize( pilotResultNo ) - 1;
    }
    ofSetColor( 255, 255, 255 );

    if( pilotGetAllLapSize( pilotResultNo ) > 0 ) {
        for( int line = 0; line < bestLapLineMax && idx >= 0; line++ ) {
            struct lap *lap = pilotGetAllLap( pilotResultNo, idx );
            std::stringstream strIndex, strLapTime;
            strIndex << lap->time;
            strIndex << " ";
            strIndex << std::setw( 2 ) << std::setfill( ' ' ) << lap->sessonCount;
            strIndex << "-";
            strIndex << std::setw( 2 ) << std::setfill( ' ' ) << lap->sessonLapCount;
            strIndex << " ";
            strIndex << std::setw( 3 ) << std::setfill( ' ' ) << lap->lapCount;
            strLapTime << std::setw( 5 ) << std::setfill( ' ' ) << std::fixed << setprecision( 2 ) << lap->lapTime;
            ofSetColor( 255, 255, 255 );
            myFontLap.drawString( strIndex.str(), commonPosX + pilotResultIndexX, pilotResultIndexY + LAP_STEP * ( line + 1 ) );
            if ( lap->bestLapf ) {
                ofSetColor( COLOR_BEST );
            } else if ( lap->best3Lapf ) {
                ofSetColor( COLOR_BEST3 );
            } else if ( lap->lapTime < setLapTime ) {
                ofSetColor( COLOR_LAP_GREEN );
            }
            myFontLap.drawString( strLapTime.str(), commonPosX + pilotResultTimeX, pilotResultTimeY + LAP_STEP * ( line + 1 ) );
            idx--;
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    for( int idx = 0; idx < cameraNum; idx++ ) {
        channel[idx].draw( elapsedTime );
    }
    if ( drawDealy ) {
        drawDealy--;
        if ( !drawDealy ) {
            drawBestLap();
            drawBest3Lap();
            drawPilotResult();
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed( int key )
{
    switch ( key ) {
    case 0xe00e:    /* right */
        if( cursorTab == 2 ) {
            if ( ++pilotResultNo >= pilotGetSize() ) {
                pilotResultNo = 0;
            }
            drawDealy = DRAW_DELAY;
        }
        break;
    case 0xe00c:    /* left */
        if( cursorTab == 2 ) {
            if ( --pilotResultNo < 0 ) {
                pilotResultNo = pilotGetSize() - 1;
            }
            drawDealy = DRAW_DELAY;
        }
        break;
    case 0xe00d:    /* up */
        cursorLine++;
        drawDealy = DRAW_DELAY;
        break;
    case 0xe00f:    /* down */
        cursorLine--;
        drawDealy = DRAW_DELAY;
        break;
    case 0x1b:  /* esc */
        cursorTab = -1;
        drawDealy = DRAW_DELAY;
        break;
    case '\t':  /* tab */
        cursorTab = ( cursorTab + 1 ) % 3;
        cursorLine = 0;
        drawDealy = DRAW_DELAY;
        break;
    case 'm':
    case 'M':
        changeMinLap();
        break;
    case 'a':
    case 'A':
        changeSetLap();
        break;
    case 's':
    case 'S':
        changeSessionTimeout();
        break;
    case 'h':
    case 'H':
        ofSystemAlertDialog( helpMessage );
        break;
    case '.':
        ofSystemAlertDialog( "Exit application" );
        ofExit();
        break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased( int key )
{

}

//--------------------------------------------------------------
void ofApp::mouseMoved( int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged( int x, int y, int button )
{

}

//--------------------------------------------------------------
void ofApp::mousePressed( int x, int y, int button )
{

}

//--------------------------------------------------------------
void ofApp::mouseReleased( int x, int y, int button )
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered( int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseExited( int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::windowResized( int w, int h )
{
    // view
    setViewParams();
}

//--------------------------------------------------------------
void ofApp::gotMessage( ofMessage msg )
{

}

//--------------------------------------------------------------
void ofApp::dragEvent( ofDragInfo dragInfo )
{

}

//--------------------------------------------------------------
void changeMinLap()
{
    string str;
    int lap;
    str = ofToString( minLapTime );
    str = ofSystemTextBoxDialog( "Min. lap time (1~" + ofToString( ARAP_MAX_MNLAP ) + "sec):", str );
    lap = ( str == "" ) ? 0 : ofToInt( str );
    if ( lap > 0 && lap <= ARAP_MAX_MNLAP ) {
        minLapTime = lap;
    } else {
        ofSystemAlertDialog( "Please enter 1~" + ofToString( ARAP_MAX_MNLAP ) );
        changeMinLap();
    }
}

//--------------------------------------------------------------
void changeSetLap()
{
    string str;
    int lap;
    str = ofToString( setLapTime );
    str = ofSystemTextBoxDialog( "Set. lap time (1~" + ofToString( ARAP_MAX_MNLAP ) + "sec):", str );
    lap = ( str == "" ) ? 0 : ofToInt( str );
    if ( lap > 0 && lap <= ARAP_MAX_MNLAP ) {
        setLapTime = lap;
    } else {
        ofSystemAlertDialog( "Please enter 1~" + ofToString( ARAP_MAX_MNLAP ) );
        changeSetLap();
    }
}

//--------------------------------------------------------------
void changeSessionTimeout()
{
    string str;
    int lap;
    str = ofToString( minLapTime );
    str = ofSystemTextBoxDialog( "Set. sesson timeout (1~" + ofToString( sessonTimeout ) + "sec):", str );
    lap = ( str == "" ) ? 0 : ofToInt( str );
    if ( lap > 0 && lap <= ARAP_SESSON_TIMEOUT ) {
        sessonTimeout = lap;
    } else {
        ofSystemAlertDialog( "Please enter 1~" + ofToString( ARAP_MAX_MNLAP ) );
        changeSessionTimeout();
    }
}

//--------------------------------------------------------------
void bindCameras()
{

    ofVideoGrabber grabber;
    vector<ofVideoDevice> devices = grabber.listDevices();
    cameraNum = 0;
    for ( vector<ofVideoDevice>::iterator it = devices.begin(); it != devices.end(); ++it ) {
//        if (regex_search(it->deviceName, regex("USB2.0 PC CAMERA")) == true
//            || regex_search(it->deviceName, regex("GV-USB2")) == true) {
            if ( cameraNum < CAMERA_MAXNUM ) {
                if ( channel[cameraNum].initSet( cameraNum, it->id ) ) {
                    pilotNextSession( pilotAdd( "Pilot" + ofToString( cameraNum + 1 ) ) );
                    cameraNum++;
                }
            }
            if ( cameraNum == CAMERA_MAXNUM ) {
                break;
            }
//        }
    }
    if ( cameraNum == 0 ) {
        ofSystemAlertDialog( "no FPV receiver detected" );
        ofExit();
    }
}






//--------------------------------------------------------------
void setViewParams()
{

    width = ofGetWidth();
    height = ofGetHeight();

    int posXstep = ( width - COMMON_AREA ) / cameraNum;
    //int posXstep = (width - COMMON_AREA)/3;                        // debug

    int posx = 0;
    for( int idx = 0; idx < cameraNum; idx++ ) {
        channel[idx].window( posx, 0, posXstep, height );
        posx += posXstep;
    }

    bestlapTitleX = - ( myFontLapWidth * 7 ) / 2; // "BestLap"
    bestlapTitleY = 10 + LAP_STEP;
    bestlapLineX = - COMMON_WIDTH / 2;
    bestlapLineY = bestlapTitleY + 10;
    bestlapTimeX = - COMMON_WIDTH / 2;
    bestlapTimeY = bestlapLineY + 10;
    bestlapNameX = - COMMON_WIDTH / 2 + myFontLapWidth * 12; // "999.99(xxx)_"
    bestlapNameY = bestlapLineY + 10;

    best3lapTitleX = - ( myFontLapWidth * 8 ) / 2; // "Best3Lap"
    best3lapTitleY = height / 3 + LAP_STEP + 10;
    best3lapLineX = - COMMON_WIDTH / 2;
    best3lapLineY = best3lapTitleY + 10;
    best3lapTimeX = - COMMON_WIDTH / 2;
    best3lapTimeY = best3lapLineY + 10;
    best3lapNameX = - COMMON_WIDTH / 2 + myFontLapWidth * 12; // "999.99(xxx)_"
    best3lapNameY = best3lapLineY + 10;

    pilotResultTitleY = ( height / 3 ) * 2 + LAP_STEP + 10 ;
    pilotResultLineX = - COMMON_WIDTH / 2;
    pilotResultLineY = pilotResultTitleY + 10;
    pilotResultIndexX = - COMMON_WIDTH / 2;
    pilotResultIndexY = pilotResultLineY + 10;
    pilotResultTimeX = - COMMON_WIDTH / 2 + myFontLapWidth * 20;
    pilotResultTimeY = pilotResultLineY + 10;

    commonPosX = width - COMMON_AREA / 2;
    bestLapLineMax = ( height / 3 - 75 ) / LAP_STEP;

    drawDealy = DRAW_DELAY;

}

