#include "PlayerGui.h"

PlayerGui::PlayerGui()
{
    // Add buttons
    for (auto* btn : { &loadButton, &playButton , &stopButton ,&loopButton })
    {
        addAndMakeVisible(*btn);
        btn->addListener(this);
    }

    // Volume slider
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
}

PlayerGui::~PlayerGui(){}

void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
}

void PlayerGui::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGui::resized()
{
    int y = 10;
    loadButton.setBounds(10, y, 80, 30);
    playButton.setBounds(100, y, 80, 30);
    stopButton.setBounds(190, y, 80, 30);
    loopButton.setBounds(280, y, 80, 30); // task 4
    volumeSlider.setBounds(10, 60, getWidth() - 20, 30);

}

void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        filechooser = std::make_unique<juce::FileChooser>(
    "Select an audio file...",
    juce::File{},
    "*.wav;*.mp3");


        filechooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
            [this](const juce::FileChooser& fc)
            {
                auto file = fc.getResult();
                if (file.existsAsFile())
                    playerAudio.LoadFile(file);
            });
    }

    else if (button == &playButton)
    {
        playerAudio.play();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &loopButton)
    {
        bool newLoopState = !playerAudio.getIsLooping();
        playerAudio.setLooping(newLoopState);
        loopButton.setButtonText(newLoopState ? "Un-Loop" : "Loop");
    }



}

void PlayerGui::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &volumeSlider)
        playerAudio.setGain((float)volumeSlider.getValue());
}
