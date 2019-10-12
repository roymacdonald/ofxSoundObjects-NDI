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
//---------- UTILS
//--------------------------------------------------------------------------
std::pair<std::string, int> getIpAndPort(const std::string&url){
	
	auto split = ofSplitString(url, ":");
	std::string ip = "";
	int port = -1;
	if(split.size()){
		auto ip = split[0];
		if(split.size() > 1){
			port = ofToInt(split[1]);
		}
	}
	return {ip, port};
}



//--------------------------------------------------------------------------
//---------- NDI RECEIVER SETTINGS
//--------------------------------------------------------------------------

ofxNDIReceiverSettings::ofxNDIReceiverSettings(const std::string& _sourceName,
					   const std::string& _sourceUrl,
					   const std::string& _receiverName,
					   const std::string& _group,
					   uint32_t _waittime_ms,
					   ofxNDI::Location _location):
sourceName(_sourceName),
sourceUrl(_sourceUrl),
receiverName(_receiverName),
group(_group),
waittime_ms(_waittime_ms),
location(_location)

{
}
//--------------------------------------------------------------------------
std::pair<std::string, int> ofxNDIReceiverSettings::getSourceIpAndPort(){
	return getIpAndPort(sourceUrl);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSettings::ofxNDIReceiverSettings::isSourceEmpty() const{
	return sourceName.empty() && sourceUrl.empty();
}
//--------------------------------------------------------------------------
ofxNDIReceiverSettings::operator ofxNDI::Recv::Receiver::Settings() const {
	ofxNDI::Recv::Receiver::Settings s;
	
	s.bandwidth = ( quality == ofxNDISoundQuality::HIGH )? NDIlib_recv_bandwidth_audio_only : NDIlib_recv_bandwidth_lowest ;
	s.name = receiverName;
	
	return s;
}
//--------------------------------------------------------------------------
ofxNDIReceiverSettings::operator ofxNDI::Source() const {
	ofxNDI::Source s;
	s.p_ndi_name = sourceName;
	s.p_url_address = sourceUrl;
	return s;
}


//--------------------------------------------------------------------------
//---------- NDI RECEIVER
//--------------------------------------------------------------------------
ofxNDIReceiverSoundObject::ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){
	numChannels = 0;
	metadata ="";
	bAudioNeedsSetup = false;
}
//--------------------------------------------------------------------------
bool findSource( std::function<bool(const ofxNDI::Source & s)> func, ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips, const std::string& notFoundMsg){
	//						const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	
	auto sources = ofxNDI::listSources(settings.waittime_ms, settings.location, settings.group, extra_ips);
	
	auto found = find_if(begin(sources), end(sources), func);
	if(found == end(sources)) {
		ofLogWarning("ofxNDI") << "no NDI source " << notFoundMsg;
	}else{
		settings.sourceName = found->p_ndi_name;
		settings.sourceUrl = found->p_url_address;
		return true;
	}
	
	return false;
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::findSourceByName(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips){
// std::string& name, const std::string &group,uint32_t waittime_ms,ofxNDI::Location location, const std::vector<std::string>& extra_ips){
	if(settings.sourceName != "") {
		 return findSource([&settings](const ofxNDI::Source &s) {
							  return ofIsStringInString(s.p_ndi_name, settings.sourceName);
						  },
						  settings, extra_ips, "with name: " +  settings.sourceName);
		
	}
	ofLogWarning("ofxNDIReceiverSoundObject::findSourceByName") << "can not find a source by name if name is empty";
	return false;
}

//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::findSourceByIpAndPort(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips){
	
	
	if(settings.sourceUrl != "") {
		auto ipAndPort = settings.getSourceIpAndPort();
		return findSource([&ipAndPort](const ofxNDI::Source &s) {
			auto sip = getIpAndPort(s.p_url_address);
			return (ipAndPort.first == sip.first) && (ipAndPort.second == -1 || ipAndPort.second == sip.second);
		},settings, extra_ips, "with IP: " + ipAndPort.first + " and port: " + ((ipAndPort.second == -1)?" ANY": ofToString(ipAndPort.second)));
	}
	ofLogWarning("ofxNDIReceiverSoundObject::findSourceByIpAndPort") << "can not find a source by IP if IP is empty";
	return false;
}

