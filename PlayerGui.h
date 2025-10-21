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
    juce::TextButton loadButton { "Load" };
    juce::TextButton playButton { "play" };
    juce::TextButton stopButton { "stop" };
    juce::Slider volumeSlider;

    std::unique_ptr<juce::FileChooser>filechooser;

    //events handlers

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
    };