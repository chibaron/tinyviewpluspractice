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

extern ofSoundPlayer beepSound, bestSound, setSound;
extern ofxTrueTypeFontUC myFontLabel, myFontLap;
extern ofVideoGrabber grabber[4];
extern int myFontLapWidth;
extern int minLapTime;
extern int setLapTime;
extern int sessonTimeout;



/////////////////// tvppChannel //////////////////////////////////

bool tvppChannel::initSet( int idx, int deviceID )
{
    state = WAIT_DEFAULT;
    index = idx;
    grabber[index].setDeviceID( deviceID );
    if ( grabber[index].initGrabber( CAMERA_WIDTH, CAMERA_HEIGHT ) == false ) {
        return false;
    }

    defaultPilotNo = idx;
    pilotNo = idx;
    prevElapsedSec = -1;
    aruco.setUseHighlyReliableMarker( ARAP_MKR_FILE );
    aruco.setThreaded( true );
    aruco.setup2d( CAMERA_WIDTH, CAMERA_HEIGHT );
    struct channelLap tv = channelLap();
    tv.pilotNo = defaultPilotNo;
    tvew.push_back( tv );
    drawLapTime = 5;
}

bool tvppChannel::newSesson( int no, float elapsedTime )
{
    if ( pilotNo == no && state != ACTIVE_LAP ) {
        return false;
    }
    pilotNo = no;
    if ( pilotNo == defaultPilotNo ) {
        state = WAIT_DEFAULT;
    } else {
        state = WAIT_PILOT;
    }
    prevElapsedSec = elapsedTime;
    if ( tvew[tvew.size() - 1].lapTime != 0 ) {
        struct channelLap tv = channelLap();
        tvew.push_back( tv );
        drawLapTime = 5;
    }
    return true;
}


ofPixels tvppChannel::getImage( void )
{
    return image;
}


bool tvppChannel::update( float elapsedTime )
{
    bool drawCommon = false;
    if ( ( ( elapsedTime - prevElapsedSec ) > sessonTimeout ) && ( state != WAIT_DEFAULT ) ) {
        newSesson( defaultPilotNo, elapsedTime );
        pilotNextSession( pilotNo );
    }
    grabber[index].update();
    if ( grabber[index].isFrameNew() ) {
        drawImage = true;
        image = grabber[index].getPixels();
        // aruco
        aruco.detectMarkers( image );
        // all markers
        int anum = aruco.getNumMarkers();
        if ( anum == 0 ) {
            flickerCount++;
            if ( flickerCount <= 3 ) {
                anum = foundMarkerNum; // anti flicker
            } else {
                flickerCount = 0;
            }
        } else {
            flickerCount = 0;
        }
        // vaild markers
        int vnum = aruco.getNumMarkersValidGate();
        if ( vnum == 0 ) {
            flickerValidCount++;
            if ( flickerValidCount <= 3 ) {
                vnum = foundValidMarkerNum; // anti flicker
            } else {
                flickerValidCount = 0;
            }
        } else {
            flickerValidCount = 0;
        }
        if ( anum == 0 && enoughValidMarkers == true && foundMarkerNum == foundValidMarkerNum ) {
            drawLapTime = 5;
            drawCommon = true;
            if ( state == ACTIVE_LAP ) {
                float lapTime = min( round( ( elapsedTime - prevElapsedSec ) * 100 ) / 100, 999.99f );
                if ( lapTime >= minLapTime ) {
                    float lapTime = min( round( ( elapsedTime - prevElapsedSec ) * 100 ) / 100, 999.99f );
                    bool newbest = pilotLapCount( pilotNo, lapTime );
                    class channelLap tv = channelLap();
                    tv.pilotNo = pilotNo;
                    tv.lapTime = lapTime;
                    tv.lapCount = pilotGetSessonLapCount( pilotNo );
                    tvew.push_back( tv );
                    while( tvew.size() > 100 ) {
                        tvew.erase( tvew.begin() );
                    }
                    speakFunc( LAP_TIME, pilotGetName( pilotNo ), lapTime );
                    if ( newbest ) {
                        bestSound.play();
                    } else if ( lapTime < setLapTime ) {
                        setSound.play();
                    } else {
                        beepSound.play();
                    }
                    prevElapsedSec = elapsedTime;
                }
            } else {
                state = ACTIVE_LAP;
                beepSound.play();
                speakFunc( PILOT_START, pilotGetName( pilotNo ), 0 );
                prevElapsedSec = elapsedTime;
            }
        }
        foundMarkerNum = anum;
        foundValidMarkerNum = vnum;
        if ( anum == 0 ) {
            enoughValidMarkers = false;
        } else if ( vnum >= ARAP_MNUM_THR ) {
            enoughValidMarkers = true;
        }
    }

    return drawCommon;
}