//--------------------------------------------------------------------------

bool ofxNDIReceiverSoundObject::findSourceByUrl(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips){
	
	if(settings.sourceUrl != "") {
		return findSource([&settings](const ofxNDI::Source &s) {
			return settings.sourceUrl == s.p_url_address;
		},settings, extra_ips, "at URL: " + settings.sourceUrl );
	}
	ofLogWarning("ofxNDIReceiverSoundObject::findSourceByUrl") << "can not find a source by URL if URL is empty";
	return false;
	
}

//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceIpAndPort(const std::string& ip, const int & port, const ofxNDIReceiverSettings& settings ){
	this->settings = settings;
	this->settings.sourceUrl = ip + ":" + ofToString(port);
	return setup(this->settings);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceUrl(const std::string & sourceUrl, const ofxNDIReceiverSettings& settings ){
	this->settings = settings;
	this->settings.sourceUrl = sourceUrl;
	return setup(this->settings);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setupBySourceName(const std::string &sourceName, const ofxNDIReceiverSettings& settings){
	this->settings = settings;
	this->settings.sourceName = sourceName;
	return setup(this->settings);
}
//--------------------------------------------------------------------------
bool ofxNDIReceiverSoundObject::setup(const ofxNDIReceiverSettings& settings){
	this->settings = settings;
	if(!this->settings.isSourceEmpty()){
		bAudioNeedsSetup =false;
		bool bFound = findSourceByName(this->settings);
		if(!bFound){
			bFound = findSourceByUrl(this->settings);
		}
		
		if(bFound && receiver_.setup(this->settings, this->settings)){
			bAudioNeedsSetup = true;
			std::cout << settings << std::endl;
			return true;
		}else{
			ofLogWarning("ofxNDIReceiverSoundObject::setup") << "Unable to setup. Source seems to be unavailable. try calling reconnect() later.";
			return false;
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
//	ofLogVerbose("ofxNDIReceiverSoundObject::reconnect") << "attempting reconnect..";
	bool reconnected = setup(this->settings);
	
	ofLogVerbose("ofxNDIReceiverSoundObject::reconnect") << (reconnected?"Success.":"not possible.") << " Receiver Name: " << this->settings.receiverName << " source name: " << this->settings.sourceName << " source url: " << this->settings.sourceUrl;
	return reconnected;
	
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
	return settings.sourceName;
}
//--------------------------------------------------------------------------
const std::string& ofxNDIReceiverSoundObject::getSourceUrl(){
	return settings.sourceUrl;
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
//--------------------------------------------------------------------------
ofxNDIReceiverSettings& ofxNDIReceiverSoundObject::getSettings(){
	return settings;
}
//--------------------------------------------------------------------------
const ofxNDIReceiverSettings& ofxNDIReceiverSoundObject::getSettings() const{
	return settings;
}
//--------------------------------------------------------------------------
std::ostream& operator << (std::ostream& os, const ofxNDIReceiverSettings& ss) {
	os << "receiverName " << ss.receiverName 
	<< " sourceName " << ss.sourceName 
	<< " sourceUrl " << ss.sourceUrl 
	<< " group " << ss.group 
	<< " waittime_ms " << ss.waittime_ms 
	<< " location ";
	switch (ss.location) {
		case ofxNDI::Location::BOTH: os << "BOTH" ;  break;
		case ofxNDI::Location::LOCAL: os<< "LOCAL"; break;
		case ofxNDI::Location::REMOTE: os<< "REMOTE"; break;
	}
	
	os << " quality " << ((ss.quality == ofxNDISoundQuality::HIGH )?"HIGH":"LOW");
	 
	
	
	return os;
}
