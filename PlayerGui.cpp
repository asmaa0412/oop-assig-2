#include "PlayerGui.h"

juce::String PlayerGui::formatTime(double seconds) {
    if (seconds < 0.0) seconds = 0.0;
    int totalSeconds = (int)std::round(seconds);
    int minutes = totalSeconds / 60;
    int remainingSeconds = totalSeconds % 60;
    return juce::String::formatted("%02d:%02d", minutes, remainingSeconds);
}

PlayerGui::PlayerGui() {
    thumbnailFormatManager.registerBasicFormats();

    addAndMakeVisible(progressBar);
    progressBar.setRange(0.0, 1.0, 0.001);
    progressBar.setValue(0.0);
    progressBar.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    progressBar.setColour(juce::Slider::thumbColourId, accentColor);
    progressBar.setColour(juce::Slider::trackColourId, primaryColor);
    progressBar.setColour(juce::Slider::backgroundColourId, bgColor.brighter(0.1f));

    addAndMakeVisible(progressLabel);
    progressLabel.setJustificationType(juce::Justification::centred);
    progressLabel.setFont(juce::Font(14.0f, juce::Font::bold));
    progressLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    progressLabel.setColour(juce::Label::textColourId, textColor);

    juce::TextButton* buttons[] = {
        &loadButton, &playButton, &pauseButton, &restartButton,
        &goToEndButton, &muteButton, &loopButton, &setAButton,
        &setBButton, &clearLoopButton
    };

    for (auto* btn : buttons) {
        addAndMakeVisible(btn);
        btn->addListener(this);
        btn->setColour(juce::TextButton::buttonColourId, primaryColor);
        btn->setColour(juce::TextButton::textColourOffId, textColor);
        btn->setColour(juce::TextButton::buttonOnColourId, accentColor);
    }

    addAndMakeVisible(volumeSlider);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.7);
    volumeSlider.addListener(this);
    volumeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    volumeSlider.setColour(juce::Slider::thumbColourId, accentColor);
    volumeSlider.setColour(juce::Slider::trackColourId, primaryColor);
    volumeSlider.setColour(juce::Slider::textBoxTextColourId, textColor);

    addAndMakeVisible(speedSlider);
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 60, 20);
    speedSlider.setColour(juce::Slider::thumbColourId, accentColor);
    speedSlider.setColour(juce::Slider::trackColourId, primaryColor);
    speedSlider.setColour(juce::Slider::textBoxTextColourId, textColor);

    addAndMakeVisible(positionSlider);
    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.setValue(0.0);
    positionSlider.addListener(this);
    positionSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    positionSlider.setColour(juce::Slider::thumbColourId, accentColor);
    positionSlider.setColour(juce::Slider::trackColourId, primaryColor);

    addAndMakeVisible(timeDisplayLabel);
    timeDisplayLabel.setJustificationType(juce::Justification::centred);
    timeDisplayLabel.setFont(juce::Font(16.0f, juce::Font::bold));
    timeDisplayLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    timeDisplayLabel.setColour(juce::Label::textColourId, textColor);

    addAndMakeVisible(metadataLabel);
    metadataLabel.setJustificationType(juce::Justification::centredLeft);
    metadataLabel.setText("No file loaded", juce::dontSendNotification);
    metadataLabel.setColour(juce::Label::textColourId, textColor);

    addAndMakeVisible(loopLabel);
    loopLabel.setJustificationType(juce::Justification::centred);
    loopLabel.setFont(juce::Font(12.0f, juce::Font::plain));
    loopLabel.setText("Loop: Not Set", juce::dontSendNotification);
    loopLabel.setColour(juce::Label::textColourId, textColor);

    addAndMakeVisible(playlistBox);
    playlistBox.setRowHeight(25);
    playlistBox.setModel(this);
    playlistBox.setColour(juce::ListBox::backgroundColourId, bgColor.brighter(0.1f));

    startTimerHz(30);
}

PlayerGui::~PlayerGui() {
    stopTimer();
}

void PlayerGui::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGui::getNextAudioBlock(const juce::AudioSourceChannelInfo& buffer) {
    playerAudio.getNextAudioBlock(buffer);
}

