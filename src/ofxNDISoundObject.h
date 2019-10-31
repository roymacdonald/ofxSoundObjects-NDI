//
//  NDISenderObject.h
//  example-ofxNDI
//
//  Created by Roy Macdonald on 9/14/19.
//
//
#pragma once

#include "ofxSoundObject.h"
#include <mutex>
#include <future>
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
	void setMetadata(const std::string& metadata);
	
private:

	ofxNDISender sender_;
	ofxNDISendAudio audio_;

	std::atomic<bool> bMute;
	
	std::atomic<size_t> numChannels;
	ofSoundBuffer workingBuffer;
	
	
	std::string metadata;
	std::mutex metadataMutex;
};
//--------------------------------------------------------------------------
enum ofxNDISoundQuality{
	HIGH = 0,
	LOW
};
//--------------------------------------------------------------------------
class ofxNDIReceiverSettings{
public:

	
	ofxNDIReceiverSettings(){}
	ofxNDIReceiverSettings(const std::string& _sourceName,
						   const std::string& _sourceUrl,
						   const std::string& _receiverName,
						   const std::string& _group="",
						   uint32_t _waittime_ms = 1000,
						   ofxNDI::Location _location = ofxNDI::Location::BOTH);
	
	
	std::string receiverName, sourceName, sourceUrl;
	std::string group="";
	uint32_t waittime_ms = 1000;
	ofxNDI::Location location = ofxNDI::Location::BOTH;
	ofxNDISoundQuality quality = ofxNDISoundQuality::HIGH;
	
	bool isSourceEmpty() const;
	
	std::pair<std::string, int> getSourceIpAndPort();

	operator ofxNDI::Recv::Receiver::Settings() const;

	operator ofxNDI::Source() const;

	friend std::ostream& operator << (std::ostream& os, const ofxNDIReceiverSettings& ss) ;
	
};
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject();
	
	
	static bool findSourceByName(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips={});

	static bool findSourceByIpAndPort(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips={});

	static bool findSourceByUrl(ofxNDIReceiverSettings& settings, const std::vector<std::string>& extra_ips={});

	
	bool setup(const ofxNDIReceiverSettings& settings);
	
	bool setupBySourceIpAndPort(const std::string& ip, const int & port = -1, const ofxNDIReceiverSettings& settings = ofxNDIReceiverSettings());
	bool setupBySourceUrl(const std::string & sourceUrl, const ofxNDIReceiverSettings& settings = ofxNDIReceiverSettings());
	bool setupBySourceName(const std::string &sourceName, const ofxNDIReceiverSettings& settings = ofxNDIReceiverSettings());

	bool reconnect();
	void asyncReconnect();
	
	
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
	
	
		  ofxNDIReceiverSettings& getSettings();
	const ofxNDIReceiverSettings& getSettings() const;
	
	
	bool hasMetadata();
	const std::string& getMetadata();
	
private:
	
	ofxNDIReceiverSettings settings;
	
	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;

	std::atomic<bool> bAudioNeedsSetup;

	std::atomic<size_t> numChannels;
	
	ofSoundBuffer workingBuffer;
	
	
	std::string metadata = "";
	std::mutex metadataMutex;
	
	std::future<bool> reconnectFuture;
	
};


















