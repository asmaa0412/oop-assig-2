#include "MainComponent.h"

MainComponent::MainComponent() {
    addAndMakeVisible(tabbedComponent);

    tabbedComponent.addTab("Player 1", juce::Colours::darkgrey, &playerGui1, false);
    tabbedComponent.addTab("Player 2", juce::Colours::darkgrey, &playerGui2, false);

    addAndMakeVisible(mixButton);
    addAndMakeVisible(effectsButton);
    addAndMakeVisible(effectsComboBox);

    mixButton.addListener(this);
    effectsButton.addListener(this);
    effectsComboBox.addListener(this);

    mixButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(97, 175, 239));
    effectsButton.setColour(juce::TextButton::buttonColourId, juce::Colour::fromRGB(97, 175, 239));

    effectsComboBox.addItem("None", 1);
    effectsComboBox.addItem("Reverb Pro", 2);
    effectsComboBox.addItem("Multi Echo", 3);
    effectsComboBox.addItem("Chorus FX", 4);
    effectsComboBox.addItem("Flanger", 5);
    effectsComboBox.addItem("Low Pass", 6);
    effectsComboBox.setSelectedId(1);

    addAndMakeVisible(player1MixSlider);
    addAndMakeVisible(player2MixSlider);
    addAndMakeVisible(player1MixLabel);
    addAndMakeVisible(player2MixLabel);
    addAndMakeVisible(mixTitleLabel);

    player1MixSlider.setRange(0.0, 1.0, 0.01);
    player1MixSlider.setValue(0.5);
    player1MixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    player1MixSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(255, 159, 67));
    player1MixSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(97, 175, 239));
    player1MixSlider.addListener(this);

    player2MixSlider.setRange(0.0, 1.0, 0.01);
    player2MixSlider.setValue(0.5);
    player2MixSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    player2MixSlider.setColour(juce::Slider::thumbColourId, juce::Colour::fromRGB(255, 159, 67));
    player2MixSlider.setColour(juce::Slider::trackColourId, juce::Colour::fromRGB(97, 175, 239));
    player2MixSlider.addListener(this);

    player1MixLabel.setText("Player 1 Level", juce::dontSendNotification);
    player2MixLabel.setText("Player 2 Level", juce::dontSendNotification);
    mixTitleLabel.setText("Audio Mixer", juce::dontSendNotification);

    player1MixLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(240, 240, 240));
    player2MixLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(240, 240, 240));
    mixTitleLabel.setColour(juce::Label::textColourId, juce::Colour::fromRGB(240, 240, 240));
    mixTitleLabel.setJustificationType(juce::Justification::centred);
    mixTitleLabel.setFont(juce::Font(16.0f, juce::Font::bold));

    setSize(1000, 800);
    setAudioChannels(0, 2);
}

MainComponent::~MainComponent() {
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    playerGui1.prepareToPlay(samplesPerBlockExpected, sampleRate);
    playerGui2.prepareToPlay(samplesPerBlockExpected, sampleRate);
    tempBuffer.setSize(2, samplesPerBlockExpected);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) {
    if (isMixMode) {
        mixAudio(bufferToFill);
    }
    else {
        if (tabbedComponent.getCurrentTabIndex() == 0)
            playerGui1.getNextAudioBlock(bufferToFill);
        else
            playerGui2.getNextAudioBlock(bufferToFill);
    }

    if (effectsEnabled) {
        applyEffects(bufferToFill);
    }
}

void MainComponent::mixAudio(const juce::AudioSourceChannelInfo& bufferToFill) {
    bufferToFill.clearActiveBufferRegion();

    juce::AudioSourceChannelInfo tempInfo(&tempBuffer, 0, bufferToFill.numSamples);

    playerGui1.getNextAudioBlock(tempInfo);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
            tempBuffer, channel, 0,
            bufferToFill.numSamples,
            (float)player1MixSlider.getValue());
    }

    playerGui2.getNextAudioBlock(tempInfo);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel) {
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
            tempBuffer, channel, 0,
            bufferToFill.numSamples,
            (float)player2MixSlider.getValue());
    }
}