void PlayerGui::releaseResources() {
    playerAudio.releaseResources();
    thumbnail.clear();
    fileLoaded = false;
}

void PlayerGui::resized() {
    auto area = getLocalBounds();

    auto headerArea = area.removeFromTop(40);
    auto controlsArea = area.removeFromTop(200);

    auto topRow = controlsArea.removeFromTop(40);
    volumeSlider.setBounds(topRow.removeFromLeft(getWidth() / 2 - 10).reduced(10, 5));
    speedSlider.setBounds(topRow.reduced(10, 5));

    auto buttonRow1 = controlsArea.removeFromTop(50);
    int buttonWidth = getWidth() / 7;
    loadButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    playButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    pauseButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    restartButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    goToEndButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    muteButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));
    loopButton.setBounds(buttonRow1.removeFromLeft(buttonWidth).reduced(3));

    auto buttonRow2 = controlsArea.removeFromTop(50);
    int loopButtonWidth = getWidth() / 4;
    setAButton.setBounds(buttonRow2.removeFromLeft(loopButtonWidth).reduced(3));
    setBButton.setBounds(buttonRow2.removeFromLeft(loopButtonWidth).reduced(3));
    clearLoopButton.setBounds(buttonRow2.removeFromLeft(loopButtonWidth).reduced(3));

    controlsArea.removeFromTop(5);
    loopLabel.setBounds(controlsArea.removeFromTop(25).reduced(10, 0));

    controlsArea.removeFromTop(10);
    auto waveformArea = controlsArea.removeFromTop(100);

    controlsArea.removeFromTop(10);
    auto bottomRow = controlsArea.removeFromTop(40);
    positionSlider.setBounds(bottomRow.removeFromLeft(getWidth() - 120).reduced(10, 5));
    timeDisplayLabel.setBounds(bottomRow.reduced(10, 5));

    area.removeFromTop(10);
    metadataLabel.setBounds(area.removeFromTop(25).reduced(10, 5));

    auto progressArea = area.removeFromTop(50).reduced(10, 5);
    progressBar.setBounds(progressArea.removeFromTop(20));
    progressLabel.setBounds(progressArea.removeFromTop(25));

    area.removeFromTop(5);
    playlistBox.setBounds(area.reduced(10));
}

void PlayerGui::paint(juce::Graphics& g) {
    g.setGradientFill(juce::ColourGradient(bgColor, 0, 0,
        bgColor.darker(0.5f), 0, getHeight(), false));
    g.fillAll();

    auto headerArea = getLocalBounds().removeFromTop(40);
    g.setColour(primaryColor.withAlpha(0.8f));
    g.fillRect(headerArea);
    g.setColour(textColor);
    g.setFont(juce::Font(20.0f, juce::Font::bold));
    g.drawText("Audio Player Pro", headerArea, juce::Justification::centred);

    auto waveformArea = getLocalBounds().withHeight(100).withY(180);
    g.setColour(bgColor.brighter(0.1f));
    g.fillRect(waveformArea);

    if (fileLoaded && thumbnail.getNumChannels() > 0 && thumbnail.getTotalLength() > 0.0) {
        g.setColour(waveColor.withAlpha(0.7f));
        thumbnail.drawChannels(g, waveformArea.reduced(2), 0.0, thumbnail.getTotalLength(), 1.0f);

        double proportion = (playerAudio.getLength() > 0.0) ?
            (playerAudio.getPosition() / playerAudio.getLength()) : 0.0;
        int x = waveformArea.getX() + (int)(proportion * waveformArea.getWidth());
        g.setColour(accentColor);
        g.drawLine((float)x, (float)waveformArea.getY(), (float)x, (float)waveformArea.getBottom(), 3.0f);

        if (isLoopSegmentSet && loopEnd > loopStart) {
            double startProportion = loopStart / playerAudio.getLength();
            double endProportion = loopEnd / playerAudio.getLength();
            int startX = waveformArea.getX() + (int)(startProportion * waveformArea.getWidth());
            int endX = waveformArea.getX() + (int)(endProportion * waveformArea.getWidth());

            g.setColour(loopColor.withAlpha(0.3f));
            g.fillRect(startX, waveformArea.getY(), endX - startX, waveformArea.getHeight());

            g.setColour(loopColor);
            g.drawLine((float)startX, (float)waveformArea.getY(), (float)startX, (float)waveformArea.getBottom(), 2.0f);
            g.drawLine((float)endX, (float)waveformArea.getY(), (float)endX, (float)waveformArea.getBottom(), 2.0f);

            g.setFont(juce::Font(10.0f, juce::Font::bold));
            g.drawText("A", startX - 10, waveformArea.getBottom() - 15, 20, 15, juce::Justification::centred);
            g.drawText("B", endX - 10, waveformArea.getBottom() - 15, 20, 15, juce::Justification::centred);
        }
    }
    else {
        g.setColour(textColor.withAlpha(0.5f));
        g.drawFittedText("Load a file to see waveform", waveformArea, juce::Justification::centred, 1);
    }
}

