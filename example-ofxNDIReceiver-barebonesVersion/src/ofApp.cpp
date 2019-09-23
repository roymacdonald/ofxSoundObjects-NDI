#include "ofApp.h"
#include "ofxSoundUtils.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	
	ofxSoundUtils::printOutputSoundDevices();
	
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 0;
	soundSettings.numOutputChannels = 2;
	soundSettings.sampleRate =48000;
	soundSettings.bufferSize = 256;
	soundSettings.numBuffers = 1;

	/// IMPORTANT!
	/// in the following line put the correct number for the audio device you want to use. These have been printed to the console
	soundSettings.setOutDevice(outDevices[0]);
	
	stream.setup(soundSettings);
	
	
	stream.setOutput(output);
	
	
	/// SUPER IMPORTANT!
	/// In the following line put the NDI source name or url that you want to connect to.
	/// Look at the console as the available sources are printed there.
	/// Note: log level needs to be verbose in order to see this in the console. 	ofSetLogLevel(OF_LOG_VERBOSE);
	/// If a blank string is passed it will connect to the first available source
	receiver.setup("ofxSoundObject NDI Sender", "ofx");
	receiver.connectTo(output);
	
	
}

//--------------------------------------------------------------
void ofApp::exit(){
	stream.close();
}
//--------------------------------------------------------------
void ofApp::update(){
}
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetBackgroundColor(60);
	
	if(bDrawHelp){
		stringstream ss;
		
		ss << "Press key [h] to draw this help text" << endl;
		ss << endl;
		ss << "is receiver connected : " << boolalpha << receiver.isConnected() <<endl;
		ss << "Source Name: " << receiver.getSourceName() <<endl;
		ss << "Source URL: " << receiver.getSourceUrl() <<endl;
		
		ofSetColor(255);
		ofDrawBitmapString(ss.str(), 20,20);
	}
	
	
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'h'){
		bDrawHelp ^= true;
	}
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
