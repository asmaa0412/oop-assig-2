#include "MainComponent.h"

MainComponent::MainComponent()
{
    addAndMakeVisible(playerGui);
    setSize(700, 350);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerGui.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    bufferToFill.clearActiveBufferRegion();
    playerGui.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    playerGui.releaseResources();
}

void MainComponent::resized()
{
    playerGui.setBounds(getLocalBounds());
}
