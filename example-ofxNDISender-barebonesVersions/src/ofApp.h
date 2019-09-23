#pragma once

#include "ofMain.h"
#include "ofxSoundPlayerObject.h"

#include "waveformDraw.h"
#include "SineWaveGenerator.h"

#include "ofxNDISoundObject.h"
#include "ofxGui.h"
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
	
	// the sender object
	ofxNDISenderSoundObject sender;
	
	// even when you might want to send audio through NDI and not output to your speakers you need a soundstream as it will be used to clock the NDI stream. The sender needs to be connected to a signal chain which has an output stream at its end. Without it it will not work.
	ofSoundStream stream;
    ofxSoundOutput output;
	ofxSoundInput input;
	/// these are here just to have different possible sources of audio to send via NDI.
    ofxSoundPlayerObject player;

	void loadDialogPlayer();
	
	
	// stuff for the help text
	
	bool bDrawHelp = true;
	
	
	
	
	
};
