#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetLogLevel(OF_LOG_VERBOSE);

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
	vuMeter.connectTo(waveCircular).connectTo(wave).connectTo(sender).connectTo(output);
	
	
    sinGen.setup(440);
	
	setInput(INPUT_SINE_WAVE);
	
	
	/// The circularBufferWaveformDraw class needs to be set how big the circular buffer will be.
	/// The value passed is how many buffers from the sound stream it will be able to fit.
	waveCircular.setNumBuffers(100);
	/// The waveform can have a grid, in this case we are going to set it to the size of the buffers received.
	/// This is useful for visually checking what we are sending in each buffer.
	waveCircular.setGridSpacingByNumSamples(soundSettings.bufferSize);
	
	
	
	
	/// Just setting up the gui and its listeners
	gui.setup();
	gui.add(amplitudeParam);
	gui.add(freqParam);
	gui.add(minFreq);
	gui.add(maxFreq);
//	gui.add(waveShapeParam);
	gui.add(setFreqToBufferSize);
	
	listeners.push(amplitudeParam.newListener([&](float&f){
		sinGen.amplitude = f;
	}));
	listeners.push(freqParam.newListener([&](float&f){
		sinGen.freq = f;
	}));
	listeners.push(minFreq.newListener([&](float&){
		if(minFreq > maxFreq){
			maxFreq = minFreq;
			freqParam = minFreq;
		}
		freqParam.setMin(minFreq.get());
	}));
	
	listeners.push(maxFreq.newListener([&](float&){
		if(maxFreq < minFreq){
			minFreq = maxFreq;
			freqParam = maxFreq;
		}
		freqParam.setMax(maxFreq.get());
	}));
	
//	listeners.push(waveShapeParam.newListener([&](float&f){
//		waveShape = f;
//	}));
	
	listeners.push(setFreqToBufferSize.newListener([&](){
		freqParam = stream.getSampleRate()/ (float)stream.getBufferSize();
	}));
	
	
	setViewports();
}

//--------------------------------------------------------------
void ofApp::update(){
//	if(inputIndex == INPUT_SINE_WAVE ){
//		sinGen.freq = ofMap(ofGetMouseX(), 0, ofGetWidth(), 20, 20000);
//		sinGen.amplitude = ofMap(ofGetMouseY(), 0, ofGetWidth(), 0, 1);
//	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofSetBackgroundColor(60);
	

	
	ofSetColor(ofColor::white);
	
	vuMeter.draw();
	
	wave.draw();
	waveCircular.draw();
	
	if(inputIndex == INPUT_SINE_WAVE && bDrawGui) gui.draw();
	
	if(bDrawHelp){

		ofSetColor(40);

		ofDrawRectangle(helpTextRect);
		ofSetColor(255);
		ofDrawBitmapString(helpText,helpTextRect.x + 20,20);
	}
	
	
}
//--------------------------------------------------------------
void ofApp::setHelpText(){
	stringstream ss;
	ss << "Press [key] to : " << endl;;
	ss << "      [space]  : switch to next input ( file player/sine wave/ live mic )" << endl;;
	ss << "      [l]      : Load an audio file to the sound player" << endl;
	ss << "      [m]      : mute / unmute the ndi output to speakers." << endl;
	ss << "      [h]      : Toggle Draw this help text" << endl;
	
	ss  <<  "CURRENT INPUT: ";
	
	
		 if(inputIndex == INPUT_PLAYER){    ss  <<  "Audio File Player"; }
	else if(inputIndex == INPUT_SINE_WAVE){ ss  <<  "Sine Wave Generator"; }
	else if(inputIndex == INPUT_LIVE_MIC){  ss  <<  "Live input (mic)"; }
	ss << endl;
	ss << endl;
	
	ss << "Clic and drag over a waveform to move around. RightButton drag to zoom." << endl;
	ss << "Do either while pressing the Shift key to restrict to a single dimension" <<endl;

	helpText = ss.str();
}

//--------------------------------------------------------------
void ofApp::setViewports(){
	
	setHelpText();
	ofRectangle prevRect (0,0,0,0);
	
	if(inputIndex == INPUT_SINE_WAVE && bDrawGui){
		gui.setPosition(0,0);
		prevRect = gui.getShape();
	}
	
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
void ofApp::setInput(InputIndex newInput){
	if(newInput != inputIndex){
		inputIndex = newInput;
		if(inputIndex == INPUT_SINE_WAVE){
			
			sinGen.connectTo(vuMeter);
			
		}else if(inputIndex == INPUT_PLAYER){
			if(!player.isLoaded()){
				loadDialogPlayer();
			}
			
			player.connectTo(vuMeter);
			
		}else if(inputIndex == INPUT_LIVE_MIC){
			input.connectTo(vuMeter);
		}
		setViewports();
	}
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (key == ' ') {
		setInput( InputIndex((inputIndex +1) % (int)NUM_INPUTS));
	}else if(key == 'l'){
		loadDialogPlayer();
	}else if(key == 'm'){
		sender.setMuteOutput(!sender.isMuteOutput());
	}else if(key == 'g'){
		if(inputIndex == INPUT_SINE_WAVE){
			bDrawGui ^= true;
		}
	}else if(key == 'h'){
		bDrawHelp ^= true;
		setViewports();
	}
	
}
//--------------------------------------------------------------
void ofApp::loadDialogPlayer(){
	ofFileDialogResult result = ofSystemLoadDialog("Please select an audio file (.mp3, .wav, .aiff, .aac");
	if (result.bSuccess) {
		player.load(result.getPath());
		player.play();
		setInput(INPUT_PLAYER);
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
	setViewports();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
