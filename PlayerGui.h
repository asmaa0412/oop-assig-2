#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGui :
    public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::ListBoxModel,
    public juce::Timer
{
public:
    PlayerGui();
    ~PlayerGui() override;

    void resized() override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& buffer);
    void releaseResources();

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton goToStartButton{ "Restart" };
    juce::TextButton goToEndButton{ "End" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };

    juce::Slider volumeSlider;
    juce::Slider positionSlider; // A–B loop
    juce::Slider posSlide;       // Playhead slider

    juce::Label timeDisplayLabel;
    juce::Label metadataLabel;

    juce::ListBox playlistBox{ "Playlist", this };
    juce::Array<juce::File> playlistFiles;

    std::unique_ptr<juce::FileChooser> fileChooser;
    double savedPosition = 0.0;
    bool isMuted = false;

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void timerCallback() override;

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void updateMetadata(const juce::File& file);

    juce::String formatTime(double seconds);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
};
