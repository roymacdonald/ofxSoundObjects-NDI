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
	numChannels = 0;
	metadata = "";
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
	
	if(numChannels != workingBuffer.getNumChannels() ||
	   workingBuffer.getNumFrames() != output.getNumFrames()||
	   workingBuffer.getSampleRate() != output.getSampleRate()){
		
		workingBuffer.allocate(output.getNumFrames(), numChannels);
		workingBuffer.setSampleRate(output.getSampleRate());
	}
	workingBuffer.setTickCount(output.getTickCount());
	workingBuffer.setDeviceID(output.getDeviceID());
	
	if(inputObject!=nullptr) {
		inputObject->audioOut(workingBuffer);
	}
	if(sender_.isSetup()){
		audio_.send(workingBuffer, metadata );
		metadata = "";
	}
	if(bMute){
		output.set(0);
	}else{
		workingBuffer.copyTo(output);
	}
}
//--------------------------------------------------------------------------
void ofxNDISenderSoundObject::setMetadata(const std::string& metadata){
	std::lock_guard<std::mutex> mtx(metadataMutex);
	this->metadata = metadata;
}


//--------------------------------------------------------------------------
//---------- NDI RECEIVER
//--------------------------------------------------------------------------
ofxNDIReceiverSoundObject::ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){
	numChannels = 0;
	metadata ="";
}
ofxNDI::Source findSource( std::function<bool(const ofxNDI::Source & s)> func,  const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	
	
	ofxNDI::Source source;
	
		auto sources = ofxNDI::listSources(waittime_ms, location, group, extra_ips);
		
		auto found = find_if(begin(sources), end(sources), func);
		if(found == end(sources)) {
			ofLogWarning("ofxNDI") << "no NDI source found";
		}else{
			source = *found;
		}
	
	return source;
	

}
//--------------------------------------------------------------------------
ofxNDI::Source ofxNDIReceiverSoundObject::findSourceByName(const std::string& name, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	
	
	
	if(name != "") {
//		auto sources = ofxNDI::listSources(waittime_ms, location, group, extra_ips);
//		
//		auto found = find_if(begin(sources), end(sources), [&name](const ofxNDI::Source &s) {
//
//			return ofIsStringInString(s.p_ndi_name, name);
//
//		});
//		if(found == end(sources)) {
//			ofLogWarning("ofxNDI") << "no NDI source found by string:" << name;
//		}else{
//			source = *found;
//		}
		
		return findSource([&name](const ofxNDI::Source &s) {
							  return ofIsStringInString(s.p_ndi_name, name);
						  },
						  group, waittime_ms, location, extra_ips);
			
		
	}
	ofLogWarning("ofxNDIReceiverSoundObject::findSourceByName") << "can not find a source by name if name is empty";
	return ofxNDI::Source();
}

//--------------------------------------------------------------------------
ofxNDI::Source ofxNDIReceiverSoundObject::findSourceByIpAndPort(const std::string& ip, const int & port, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	
	ofxNDI::Source source;
	
	if(ip != "") {
//		auto sources = ofxNDI::listSources(waittime_ms, location, group, extra_ips);
//		
//		
//		auto found = find_if(begin(sources), end(sources), [&ip, &port](const ofxNDI::Source &s) {
//			
//			auto split = ofSplitString(s.p_url_address, ":");
//			
//			
//			if(split.size() > 1){
//				return (ip == split[0]) && (port == -1 || port == ofToInt(split[1]));
//			}
//			
//			ofLogError("ofxNDIReceiverSoundObject::findSourceByIpAndPort") << "Error parsing. This should not happen.";
//			return false;
//			
//		});
//		if(found == end(sources)) {
//			ofLogWarning("ofxNDI") << "no NDI source found with IP: " << ip << "and port " << port;
//		}else{
//			source = *found;
//		}
		return findSource([&ip, &port](const ofxNDI::Source &s) {
			
			auto split = ofSplitString(s.p_url_address, ":");
			
			
			if(split.size() > 1){
				return (ip == split[0]) && (port == -1 || port == ofToInt(split[1]));
			}
			
			ofLogError("ofxNDIReceiverSoundObject::findSourceByIpAndPort") << "Error parsing. This should not happen.";
			return false;
			
		}
		,group, waittime_ms, location, extra_ips);
		
		
	}else{
		ofLogWarning("ofxNDIReceiverSoundObject::findSourceByIpAndPort") << "can not find a source by URL if URL is empty";
	}
	
	return source;
}

//--------------------------------------------------------------------------

