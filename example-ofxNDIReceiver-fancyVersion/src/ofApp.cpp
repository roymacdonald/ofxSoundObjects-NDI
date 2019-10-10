#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);
	
	
	
	ofxSoundUtils::printOutputSoundDevices();
	
	auto outDevices = ofxSoundUtils::getOutputSoundDevices();
	
	
	ofSoundStreamSettings soundSettings;
	soundSettings.numInputChannels = 0;
	soundSettings.numOutputChannels = 1;
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
	//					   NDIlib_recv_bandwidth_highest ); // the bandwitdh setting for the receiver. It can take any value listed below. Default is highest
	
	/// Bandwidth setting values
	///	NDIlib_recv_bandwidth_metadata_only  // Receive metadata.
	///	NDIlib_recv_bandwidth_audio_only     // Receive metadata, audio.
	///	NDIlib_recv_bandwidth_lowest         // Receive metadata, audio, video at a lower bandwidth and resolution.
	///	NDIlib_recv_bandwidth_highest        // Receive metadata, audio, video at full resolution.
	
	
	
	
	
	/// You can set the number of audio channels that the NDI receiver should expect. This allows you to have a larger channel count that what your soundstream actually supports.
	/// If you dont set this you'll simply get the amount of channels that your sound stream has.
	receiver.setNumChannels(2);
	
	
	/// Connect the receiver to the signal chain. Because of the design of ofxSoundObjects the receiver needs to be connected to a signal chain which is connected to soundstream (output).
	/// In this case we are just connecting it to output, but you could have it connected to a much more complex signal chain, as it is in this case. 

	
	receiver.connectTo(vuMeter).connectTo(waveCircular).connectTo(wave).connectTo(output);
	
	
	
	/// The circularBufferWaveformDraw class needs to be set how big the circular buffer will be.
	/// The value passed is how many buffers from the sound stream it will be able to fit.
	waveCircular.setNumBuffers(100);
	/// The waveform can have a grid, in this case we are going to set it to the size of the buffers received.
	/// This is useful for visually checking what we are sending in each buffer.
	waveCircular.setGridSpacingByNumSamples(soundSettings.bufferSize);
	
	
	
	setViewports();
	
	
}
//--------------------------------------------------------------
void ofApp::setHelpText(){
	stringstream ss;
	
	ss << "Press [key] to : " << endl;;
	ss << "      [h]      : Draw this help text" << endl;
	ss << "is receiver connected : " << boolalpha << receiver.isConnected() <<endl;
	ss << "Source Name: " << receiver.getSourceName() <<endl;
	ss << "Source URL: " << receiver.getSourceUrl() <<endl;
	
	ss << endl;
	
	ss << "Clic and drag over a waveform to move around. RightButton drag to zoom." << endl;
	ss << "Do either while pressing the Shift key to restrict to a single dimension" <<endl;
	
	helpText = ss.str();
}

//--------------------------------------------------------------
void ofApp::setViewports(){
	
	setHelpText();
	ofRectangle prevRect (0,0,0,0);
	

	if(bDrawHelp){
		ofBitmapFont bf;
		auto bb = bf.getBoundingBox(helpText, 20, 20);
		helpTextRect.set(prevRect.getMaxX(), 0, ofGetWidth() - prevRect.getMaxX(), bb.getMaxY() + 10);
		prevRect = helpTextRect;
	}
	
	ofRectangle vuRect(0, prevRect.getMaxY(), 40, ofGetHeight() - prevRect.getMaxY());
	
	wave.setup({vuRect.getTopRight(), ofGetWidth() - vuRect.getMaxX(), vuRect.height*0.5f});
	
	waveCircular.setup({wave.getBottomLeft(), wave.width, wave.height});
	
	vuMeter.setup(vuRect,VUMeter::VU_DRAW_VERTICAL,VUMeter::VU_STACK_VERTICAL);
	
	
	
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
	
	//this is called on each draw so to update the drawing of the stream metadata
	//	setViewports();
	
	
	ofSetColor(ofColor::white);
	
	wave.draw();
	waveCircular.draw();
	
	vuMeter.draw();
	
	if(bDrawHelp){
		
		ofSetColor(40);
		ofDrawRectangle(helpTextRect);
		
		ofSetColor(255);
		ofDrawBitmapString(helpText,helpTextRect.x + 20,20);
	}
	ofDrawBitmapStringHighlight(receiver.getMetadata(), wave.x + 10, wave.y + 20);
	
}
//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	if(key == 'h'){
		bDrawHelp ^= true;
		setViewports();
	}
}
//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){}
//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
	setViewports();
}
//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){}
//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){}