void tvppChannel::draw( float elapsedTime )
{
    // laptime
    if( drawLapTime ) {
        --drawLapTime;
        if( !drawLapTime ) {
            ofSetColor( 0, 0, 0 );
            ofDrawRectangle( posX, posY, width, height );
            // line
            ofSetColor( COLOR_CHANNEL );
            ofDrawRectangle( lineX, lineY, chWidth, LINEHEIGHT );
            // laptime
            ofSetColor( 255, 255, 255 );
            int line = 0;
            for( auto itr = tvew.rbegin(); itr != tvew.rend(); ++itr ) {
                std::stringstream strPilot , strLapTime;
                if ( itr->lapTime ) {
                    strLapTime << std::setw( 5 ) << std::setfill( ' ' ) << std::fixed << setprecision( 2 ) << itr->lapTime;
                    strPilot << pilotGetName( itr->pilotNo ).substr( 0, nameLen ) << "(" << itr->lapCount << ")";
                    ofSetColor( 255, 255, 255 );
                    myFontLap.drawString( strPilot.str(), lapNameX, lapNameY + LAP_STEP * ( line + 1 ) );
                    if ( itr->lapTime == pilotGetBestLap( itr->pilotNo ) ) {
                        ofSetColor( COLOR_BEST );
                    } else if ( itr->lapTime < setLapTime ) {
                        ofSetColor( COLOR_LAP_GREEN );
                    }
                    myFontLap.drawString( strLapTime.str(), lapTimeX, lapTimeY + LAP_STEP * ( line + 1 ) );
                } else {
                    ofSetColor( 255, 255, 255 );
                    myFontLap.drawString( "---", lapNameX, lapNameY + LAP_STEP * ( line + 1 ) );
                }
                if ( ++line >= lapLineMax ) {
                    break;
                }
            }
        }
    }
    // image
    if ( drawImage ) {
        drawImage = false;
        ofSetColor( 255, 255, 255 );
        grabber[index].draw( imageX, imageY, imageWidth, imageHeight );
        // rect
        ofPushMatrix();
        ofTranslate(imageX, imageY);
        ofScale((float)(imageWidth) / (float)CAMERA_WIDTH, (float)(imageWidth) / (float)CAMERA_WIDTH, 1);
        ofSetLineWidth(3);
        aruco.draw2dGate(ofColor(COLOR_YELLOW), ofColor(COLOR_ALERT), false);
        ofPopMatrix();



        ofSetColor( COLOR_YELLOW );
        // pilot
        myFontLabel.drawString( pilotGetName( pilotNo ), imageNameX, imageNameY );
        std::stringstream t;
        // time
        if ( state == ACTIVE_LAP ) {
            t << std::setw( 5 ) << std::setfill( ' ' ) << std::fixed << setprecision( 2 ) << ( elapsedTime - prevElapsedSec );
        } else {
            t << "--.--";
        }
        myFontLap.drawString( t.str(), imageTimeX, imageTimeY );
        // marker
        string lv = "";
        for ( int j = 0; j < foundMarkerNum; j++ ) {
            lv += "|";
        }
        myFontLabel.drawString( lv, imageMarkerX, imageMarkerY );
    }
}

void tvppChannel::window( int iPosX, int iPosY, int iWidth, int iHeight )
{
    posX = iPosX;
    posY = iPosY;
    height = iHeight;
    width = iWidth;
    chWidth = std::min( width - 20, CHANNEL_WIDTH );

    int centerPosX = iPosX + iWidth / 2;
    nameLen = ( ( chWidth -  myFontLapWidth * 11 ) / myFontLapWidth ) & 0xfffe; // "(00)_999.99". and 0xfffe for Japanese
    imageWidth = std::min( chWidth - 100, 200 );
    imageHeight = imageWidth / CAMERA_RATIO;
    imageX = centerPosX - imageWidth / 2;
    imageY = 10 ;
    imageNameX = centerPosX - imageWidth / 2 + 10;
    imageNameY = imageY + 5 + LABEL_STEP;
    imageMarkerX = centerPosX - imageWidth / 2 + 10;
    imageMarkerY = imageY + 5 + LABEL_STEP * 2;
    imageTimeX = centerPosX - myFontLapWidth * 6 / 2;                    // "999.99"
    imageTimeY = imageY + imageHeight - 5;;
    lineX = centerPosX - chWidth / 2;
    lineY = imageY + imageHeight + 10;
    lapNameX = centerPosX - chWidth / 2;
    lapNameY = lineY + 10;
    lapTimeX = centerPosX + chWidth / 2 - myFontLapWidth * 5;
    lapTimeY = lineY + 10;
    lapLineMax = ( iHeight - lineY - 30 ) / LAP_STEP;

    drawLapTime = 5;
}


