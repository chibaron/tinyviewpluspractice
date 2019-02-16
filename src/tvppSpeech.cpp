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

HRESULT                        cpVicehr = E_NOINTERFACE;
CComPtr<ISpVoice>              cpVoice;

// speech
class sayWin : public ofThread {
public:
	void exec(string lang, string text){
		if (FAILED(cpVicehr)) {
			cpVicehr = cpVoice.CoCreateInstance(CLSID_SpVoice);
		}
		if (SUCCEEDED(cpVicehr)) {
			if (lang == "en") {
				this->text = "<xml><lang langid=\"409\">" + text + "</lang></xml>";     // 409:English
			}
			else if (lang == "jp") {
				this->text = "<xml><lang langid=\"411\">" + text + "</lang></xml>";     // 411:Japanese
			}
			startThread();
		}
    }
private:
	void threadedFunction(){
		int iBufferSize = ::MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, (wchar_t *)NULL, 0);
		wchar_t* wpBufWString = (wchar_t*)new wchar_t[iBufferSize];
		::MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, wpBufWString, iBufferSize);
		cpVoice->Speak(wpBufWString, SPF_DEFAULT, NULL);
		delete[] wpBufWString;
    }
	string text;
};
sayWin mySayWin[8];



void speakFunc(enum SPEAK speak, string name, float lap)
{
    switch(speak){
        case PILOT_ENTRY:
            speakAny("jp", name + "選手 エントリー");
            break;
        case PILOT_START:
            speakAny("jp", name + "選手 スタート");
            break;
        case LAP_TIME:
            speakAny("jp", name + " " + ofToString(lap) );
            break;
    }
}

void speakAny(string lang, string text)
{
#ifdef TARGET_OSX
    int pid = fork();
    if (pid == 0) {
        // child process
        if (lang == "en") {
            execlp("say", "", "-r", "240", "-v", "Victoria", text.c_str(), NULL);
        }
        else if (lang == "jp") {
            execlp("say", "", "-r", "240", "-v", "Kyoko", text.c_str(), NULL);
        }
        OF_EXIT_APP(-1);
    }
#endif /* TARGET_OSX */
#ifdef TARGET_WIN32
    for (int i = 0; i < SPCH_SLOT_NUM; i++) {
        if (mySayWin[i].isThreadRunning() == false) {
            mySayWin[i].exec(lang, text);
            break;
        }
    }
#endif /* TARGET_WIN32 */
}




