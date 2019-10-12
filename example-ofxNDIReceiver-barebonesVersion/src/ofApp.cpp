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
	/// SET THE RECEIVER SETTINGS
	/// Look at the console as the available sources are printed there.
	/// you can either set the name or url. If the source is not found by name, then it will be searched by url.
	/// Note: log level needs to be verbose in order to see this in the console. 	ofSetLogLevel(OF_LOG_VERBOSE);
	
	ofxNDIReceiverSettings settings;
	//	settings.receiverName="";
	settings.sourceName="ofxSoundObject NDI Sender";
	//	settings.sourceUrl=""; //could be an
	settings.group="ofx";
	//	settings.waittime_ms = 1000;
	//	settings.location = ofxNDI::Location::BOTH;
	//	settings.quality = ofxNDISoundQuality::HIGH;
	
	receiver.setup(settings);
	
	

	/// You can set the number of audio channels that the NDI receiver should expect. This allows you to have a larger channel count that what your soundstream actually supports.
	/// If you dont set this you'll simply get the amount of channels that your sound stream has.
	receiver.setNumChannels(1);
	
	
	/// Connect the receiver to the output. Because of the design of ofxSoundObjects the receiver needs to be connected to a signal chain which is connected to soundstream.
	/// In this case we are just connecting it to output, but you could have it connected to a much more complex signal chain. Take a look at the fancy example which connects the receiver to several sound objects.
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
