#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"

class PlayerGui : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
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
    //Gui elements
    juce::TextButton loadButton{ "Load" };
    juce::TextButton playButton{ "play" };
    juce::TextButton endButton{ "end" };
	juce::TextButton pauseButton{ "Pause" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };  //task 4
    juce::Slider volumeSlider;
    double savedPosition = 0.0;


    std::unique_ptr<juce::FileChooser>filechooser;
    bool isMuted = false;

    //events handlers

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
};

