#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

	
	ofxSoundUtils::printOutputSoundDevices();
	ofxSoundUtils::printInputSoundDevices();
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	auto inDevices = ofxSoundUtils::getInputSoundDevices();
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 2;
	soundSettings.numOutputChannels = 2;
	soundSettings.sampleRate = 48000;
	soundSettings.bufferSize = 256;
	/// IMPORTANT!
	/// in the following line put the correct number for the audio device you want to use. These have been printed to the console
	soundSettings.setOutDevice(outDevices[0]);
	soundSettings.setInDevice(inDevices[0]);
	
	stream.setup(soundSettings);
	
	stream.setInput(input);
	stream.setOutput(output);
	
	
	/// SUPER IMPORTANT!
	/// In the following line put the NDI name of the sender. This is what's going to be used to identify it on the network.
	/// You can additionally pass a second string for the group where it's going to be published.
	//	sender.setup("ofxSoundObject NDI Sender");
	sender.setup("ofxSoundObject NDI Sender", "ofx");
	
	
	/// EVEN MORE IMPORTANT !
	/// even when you might want to send audio through NDI and not output to your speakers you need a soundstream as it will be used to clock the NDI stream. The sender needs to be connected to a signal chain which has an output stream at its end. Without it it will not work.
	/// As such you might also want to mute the output of the sender.
	
	sender.setMuteOutput(true);
	
	// the current sound source, which is setup in the setInput function will connect to the vuMeter, thus completing the signal chain
	player.connectTo(sender).connectTo(output);
	loadDialogPlayer();
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofSetBackgroundColor(60);
	

	ofSetColor(ofColor::yellow);
	
	
	player.drawDebug(20, 20);
	
	
	ofSetColor(ofColor::white);
	
	
	
	if(bDrawHelp){

		ofBitmapFont bf;
		stringstream ss;
		ss << "Press [key] to : " << endl;;
		ss << "      [l]      : Load an audio file to the sound player" << endl;
		ss << "      [m]      : mute / unmute the ndi output to speakers." << endl;
		ss << "      [h]      : Toggle Draw this help text" << endl;
		
		ofSetColor(255);
		ofDrawBitmapString(ss.str(), ofGetWidth() - bf.getBoundingBox(ss.str(), 0,0).width - 20 ,20);
	}
	
	
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'l'){
		loadDialogPlayer();
	}else if(key == 'm'){
		sender.setMuteOutput(!sender.isMuteOutput());
	}else if(key == 'h'){
		bDrawHelp ^= true;
	}
	
}
//--------------------------------------------------------------
void ofApp::loadDialogPlayer(){
	ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac");
	if (result.bSuccess) {
		player.load(result.getPath());
		player.play();
		player.setLoop(true);
	}
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
