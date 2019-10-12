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
	}else if(key == 'r'){
		receiver.reconnect();
		setViewports();// this is called so the help text gets updated
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
