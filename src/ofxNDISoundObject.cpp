//
//  ofxNDISoundObject.cpp
//
//  Created by Roy Macdonald on 9/14/19.
//
//

#include "ofxNDISoundObject.h"
#include "ofLog.h"
//--------------------------------------------------------------------------
//---------- NDI SENDER
//--------------------------------------------------------------------------
ofxNDISenderSoundObject::ofxNDISenderSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_PROCESSOR){
	bMute = true;
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setup(const std::string& name, const std::string & group){
	if(sender_.setup(name, group)) {
		audio_.setup(sender_);
	}
	else {
		ofLogError("NDISenderObject::setup") << "NDI setup failed.";
	}
}

//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setMuteOutput(bool bMute){
	this->bMute = bMute;
}
//--------------------------------------------------------------------------
bool ofxNDISenderSoundObject::isMuteOutput(){
	return bMute;
}
//--------------------------------------------------------------------------
size_t ofxNDISenderSoundObject::getNumChannels(){
	return numChannels;
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setNumChannels(const size_t& channels){
	numChannels = channels;
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::audioOut(ofSoundBuffer &output) {
	
	if(numChannels == 0) numChannels = output.getNumChannels();
	
	workingBuffer.allocate(output.getNumFrames(), numChannels);
	workingBuffer.setSampleRate(output.getSampleRate());
	workingBuffer.setTickCount(output.getTickCount());
	workingBuffer.setDeviceID(output.getDeviceID());
	
	if(inputObject!=nullptr) {
		inputObject->audioOut(workingBuffer);
	}
	if(sender_.isSetup()){
		audio_.send(workingBuffer);
	}
	if(bMute){
		output.set(0);
	}else{
		workingBuffer.copyTo(output);
	}
}


//--------------------------------------------------------------------------
//---------- NDI RECEIVER
//--------------------------------------------------------------------------

ofxNDI::Source ofxNDIReceiverSoundObject::findSource(const std::string& name_or_url, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string> extra_ips){
	
	bool bFound = false;
	
	ofxNDI::Source source;
	
	if(name_or_url != "") {
		auto sources = ofxNDI::listSources(waittime_ms, location, group, extra_ips);
		
		auto found = find_if(begin(sources), end(sources), [name_or_url](const ofxNDI::Source &s) {
			return ofIsStringInString(s.p_ndi_name, name_or_url) || ofIsStringInString(s.p_url_address, name_or_url);
		});
		
		if(found == end(sources)) {
			ofLogWarning("ofxNDI") << "no NDI source found by string:" << name_or_url;
		}else{
			source = *found;
		}
	}
	return source;
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::setup(int sourceIndex, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	bAudioNeedsSetup =false;
	ofxNDI::Recv::Receiver::Settings settings;
	settings.name = receiverName;
	settings.bandwidth = bandwidth;
	if(receiver_.setup(sourceIndex, settings)){
		this->source = source;
		bAudioNeedsSetup = true;
	}
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::setup(const ofxNDI::Source &source, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	bAudioNeedsSetup =false;
	if(source.p_ndi_name.empty() && source.p_url_address.empty()){
		setup(0, receiverName, bandwidth);
	}else{
		ofxNDI::Recv::Receiver::Settings settings;
		settings.name = receiverName;
		settings.bandwidth = bandwidth;
		if(receiver_.setup(source, settings)){
			this->source = source;
			bAudioNeedsSetup = true;
		}
	}
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::setup(const std::string& name_or_url, const std::string &group, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	setup(findSource(name_or_url, group), receiverName, bandwidth);
}

//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::audioOut(ofSoundBuffer &output) {
	if(isConnected()){
		if(bAudioNeedsSetup){
			bAudioNeedsSetup = false;
			audio_.setup(receiver_);
			audio_.setSampleRate(output.getSampleRate());
			audio_.setNumChannels(numChannels);
			audio_.setNumSamples(output.getNumFrames());
		}
		if(numChannels == 0) {
			numChannels = output.getNumChannels();
		}
		
		if(numChannels != output.getNumChannels() ||
		   output.getNumChannels() != workingBuffer.getNumChannels() ||
		   numChannels != workingBuffer.getNumChannels()
		   ){
			
			workingBuffer.allocate(output.getNumFrames(), numChannels);
			workingBuffer.setSampleRate(output.getSampleRate());
			workingBuffer.setTickCount(output.getTickCount());
			workingBuffer.setDeviceID(output.getDeviceID());
		}
		
		if(receiver_.isConnected()) {
			audio_.update();
			if(audio_.isFrameNew()) {
				audio_.decodeTo(workingBuffer);
				workingBuffer.copyTo(output);
			}else{
				output.set(0);
			}
		}
	}else
	{
		// if the receiver_ object is not setup just place silence in the out buffer
		output.set(0);
	}
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::isConnected(){
	return receiver_.isConnected();
}
//--------------------------------------------------------------------------
const std::string& ofxNDIReceiverSoundObject::getSourceName(){
	return source.p_ndi_name;
}
//--------------------------------------------------------------------------
const std::string& ofxNDIReceiverSoundObject::getSourceUrl(){
	return source.p_url_address;
}
//--------------------------------------------------------------------------
size_t ofxNDIReceiverSoundObject::getNumChannels(){
	return numChannels;
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::setNumChannels(const size_t& channels){
	numChannels = channels;
}
//--------------------------------------------------------------------------
ofxNDIReceiver& ofxNDIReceiverSoundObject::getOfxNDIReceiver(){
	return receiver_;
}
//--------------------------------------------------------------------------
const ofxNDIReceiver& ofxNDIReceiverSoundObject::getOfxNDIReceiver() const{
	return receiver_;
}
//--------------------------------------------------------------------------
ofxNDIRecvAudioFrameSync& ofxNDIReceiverSoundObject::getOfxNDIAudioFrame(){
	return audio_;
}
//--------------------------------------------------------------------------
const ofxNDIRecvAudioFrameSync& ofxNDIReceiverSoundObject::getOfxNDIAudioFrame() const{
	return audio_;
}