void PlayerGui::buttonClicked(juce::Button* button) {
    if (button == &loadButton) {
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file",
            juce::File::getSpecialLocation(juce::File::userDesktopDirectory),
            "*");

        auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

        fileChooser->launchAsync(folderChooserFlags, [this](const juce::FileChooser& chooser) {
            auto file = chooser.getResult();
            if (file.existsAsFile()) {
                loadFileIntoPlayer(file);

                if (!playlistFiles.contains(file)) {
                    playlistFiles.add(file);
                    playlistBox.updateContent();
                }
            }
            });
    }
    else if (button == &playButton) {
        if (fileLoaded) {
            playerAudio.play();
        }
    }
    else if (button == &pauseButton) {
        playerAudio.stop();
    }
    else if (button == &restartButton) {
        if (fileLoaded) {
            playerAudio.setPosition(0.0);
        }
    }
    else if (button == &goToEndButton) {
        if (fileLoaded) {
            playerAudio.setPosition(playerAudio.getLength());
        }
    }
    else if (button == &muteButton) {
        isMuted = !isMuted;
        if (isMuted) {
            lastVolume = volumeSlider.getValue();
            playerAudio.setGain(0.0f);
        }
        else {
            playerAudio.setGain(lastVolume);
        }
        muteButton.setButtonText(isMuted ? "Unmute" : "Mute");
        muteButton.setToggleState(isMuted, juce::dontSendNotification);
    }
    else if (button == &loopButton) {
        bool shouldLoop = !playerAudio.getIsLooping();
        playerAudio.setLooping(shouldLoop);
        loopButton.setToggleState(shouldLoop, juce::dontSendNotification);
        loopButton.setButtonText(shouldLoop ? "Loop On" : "Loop");
    }
    else if (button == &setAButton) {
        if (fileLoaded) {
            loopStart = playerAudio.getPosition();
            isLoopSegmentSet = true;
            if (loopEnd > loopStart) {
                isLoopingSegment = true;
            }
            updateLoopDisplay();
        }
    }
    else if (button == &setBButton) {
        if (fileLoaded) {
            loopEnd = playerAudio.getPosition();
            isLoopSegmentSet = true;
            if (loopEnd > loopStart) {
                isLoopingSegment = true;
            }
            updateLoopDisplay();
        }
    }
    else if (button == &clearLoopButton) {
        loopStart = 0.0;
        loopEnd = 0.0;
        isLoopSegmentSet = false;
        isLoopingSegment = false;
        updateLoopDisplay();
    }
}

void PlayerGui::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) {
        if (!isMuted) {
            playerAudio.setGain((float)volumeSlider.getValue());
            lastVolume = volumeSlider.getValue();
        }
    }
    else if (slider == &speedSlider) {
        playerAudio.setSpeed(speedSlider.getValue());
    }
    else if (slider == &positionSlider) {
        if (fileLoaded && playerAudio.getLength() > 0.0) {
            double newPosition = positionSlider.getValue() * playerAudio.getLength();
            playerAudio.setPosition(newPosition);
        }
    }
}

