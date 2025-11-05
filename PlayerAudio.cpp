
#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();
}

PlayerAudio::~PlayerAudio()
{
    transportSource.setSource(nullptr);
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    transportSource.getNextAudioBlock(bufferToFill);

    if (!looping && !transportSource.isPlaying()  //task 4
        && getPosition() >= getLength() - 0.05)
    {
        stop();
    }
}

void PlayerAudio::releaseResources()
{
    transportSource.releaseResources();
}

bool PlayerAudio::LoadFile(const juce::File& file)
{
    bool wasLooping = getIsLooping();

    transportSource.stop();
    transportSource.setSource(nullptr);

    juce::AudioFormatReader* reader = formatManager.createReaderFor(file);
    if (reader != nullptr)
    {
        readerSource.reset(new juce::AudioFormatReaderSource(reader, true));
        readerSource->setLooping(wasLooping);
        transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
        return true;
    }
    return false;
}


void PlayerAudio::play()
{
    transportSource.start();
}

void PlayerAudio::stop()
{
    transportSource.stop();
    transportSource.setPosition(0.0);
}

void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}

void PlayerAudio::setPosition(double pos)
{
    transportSource.setPosition(pos);
}

double PlayerAudio::getPosition() const
{
    return transportSource.getCurrentPosition();
}

double PlayerAudio::getLength() const
{
    return transportSource.getLengthInSeconds();
}

void PlayerAudio::setLooping(bool shouldLoop)
{
    if (readerSource != nullptr)
        readerSource->setLooping(shouldLoop);
}

bool PlayerAudio::getIsLooping() const
{
    return (readerSource != nullptr) && readerSource->isLooping();
}
