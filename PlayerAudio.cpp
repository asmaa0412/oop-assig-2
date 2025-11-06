#include "PlayerAudio.h"

PlayerAudio::PlayerAudio() {
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio() {
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    resampleSource.getNextAudioBlock(bufferToFill);
}

void PlayerAudio::releaseResources() {
    transportSource.releaseResources();
    resampleSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file) {
    transportSource.stop();
    transportSource.setSource(nullptr);
    readerSource.reset();

    if (auto* reader = formatManager.createReaderFor(file)) {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        readerSource->setLooping(looping);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        return true;
    }
    return false;
}

void PlayerAudio::play() { 
    transportSource.start(); 
}

void PlayerAudio::stop() { 
    transportSource.stop(); 
    transportSource.setPosition(0.0); 
}

void PlayerAudio::setGain(float gain) { 
    transportSource.setGain(gain); 
}

void PlayerAudio::setSpeed(double newSpeed) { 
    resampleSource.setResamplingRatio(newSpeed); 
}

void PlayerAudio::setPosition(double pos) { 
    transportSource.setPosition(pos); 
}

double PlayerAudio::getPosition() const { 
    return transportSource.getCurrentPosition(); 
}

double PlayerAudio::getLength() const { 
    return transportSource.getLengthInSeconds(); 
}

void PlayerAudio::setLooping(bool shouldLoop) {
    looping = shouldLoop;
    if (readerSource) readerSource->setLooping(looping);
}

bool PlayerAudio::getIsLooping() const {
    return readerSource ? readerSource->isLooping() : false;
}
