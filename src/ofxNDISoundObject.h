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
						   ofxNDI::Location _location = ofxNDI::Location::BOTH):
	sourceName(_sourceName),
	sourceUrl(_sourceUrl),
	receiverName(_receiverName),
	group(_group),
	waittime_ms(_waittime_ms),
	location(_location){}
	
	
	std::string receiverName, sourceName, sourceUrl;
	std::string group="";
	uint32_t waittime_ms = 1000;
	ofxNDI::Location location = ofxNDI::Location::BOTH;
	ofxNDISoundQuality quality = ofxNDISoundQuality::HIGH;
	
	bool isSourceEmpty(){
		return sourceName.empty() && sourceUrl.empty();
	}
	
	std::pair<std::string, int> getSourceIpAndPort(){
		
		auto split = ofSplitString(sourceUrl, ":");
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

	operator ofxNDI::Recv::Receiver::Settings() const {
		ofxNDI::Recv::Receiver::Settings s;
				
		s.bandwidth = ( quality == ofxNDISoundQuality::HIGH )? NDIlib_recv_bandwidth_audio_only : NDIlib_recv_bandwidth_lowest ;
		s.name = receiverName;
		
		return s;
	}
	operator ofxNDI::Source() const {
		ofxNDI::Source s;
		s.p_ndi_name = sourceName;
		s.p_url_address = sourceUrl;
		return s;
	}
	
	
	
//	std::vector<std::string> extra_ips;
};

//--------------------------------------------------------------------------
class ofxNDIReceiverSoundObject : public ofxSoundObject{

public:
	ofxNDIReceiverSoundObject();
	
	
	static ofxNDI::Source findSourceByName(const std::string& name,
									 const std::string &group="",
									 uint32_t waittime_ms=1000,
									 ofxNDI::Location location= ofxNDI::Location::BOTH,
									 const std::vector<std::string>& extra_ips={});

	static ofxNDI::Source findSourceByIpAndPort(const std::string& ip,
										  const int & port = -1,
										  const std::string &group="",
										  uint32_t waittime_ms=1000,
										  ofxNDI::Location location= ofxNDI::Location::BOTH,
										  const std::vector<std::string>& extra_ips={});

	
	static ofxNDI::Source findSourceByUrl(const std::string& url,
									 const std::string &group="",
									 uint32_t waittime_ms=1000,
									 ofxNDI::Location location= ofxNDI::Location::BOTH,
									 const std::vector<std::string>& extra_ips={});

	
	
	
	/// The following setup functions all receive as last parameter the bandwith setting. You can choose any of the following values.
	////
	///	NDIlib_recv_bandwidth_metadata_only  // Receive metadata.
	///	NDIlib_recv_bandwidth_audio_only     // Receive metadata, audio.
	///	NDIlib_recv_bandwidth_lowest         // Receive metadata, audio, video at a lower bandwidth and resolution.
	///	NDIlib_recv_bandwidth_highest        // Receive metadata, audio, video at full resolution.

	
	
	bool setup(const ofxNDIReceiverSettings& settings);
	
	bool setupBySourceIpAndPort(const std::string& ip, const int & port = -1, std::string receiverName="", 	ofxNDISoundQuality quality = ofxNDISoundQuality::HIGH);
	
	bool setupBySourceUrl(const std::string & sourceUrl, std::string receiverName="", 	ofxNDISoundQuality quality = ofxNDISoundQuality::HIGH);
	
	bool setupBySourceName(const std::string &sourceName, std::string receiverName="", ofxNDISoundQuality quality = ofxNDISoundQuality::HIGH);
	
	bool reconnect();
	
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
	
	
	
	bool hasMetadata();
	
	const std::string& getMetadata();
	
private:
	
	ofxNDIReceiverSettings settings;
	
	ofxNDIReceiver receiver_;
	ofxNDIRecvAudioFrameSync audio_;

//	ofxNDI::Recv::Receiver::Settings receiverSettings;
//	std::string receiverName;
//	NDIlib_recv_bandwidth_e bandwidth;
//	
	
	ofxNDI::Source source;
	std::string group;
	bool bAudioNeedsSetup = false;

	std::atomic<size_t> numChannels;
	
	ofSoundBuffer workingBuffer;
	
	
	std::string metadata;
	std::mutex metadataMutex;
	
};


















