#include "MainComponent.h"

MainComponent::MainComponent() {
    addAndMakeVisible(playerGui);
    setSize(800, 600);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() {
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    playerGui.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    playerGui.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources() {
    playerGui.releaseResources();
}

void MainComponent::resized() {
    playerGui.setBounds(getLocalBounds());
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour::fromRGB(40, 44, 52));
}
