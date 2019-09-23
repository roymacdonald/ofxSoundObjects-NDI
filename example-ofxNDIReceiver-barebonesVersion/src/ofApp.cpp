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
	
	/// the receiver.setup(...) function can receive more parameters.
//	
//		receiver.setup(
//					   "ofxSoundObject NDI Sender",   // the name of the sender from which you want to receive audio.
//		               "ofx",                         // the name of the group at which the sender is published. Default is empty
//					   "ofxSoundObject NDI Receiver", // the name of this receiver. It is defaulted to empty.
//					   NDIlib_recv_bandwidth_highest  // the bandwitdh setting for the receiver. It can take any value listed below.
//		               );
	
	/// Bandwidth setting values
	///	NDIlib_recv_bandwidth_metadata_only  // Receive metadata.
	///	NDIlib_recv_bandwidth_audio_only     // Receive metadata, audio.
	///	NDIlib_recv_bandwidth_lowest         // Receive metadata, audio, video at a lower bandwidth and resolution.
	///	NDIlib_recv_bandwidth_highest        // Receive metadata, audio, video at full resolution.
	
	
	
	

	/// You can set the number of audio channels that the NDI receiver should expect. This allows you to have a larger channel count that what your soundstream actually supports.
	/// If you dont set this you'll simply get the amount of channels that your sound stream has.
	receiver.setNumChannels(2);
	
	
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
