
# ofxSoundObjects_NDI

This is an additional module to be used with [ofxSoundObjects](https://github.com/roymacdonald/ofxSoundObjects/).

This addon was specifically made for using ofxNDI to allow you to send and receive audio from other computers over a network, while using ofxSoundObjects

There are two classes made for this `ofxNDISenderSoundObject` and `ofxNDIReceiverSoundObject`. Look at the examples on how to use these.

## Installation	
You will need to download or clone into `openFrameworks/addons/` folder the following:

* This addon
* [ofxSoundObjects](https://github.com/roymacdonald/ofxSoundObjects/)
* [ofxNDI](https://github.com/nariakiiwatani/ofxNDI) 

Use the Project Generator and add these three addons to either update a project or create a new one.

### Xcode [IMPORTANT!]
There is one additional step you need to take so the NDI librarie links properly.
Once you've created your new project copy the `config.make` file from either `/ofxSoundObjects_NDI/example-ofxNDISender/config.make` or `ofxSoundObjects_NDI/example-ofxNDIReceiver/config.make` into your newly created project. Now update your project with project generator. You only need to do this when creating the project. 


## Dependencies
* [ofxSoundObjects](https://github.com/roymacdonald/ofxSoundObjects/)
* [ofxNDI](https://github.com/nariakiiwatani/ofxNDI) 


## License
Read the `license.md` file