ofxNDI::Source ofxNDIReceiverSoundObject::findSourceByUrl(const std::string& url, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	
	if(url.empty()){
		ofLogWarning("ofxNDIReceiverSoundObject::findSourceByUrl") << "can not find a source by URL if URL is empty";
		return ofxNDI::Source();
	}
			
	auto split = ofSplitString(url, ":");
	std::string ip;
	int port = -1;
	if(split.size()){
		auto ip = split[0];
		if(split.size() > 1){
			port = ofToInt(split[1]);
		}
		return findSourceByIpAndPort(ip, port, group,  waittime_ms,  location, extra_ips);
		
	}
	
}


//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceIpAndPort(const std::string& ip, const int & port, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	this->source.p_url_address = ip +":"+ ofToString(port);
	return this->setup(findSourceByIpAndPort(ip, port), receiverName, bandwidth);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceUrl(const std::string & sourceUrl, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	this->source.p_url_address = sourceUrl;
	return this->setup(findSourceByUrl(sourceUrl), receiverName, bandwidth);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceName(const std::string &sourceName, std::string receiverName, NDIlib_recv_bandwidth_e bandwidth){
	this->source.p_ndi_name = sourceName;
	return this->setup(findSourceByName(sourceName), receiverName, bandwidth);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setup(const ofxNDIReceiverSettings& settings){
	if(!settings.isSourceEmpty()){
		bAudioNeedsSetup =false;
		
//		receiverSettings.name = receiverName;
//		receiverSettings.bandwidth = bandwidth;
//		this->source = source;
		
		
		
		if(receiver_.setup(source, receiverSettings)){
			bAudioNeedsSetup = true;
			return true;
		}else{
			ofLogWarning("ofxNDIReceiverSoundObject::setup") << "Unable to setup. Source seems to be unable. try calling reconnect().";
		}
	}
	ofLogVerbose("ofxNDIReceiverSoundObject::setup") << "Unable to setup. source name and url are empty.";
	return false;
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::reconnect(){
	if(isConnected()){
		ofLogWarning("ofxNDIReceiverSoundObject::reconnect") << "already connected!";
		return true;
	}
	ofLogVerbose("ofxNDIReceiverSoundObject::reconnect") << "attempting reconnect..";
		if(!source.p_ndi_name.empty()){
			return this->setupBySourceName(source.p_ndi_name, receiverSettings.name, receiverSettings.bandwidth);
		}if(!source.p_url_address.empty()){
			return this->setupBySourceUrl(source.p_url_address, receiverSettings.name, receiverSettings.bandwidth);
		}
	
	ofLogVerbose("ofxNDIReceiverSoundObject::reconnect") << "not possible. Name: " << source.p_ndi_name << " url: " << source.p_url_address << " receiver name: " << receiverSettings.name;
		return false;
	
}
//--------------------------------------------------------------------------
void ofxNDIReceiverSoundObject::audioOut(ofSoundBuffer &output) {
	if(isConnected()){
		if(numChannels == 0) {
			numChannels = output.getNumChannels();
		}

		if(bAudioNeedsSetup){
			bAudioNeedsSetup = false;
			audio_.setup(receiver_);
			audio_.setSampleRate(output.getSampleRate());
			audio_.setNumChannels(numChannels);
			audio_.setNumSamples(output.getNumFrames());
		}
		
		if(numChannels != workingBuffer.getNumChannels() ||
		   workingBuffer.getNumFrames() != output.getNumFrames()||
		   workingBuffer.getSampleRate() != output.getSampleRate()){
			
			workingBuffer.allocate(output.getNumFrames(), numChannels);
			workingBuffer.setSampleRate(output.getSampleRate());
		}
		workingBuffer.setTickCount(output.getTickCount());
		workingBuffer.setDeviceID(output.getDeviceID());
		
		
		audio_.update();
		if(audio_.isFrameNew()) {
			audio_.decodeTo(workingBuffer);
			if(audio_.getFrame().p_metadata != NULL){
				metadata = std::string(audio_.getFrame().p_metadata);
			}else{
				metadata = "";
			}
		
			workingBuffer.copyTo(output);
		}else{
			output.set(0);
		}
		
	}else{
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
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::hasMetadata(){
	std::lock_guard<std::mutex> mtx(metadataMutex);
	return metadata != "";
}
//--------------------------------------------------------------------------
const std::string& ofxNDIReceiverSoundObject::getMetadata(){
	std::lock_guard<std::mutex> mtx(metadataMutex);
	return metadata;
}