void MainComponent::applyEffects(const juce::AudioSourceChannelInfo& bufferToFill) {
    int effectType = effectsComboBox.getSelectedId();

    auto& buffer = *bufferToFill.buffer;
    const int numSamples = bufferToFill.numSamples;
    const int startSample = bufferToFill.startSample;

    switch (effectType) {
    case 2:
        applyReverbEffect(buffer, startSample, numSamples);
        break;

    case 3:
        applyEchoEffect(buffer, startSample, numSamples);
        break;

    case 4:
        applyChorusEffect(buffer, startSample, numSamples);
        break;

    case 5:
        applyFlangerEffect(buffer, startSample, numSamples);
        break;

    case 6:
        applyLowPassFilter(buffer, startSample, numSamples);
        break;
    }
}

void MainComponent::applyReverbEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    static float reverbBuffer[2][5000] = { 0 };
    static int reverbIndex = 0;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* revBuffer = reverbBuffer[channel];

        for (int i = 0; i < numSamples; ++i) {
            int sampleIndex = startSample + i;

            float reverb = 0.0f;
            reverb += revBuffer[(reverbIndex + 5000 - 147) % 5000] * 0.6f;
            reverb += revBuffer[(reverbIndex + 5000 - 341) % 5000] * 0.4f;
            reverb += revBuffer[(reverbIndex + 5000 - 689) % 5000] * 0.3f;
            reverb += revBuffer[(reverbIndex + 5000 - 1123) % 5000] * 0.2f;

            revBuffer[reverbIndex] = channelData[sampleIndex] + reverb * 0.85f;

            channelData[sampleIndex] = channelData[sampleIndex] * 0.7f + reverb * 0.3f;

            reverbIndex = (reverbIndex + 1) % 5000;
        }
    }
}

void MainComponent::applyEchoEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    static juce::AudioBuffer<float> echoBuffer;
    static bool echoBufferInitialized = false;

    if (!echoBufferInitialized) {
        echoBuffer.setSize(buffer.getNumChannels(), 20000);
        echoBuffer.clear();
        echoBufferInitialized = true;
    }

    static int echoWritePos = 0;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* echoData = echoBuffer.getWritePointer(channel);

        for (int i = 0; i < numSamples; ++i) {
            int sampleIndex = startSample + i;

            float echo1 = echoData[(echoWritePos + 20000 - 5000) % 20000] * 0.5f;
            float echo2 = echoData[(echoWritePos + 20000 - 10000) % 20000] * 0.3f;
            float echo3 = echoData[(echoWritePos + 20000 - 15000) % 20000] * 0.2f;

            float combinedEcho = echo1 + echo2 + echo3;

            echoData[echoWritePos] = channelData[sampleIndex] + combinedEcho * 0.8f;

            channelData[sampleIndex] = channelData[sampleIndex] * 0.6f + combinedEcho * 0.4f;

            echoWritePos = (echoWritePos + 1) % 20000;
        }
    }
}

void MainComponent::applyChorusEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    static float lfoPhase = 0.0f;
    const float lfoRate = 0.5f;
    const float lfoDepth = 10.0f;
    const int baseDelay = 15;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);

        for (int i = 0; i < numSamples; ++i) {
            int sampleIndex = startSample + i;

            float lfoValue = sinf(lfoPhase);
            int variableDelay = baseDelay + (int)(lfoDepth * lfoValue);

            if (variableDelay < 1) variableDelay = 1;

            if (sampleIndex >= variableDelay) {
                float delayedSample = channelData[sampleIndex - variableDelay];

                float wetMix = 0.7f;
                float dryMix = 0.3f;
                channelData[sampleIndex] = (channelData[sampleIndex] * dryMix) +
                    (delayedSample * wetMix);
            }

            lfoPhase += 2.0f * 3.14159f * lfoRate / 44100.0f;
            if (lfoPhase > 2.0f * 3.14159f) {
                lfoPhase -= 2.0f * 3.14159f;
            }
        }
    }
}

