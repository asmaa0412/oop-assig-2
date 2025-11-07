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
    void paint(juce::Graphics&) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& buffer);
    void releaseResources();

private:
    PlayerAudio playerAudio;

    juce::TextButton loadButton{ "Load File" };
    juce::TextButton playButton{ "Play" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton goToEndButton{ "End" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton loopButton{ "Loop" };
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton clearLoopButton{ "Clear Loop" };

    juce::Slider volumeSlider;
    juce::Slider speedSlider;
    juce::Slider positionSlider;
    juce::Slider progressBar;

    juce::Label timeDisplayLabel;
    juce::Label metadataLabel;
    juce::Label loopLabel;
    juce::Label progressLabel;

    juce::ListBox playlistBox{ "Playlist", this };
    juce::Array<juce::File> playlistFiles;

    std::unique_ptr<juce::FileChooser> fileChooser;
    double savedPosition = 0.0;
    bool isMuted = false;
    bool fileLoaded = false;
    float lastVolume = 0.7f;
    bool isProgressBarDragging = false;

    bool isPaused = false;
    double pausePosition = 0.0;

    double loopStart = 0.0;
    double loopEnd = 0.0;
    bool isLoopSegmentSet = false;
    bool isLoopingSegment = false;

    juce::AudioFormatManager thumbnailFormatManager;
    juce::AudioThumbnailCache thumbnailCache{ 5 };
    juce::AudioThumbnail thumbnail{ 512, thumbnailFormatManager, thumbnailCache };

    juce::Colour bgColor{ juce::Colour::fromRGB(30, 33, 40) };
    juce::Colour primaryColor{ juce::Colour::fromRGB(97, 175, 239) };
    juce::Colour accentColor{ juce::Colour::fromRGB(255, 159, 67) };
    juce::Colour waveColor{ juce::Colour::fromRGB(120, 195, 255) };
    juce::Colour textColor{ juce::Colour::fromRGB(240, 240, 240) };
    juce::Colour loopColor{ juce::Colour::fromRGB(255, 100, 100) };

    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    void sliderDragStarted(juce::Slider* slider) override;
    void sliderDragEnded(juce::Slider* slider) override;
    void timerCallback() override;

    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void selectedRowsChanged(int lastRowSelected) override;
    void updateMetadata(const juce::File& file);
    juce::String formatTime(double seconds);
    void loadWaveform(const juce::File& file);
    void loadFileIntoPlayer(const juce::File& file);
    void updateLoopDisplay();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGui)
};