void PlayerGui::timerCallback() {
    if (fileLoaded && playerAudio.getLength() > 0.0) {
        double currentPos = playerAudio.getPosition();
        double totalLength = playerAudio.getLength();

        if (totalLength > 0.0) {
            positionSlider.setValue(currentPos / totalLength, juce::dontSendNotification);
            progressBar.setValue(currentPos / totalLength, juce::dontSendNotification);

            progressLabel.setText(
                formatTime(currentPos) + " / " + formatTime(totalLength),
                juce::dontSendNotification
            );

            timeDisplayLabel.setText(
                formatTime(currentPos) + " / " + formatTime(totalLength),
                juce::dontSendNotification
            );
        }

        if (isLoopSegmentSet && isLoopingSegment && loopEnd > loopStart) {
            if (currentPos >= loopEnd || currentPos < loopStart) {
                playerAudio.setPosition(loopStart);
            }
        }
    }
    else {
        timeDisplayLabel.setText("00:00 / 00:00", juce::dontSendNotification);
        progressBar.setValue(0.0, juce::dontSendNotification);
        progressLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    }

    playButton.setEnabled(fileLoaded && !playerAudio.isPlaying());
    pauseButton.setEnabled(fileLoaded && playerAudio.isPlaying());
    restartButton.setEnabled(fileLoaded);
    goToEndButton.setEnabled(fileLoaded);
    muteButton.setEnabled(fileLoaded);
    loopButton.setEnabled(fileLoaded);
    setAButton.setEnabled(fileLoaded);
    setBButton.setEnabled(fileLoaded);
    clearLoopButton.setEnabled(isLoopSegmentSet);

    repaint();
}

void PlayerGui::updateLoopDisplay() {
    if (isLoopSegmentSet) {
        juce::String loopText = "Loop: " + formatTime(loopStart) + " - " + formatTime(loopEnd);
        if (isLoopingSegment && loopEnd > loopStart) {
            loopText += " [ACTIVE]";
        }
        loopLabel.setText(loopText, juce::dontSendNotification);
    }
    else {
        loopLabel.setText("Loop: Not Set", juce::dontSendNotification);
    }
}

int PlayerGui::getNumRows() {
    return playlistFiles.size();
}

void PlayerGui::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) {
    if (rowIsSelected) {
        g.fillAll(primaryColor.withAlpha(0.3f));
    }
    else {
        g.fillAll(bgColor.brighter(0.1f));
    }

    g.setColour(textColor);
    g.setFont(juce::Font(14.0f));

    if (rowNumber < playlistFiles.size()) {
        g.drawText(playlistFiles[rowNumber].getFileName(),
            5, 0, width - 5, height,
            juce::Justification::centredLeft);
    }
}

void PlayerGui::selectedRowsChanged(int lastRowSelected) {
    if (lastRowSelected >= 0 && lastRowSelected < playlistFiles.size()) {
        loadFileIntoPlayer(playlistFiles[lastRowSelected]);
    }
}

void PlayerGui::updateMetadata(const juce::File& file) {
    juce::String metadata = "File: " + file.getFileName();
    metadata += " | Size: " + juce::String(file.getSize() / 1024) + " KB";
    metadata += " | Format: " + file.getFileExtension().toUpperCase();
    metadataLabel.setText(metadata, juce::dontSendNotification);
}

void PlayerGui::loadWaveform(const juce::File& file) {
    thumbnail.clear();
    if (file.existsAsFile()) {
        thumbnail.setSource(new juce::FileInputSource(file));
    }
}

void PlayerGui::loadFileIntoPlayer(const juce::File& file) {
    if (playerAudio.loadFile(file)) {
        fileLoaded = true;
        updateMetadata(file);
        loadWaveform(file);

        playerAudio.setGain((float)volumeSlider.getValue());
        playerAudio.setSpeed(speedSlider.getValue());

        loopStart = 0.0;
        loopEnd = 0.0;
        isLoopSegmentSet = false;
        isLoopingSegment = false;
        updateLoopDisplay();

        repaint();
    }
}