void MainComponent::applyFlangerEffect(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    static float flangerBuffer[2][1000] = { 0 };
    static int flangerWritePos = 0;
    static float flangerPhase = 0.0f;
    const float flangerRate = 0.3f;
    const float flangerDepth = 8.0f;
    const int baseDelay = 5;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        auto* flangeBuffer = flangerBuffer[channel];

        for (int i = 0; i < numSamples; ++i) {
            int sampleIndex = startSample + i;

            float lfoValue = sinf(flangerPhase);
            int delaySamples = baseDelay + (int)(flangerDepth * (lfoValue + 1.0f) * 0.5f);

            int readPos = (flangerWritePos + 1000 - delaySamples) % 1000;
            float delayedSample = flangeBuffer[readPos];

            flangeBuffer[flangerWritePos] = channelData[sampleIndex] + delayedSample * 0.7f;
            channelData[sampleIndex] = (channelData[sampleIndex] + delayedSample) * 0.7f;

            flangerWritePos = (flangerWritePos + 1) % 1000;
            flangerPhase += 2.0f * 3.14159f * flangerRate / 44100.0f;
            if (flangerPhase > 2.0f * 3.14159f) {
                flangerPhase -= 2.0f * 3.14159f;
            }
        }
    }
}

void MainComponent::applyLowPassFilter(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) {
    static float prevSample[2] = { 0 };
    const float cutoff = 0.2f;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        float& prev = prevSample[channel];

        for (int i = 0; i < numSamples; ++i) {
            int sampleIndex = startSample + i;

            float current = channelData[sampleIndex];
            float filtered = prev + cutoff * (current - prev);

            channelData[sampleIndex] = filtered;
            prev = filtered;
        }
    }
}

void MainComponent::releaseResources() {
    playerGui1.releaseResources();
    playerGui2.releaseResources();
}

void MainComponent::resized() {
    auto area = getLocalBounds();

    auto controlArea = area.removeFromTop(40);
    mixButton.setBounds(controlArea.removeFromLeft(100).reduced(5));
    effectsButton.setBounds(controlArea.removeFromLeft(100).reduced(5));
    effectsComboBox.setBounds(controlArea.removeFromLeft(150).reduced(5));

    if (isMixMode) {
        auto mixerArea = area.removeFromBottom(100);
        mixTitleLabel.setBounds(mixerArea.removeFromTop(25).reduced(10, 0));

        auto mixerRow1 = mixerArea.removeFromTop(30).reduced(10, 5);
        player1MixLabel.setBounds(mixerRow1.removeFromLeft(100));
        player1MixSlider.setBounds(mixerRow1.removeFromLeft(200));

        auto mixerRow2 = mixerArea.removeFromTop(30).reduced(10, 5);
        player2MixLabel.setBounds(mixerRow2.removeFromLeft(100));
        player2MixSlider.setBounds(mixerRow2.removeFromLeft(200));
    }

    tabbedComponent.setBounds(area);
}

void MainComponent::paint(juce::Graphics& g) {
    g.fillAll(juce::Colour::fromRGB(40, 44, 52));
}

void MainComponent::buttonClicked(juce::Button* button) {
    if (button == &mixButton) {
        isMixMode = !isMixMode;
        mixButton.setToggleState(isMixMode, juce::dontSendNotification);
        resized();
    }
    else if (button == &effectsButton) {
        effectsEnabled = !effectsEnabled;
        effectsButton.setToggleState(effectsEnabled, juce::dontSendNotification);
    }
}

void MainComponent::comboBoxChanged(juce::ComboBox* comboBox) {
}

void MainComponent::sliderValueChanged(juce::Slider* slider) {
}
