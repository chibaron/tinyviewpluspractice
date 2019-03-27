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
#include "tvppLap.h"

struct lap rsl[65535];
uint16_t rsl_index = 0;

/////////////////// tvppLap //////////////////////////////////

struct lap* newLap(void)
{
	struct lap *p = &rsl[rsl_index++];
	
	p->time = ofGetTimestampString("%H:%M:%S");
    p->bestLapf = false;
    p->best3Lapf = false;
	return p;
}

