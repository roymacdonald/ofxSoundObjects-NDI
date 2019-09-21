//
//  NDISenderObject.h
//  example-ofxNDI
//
//  Created by Roy Macdonald on 9/14/19.
//
//
#pragma once

#include "ofxSoundObject.h"
#include <atomic>
#include "ofxNDISender.h"
#include "ofxNDISendStream.h"
#include "ofxNDIReceiver.h"
#include "ofxNDIRecvStream.h"

//--------------------------------------------------------------------------
class ofxNDISenderSoundObject : public ofxSoundObject{

public:
	ofxNDISenderSoundObject();
	
	void setup(const std::string& name, const std::string &group="");
	
	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	virtual size_t getNumChannels() override;
	void setNumChannels(const size_t& channels);
	
	void setMuteOutput(bool bMute);
	bool isMuteOutput();
	
private:

	ofxNDISender sender_;
	ofxNDISendAudio audio_;

	std::atomic<bool> bMute;
	
	size_t numChannels = 0;
	
};


//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
	void setup(const std::string& name_or_url,
			   const std::string &group="",
			   uint32_t waittime_ms=1000,
			   ofxNDI::Location location= ofxNDI::Location::BOTH,
			   const std::vector<std::string> extra_ips={});

	virtual void process(ofSoundBuffer &input, ofSoundBuffer &output) override;
	
	std::string getSourceName();
	std::string getSourceUrl();
	bool isConnected();
	
	virtual size_t getNumChannels() override;
	void setNumChannels(const size_t& channels);
	
	
private:

	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;
	ofxNDI::Source source;
	bool bAudioNeedsSetup = false;

	size_t numChannels = 0;
};


















