#include "PlayerGui.h"

juce::String PlayerGui::formatTime(double seconds)
{
    if (seconds < 0.0) seconds = 0.0;
    int totalSeconds = (int)std::round(seconds);
    int minutes = totalSeconds / 60;
    int remainingSeconds = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", minutes, remainingSeconds);
}

PlayerGui::PlayerGui()
{
    // Buttons
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &goToStartButton,
                       &goToEndButton, &muteButton, &loopButton })
    {
        addAndMakeVisible(*btn);
        btn->addListener(this);
    }

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.addListener(this);

    addAndMakeVisible(positionSlider);
    positionSlider.setSliderStyle(juce::Slider::TwoValueHorizontal);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.setRange(0.0, 100.0, 0.01);
    positionSlider.setMinAndMaxValues(0.0, 100.0);
    positionSlider.addListener(this);

    addAndMakeVisible(posSlide);
    posSlide.setRange(0.0, 1.0, 0.001);
    posSlide.setValue(0.0);
    posSlide.addListener(this);
    posSlide.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);

    addAndMakeVisible(timeDisplayLabel);
    timeDisplayLabel.setJustificationType(juce::Justification::centred);
    timeDisplayLabel.setFont(juce::Font(14.0f));
    timeDisplayLabel.setText("00:00 / 00:00", juce::dontSendNotification);

    addAndMakeVisible(metadataLabel);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);

    addAndMakeVisible(playlistBox);
    playlistBox.setRowHeight(25);

    startTimerHz(30);
}

PlayerGui::~PlayerGui()
{
    stopTimer();
}

void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& buffer)
{
    playerAudio.getNextAudioBlock(buffer);
}

void PlayerGui::releaseResources()
{
    playerAudio.releaseResources();
}

void PlayerGui::resized()
{
    int y = 10;
    volumeSlider.setBounds(10, y, getWidth() - 20, 30);

    y += 40;
    int buttonWidth = 80, buttonHeight = 30;
    loadButton.setBounds(10, y, buttonWidth, buttonHeight);
    playButton.setBounds(100, y, buttonWidth, buttonHeight);
    pauseButton.setBounds(190, y, buttonWidth, buttonHeight);
    goToStartButton.setBounds(280, y, buttonWidth, buttonHeight);
    goToEndButton.setBounds(370, y, buttonWidth, buttonHeight);
    muteButton.setBounds(460, y, buttonWidth, buttonHeight);
    loopButton.setBounds(550, y, buttonWidth, buttonHeight);

    y += 40;
    posSlide.setBounds(10, y, getWidth() - 100, 25);
    timeDisplayLabel.setBounds(getWidth() - 90, y, 80, 25);

    y += 30;
    positionSlider.setBounds(10, y, getWidth() - 100, 25);

    y += 40;
    metadataLabel.setBounds(10, y, getWidth() - 20, 25);

    y += 35;
    playlistBox.setBounds(10, y, getWidth() - 20, getHeight() - y - 10);
}

void PlayerGui::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>(
            "Select audio files...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac");

        fileChooser->launchAsync(
            juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectMultipleItems,
            [this](const juce::FileChooser& fc)
            {
                auto results = fc.getResults();
                for (auto& f : results)
                    playlistFiles.add(f);

                playlistBox.updateContent();
                repaint();
            });
    }
    else if (button == &playButton)
    {
        int selectedRow = playlistBox.getSelectedRow();
        if (selectedRow >= 0 && selectedRow < playlistFiles.size())
        {
            if (playerAudio.LoadFile(playlistFiles[selectedRow]))
            {
                updateMetadata(playlistFiles[selectedRow]);

                double length = playerAudio.getLength();
                positionSlider.setRange(0.0, length, 0.01);
                positionSlider.setMinAndMaxValues(0.0, length);
                posSlide.setRange(0.0, length, 0.01);
            }
        }
        playerAudio.play();
        pauseButton.setButtonText("Pause");
    }
    else if (button == &pauseButton)
    {
        if (playerAudio.isPlaying())
        {
            savedPosition = playerAudio.getPosition();
            playerAudio.stop();
            pauseButton.setButtonText("Resume");
        }
        else
        {
            playerAudio.setPosition(savedPosition);
            playerAudio.play();
            pauseButton.setButtonText("Pause");
        }
    }
    else if (button == &goToStartButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
        playerAudio.play();
    }
    else if (button == &goToEndButton)
    {
        playerAudio.setPosition(playerAudio.getLength() - 0.05);
    }
    else if (button == &muteButton)
    {
        isMuted = !isMuted;
        playerAudio.setGain(isMuted ? 0.0f : (float)volumeSlider.getValue());
        muteButton.setButtonText(isMuted ? "Unmute" : "Mute");
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
    if (slider == &volumeSlider && !isMuted)
        playerAudio.setGain((float)volumeSlider.getValue());

    else if (slider == &posSlide && slider->isMouseButtonDown())
        playerAudio.setPosition(posSlide.getValue());
}

void PlayerGui::timerCallback()
{
    double currentPos = playerAudio.getPosition();
    double length = playerAudio.getLength();

    double a = positionSlider.getMinValue();
    double b = positionSlider.getMaxValue();

    if (playerAudio.isPlaying() && b > a && currentPos >= b)
        playerAudio.setPosition(a);

    if (!posSlide.isMouseButtonDown())
        posSlide.setValue(currentPos, juce::dontSendNotification);

    timeDisplayLabel.setText(formatTime(currentPos) + " / " + formatTime(length),
        juce::dontSendNotification);
}

int PlayerGui::getNumRows() { return playlistFiles.size(); }

void PlayerGui::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowIsSelected) g.fillAll(juce::Colours::lightblue);
    g.setColour(juce::Colours::black);
    if (rowNumber < playlistFiles.size())
        g.drawText(playlistFiles[rowNumber].getFileName(), 5, 0, width, height, juce::Justification::centredLeft);
}

void PlayerGui::selectedRowsChanged(int lastRowSelected)
{
    if (lastRowSelected >= 0 && lastRowSelected < playlistFiles.size())
    {
        playerAudio.LoadFile(playlistFiles[lastRowSelected]);
        updateMetadata(playlistFiles[lastRowSelected]);
    }
}

void PlayerGui::updateMetadata(const juce::File& file)
{
    juce::AudioFormatManager fmt;
    fmt.registerBasicFormats();
    if (auto* reader = fmt.createReaderFor(file))
    {
        juce::String info;
        auto title = reader->metadataValues["Title"];
        auto artist = reader->metadataValues["Artist"];
        double duration = reader->lengthInSamples / reader->sampleRate;

        if (title.isNotEmpty()) info += "Title: " + title + "   ";
        else info += "File: " + file.getFileName() + "   ";

        if (artist.isNotEmpty()) info += "Artist: " + artist + "   ";
        info += "Duration: " + juce::String(duration, 2) + " sec";

        metadataLabel.setText(info, juce::dontSendNotification);
        delete reader;
    }
    else
    {
        metadataLabel.setText("File: " + file.getFileName(), juce::dontSendNotification);
    }
}
