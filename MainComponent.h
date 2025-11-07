#pragma once
#include <JuceHeader.h>
#include "PlayerGui.h"

class MainComponent : public juce::AudioAppComponent,
    public juce::Button::Listener,
    public juce::ComboBox::Listener,
    public juce::Slider::Listener
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void resized() override;
    void paint(juce::Graphics& g) override;

    void buttonClicked(juce::Button* button) override;
    void comboBoxChanged(juce::ComboBox* comboBox) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    PlayerGui playerGui1;
    PlayerGui playerGui2;
    juce::TabbedComponent tabbedComponent{ juce::TabbedButtonBar::TabsAtTop };

    juce::TextButton mixButton{ "Mix Mode" };
    juce::TextButton effectsButton{ "Effects" };
    juce::ComboBox effectsComboBox;

    juce::Slider player1MixSlider;
    juce::Slider player2MixSlider;
    juce::Label player1MixLabel;
    juce::Label player2MixLabel;
    juce::Label mixTitleLabel;

    bool isMixMode = false;
    bool effectsEnabled = false;

    juce::AudioBuffer<float> tempBuffer;

    void mixAudio(const juce::AudioSourceChannelInfo& bufferToFill);
    void applyEffects(const juce::AudioSourceChannelInfo& bufferToFill);

    void applyReverbEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void applyEchoEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void applyChorusEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void applyFlangerEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);
    void applyLowPassFilter(juce::AudioBuffer<float>& buffer, int startSample, int numSamples);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
