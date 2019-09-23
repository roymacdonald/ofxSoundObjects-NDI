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
	
	virtual size_t getNumChannels() override;
	void setNumChannels(const size_t& channels);
	
	void setMuteOutput(bool bMute);
	bool isMuteOutput();
	
	
	virtual void audioOut(ofSoundBuffer &output) override;
	
private:

	ofxNDISender sender_;
	ofxNDISendAudio audio_;

	std::atomic<bool> bMute;
	
	size_t numChannels = 0;
	ofSoundBuffer workingBuffer;
};


//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject():ofxSoundObject(OFX_SOUND_OBJECT_SOURCE){}
	
	static ofxNDI::Source findSource(const std::string& name_or_url,
	const std::string &group="",
	uint32_t waittime_ms=1000,
	ofxNDI::Location location= ofxNDI::Location::BOTH,
	const std::vector<std::string> extra_ips={});

	
	/// The following setup functions all receive as last parameter the bandwith setting. You can choose any of the following values.
	////
	///	NDIlib_recv_bandwidth_metadata_only  // Receive metadata.
	///	NDIlib_recv_bandwidth_audio_only     // Receive metadata, audio.
	///	NDIlib_recv_bandwidth_lowest         // Receive metadata, audio, video at a lower bandwidth and resolution.
	///	NDIlib_recv_bandwidth_highest        // Receive metadata, audio, video at full resolution.

	
	void setup(int sourceIndex = 0, std::string receiverName="", NDIlib_recv_bandwidth_e bandwidth=NDIlib_recv_bandwidth_highest);
	
	
	void setup(const std::string& name_or_url,
			   const std::string &group="", std::string receiverName="", NDIlib_recv_bandwidth_e bandwidth = NDIlib_recv_bandwidth_highest);
	
	void setup(const ofxNDI::Source &source, std::string receiverName="", NDIlib_recv_bandwidth_e bandwidth = NDIlib_recv_bandwidth_highest);
	
	
	const std::string& getSourceName();
	const std::string& getSourceUrl();
	bool isConnected();
	
	virtual size_t getNumChannels() override;
	void setNumChannels(const size_t& channels);
	
	
	virtual void audioOut(ofSoundBuffer &output) override;
	
	
		  ofxNDIReceiver& getOfxNDIReceiver();
	const ofxNDIReceiver& getOfxNDIReceiver() const;

		  ofxNDIRecvAudioFrameSync& getOfxNDIAudioFrame();
	const ofxNDIRecvAudioFrameSync& getOfxNDIAudioFrame() const;
	
private:
	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;
	
	ofxNDI::Source source;
	bool bAudioNeedsSetup = false;

	size_t numChannels = 0;
	
	ofSoundBuffer workingBuffer;
	
};


















