#pragma once

#include <regex>
#include "ofMain.h"

enum SPEAK {
	PILOT_ENTRY,
	PILOT_START,
	LAP_TIME
};


void speakFunc(enum SPEAK speak, string text, float lap);
void speakAny(string lang, string text);


