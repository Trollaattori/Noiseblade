/********************************************************************

Noiseblade - Noise Cancellation tool
Copyright (C) 2021  by Trollaattori

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


-- This file contains the GUI part of the plugin. --

********************************************************************/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AMPAudioProcessorEditor::AMPAudioProcessorEditor (AMPAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setOpaque (true);

    audioProcessor.noiseGate.setAttack(audioProcessor.noiseFloorAttackSliderValue);
    audioProcessor.noiseGate.setRelease(audioProcessor.noiseFloorReleaseSliderValue);
    audioProcessor.noiseGate.setRatio(3);



            getLookAndFeel().setColour (juce::ResizableWindow::backgroundColourId, juce::Colour(0x11,0x00,0x05));
            getLookAndFeel().setColour (juce::TextButton::buttonColourId , juce::Colour(0x33,0x00,0x11));
            getLookAndFeel().setColour (juce::TextButton::buttonOnColourId , juce::Colour(0xff,0x99,0xaa));
            getLookAndFeel().setColour (juce::TextButton::textColourOffId , juce::Colour(0xff,0x77,0xaa));
            getLookAndFeel().setColour (juce::TextButton::textColourOnId , juce::Colours::black);
            getLookAndFeel().setColour (juce::Label::textColourId, juce::Colour(0xff,0x99,0xaa));

            getLookAndFeel().setColour (juce::Slider::thumbColourId, juce::Colour(0xee,0x30,0x88));

            getLookAndFeel().setColour (juce::Slider::rotarySliderFillColourId, juce::Colour(0x77,0x33,0x44));
            getLookAndFeel().setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(0x88,0x44,0x55));

            getLookAndFeel().setColour (juce::GroupComponent::outlineColourId, juce::Colour(0xff,0x99,0xaa));
            getLookAndFeel().setColour (juce::GroupComponent::textColourId, juce::Colour(0xff,0x99,0xaa));



    addAndMakeVisible (audioProcessor.liveAudioScroller.get());
    addAndMakeVisible (noiseGateGroup);
    addAndMakeVisible (noiseCancellationGroup);
    addAndMakeVisible (presetsGroup);

    noiseFloorGainSlider.setRange(-10,10);
    addAndMakeVisible (noiseFloorGainSlider);
    noiseFloorGainSlider.setValue(audioProcessor.noiseFloorGainSliderValue);
    noiseFloorGainSlider.addListener(this);
    noiseFloorGainSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    noiseFloorGainSlider.setName("noiseFloorGainSlider");
    addAndMakeVisible (noiseFloorAttackSlider);
    noiseFloorAttackSlider.setName("noiseFloorAttackSlider");
    noiseFloorAttackSlider.setValue(audioProcessor.noiseFloorAttackSliderValue);
    noiseFloorAttackSlider.setRange(1.0,20.0);
    noiseFloorAttackSlider.addListener(this);
    noiseFloorAttackSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    addAndMakeVisible (noiseFloorReleaseSlider);
    noiseFloorReleaseSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    noiseFloorReleaseSlider.setName("noiseFloorReleaseSlider");
    noiseFloorReleaseSlider.setRange(1.0,100.0);
    noiseFloorReleaseSlider.setValue(audioProcessor.noiseFloorReleaseSliderValue);
    noiseFloorReleaseSlider.addListener(this);

    noiseGateButton.getToggleStateValue().setValue(audioProcessor.enableNoiseGate);
    noiseGateButton.addListener(this);
    noiseCancellationButton.getToggleStateValue().setValue(audioProcessor.enableNoiseCancellation);
    noiseCancellationButton.addListener(this);

    noiseGateButton.setClickingTogglesState(true);
    noiseCancellationButton.setClickingTogglesState(true);


    addAndMakeVisible(noiseGateButton);
    addAndMakeVisible(noiseCancellationButton);

    noiseGateButton.setButtonText("Enable");
    noiseGateButton.setName("noiseGateButton");
    noiseCancellationButton.setButtonText("Enable");
    noiseCancellationButton.setName("noiseCancellationButton");

    addAndMakeVisible (noiseFloorGainLabel);
    noiseFloorGainLabel.setText("Noise Floor Gain:",juce::NotificationType::dontSendNotification);
    addAndMakeVisible (noiseFloorAttackLabel);
    noiseFloorAttackLabel.setText("Attack:",juce::NotificationType::dontSendNotification);
    addAndMakeVisible (noiseFloorReleaseLabel);
    noiseFloorReleaseLabel.setText("Release:",juce::NotificationType::dontSendNotification);

    addAndMakeVisible (noiseCancellationWetDrySlider);
    noiseCancellationWetDrySlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    noiseCancellationWetDrySlider.setName("noiseCancellationWetDrySlider");
    noiseCancellationWetDrySlider.setValue(1.0);
    noiseCancellationWetDrySlider.setRange(0.0,1.0);
    addAndMakeVisible (noiseCancellationAmountSlider);
    noiseCancellationAmountSlider.setValue(audioProcessor.noiseCancellationLimiterValue);
    noiseCancellationAmountSlider.setRange(-100.0,100.0);
    noiseCancellationAmountSlider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
    noiseCancellationAmountSlider.setName("noiseCancellationAmountSlider");

    noiseCancellationAmountSlider.addListener(this);
    noiseCancellationWetDrySlider.addListener(this);


    addAndMakeVisible (noiseCancellationAmountLabel);
    noiseCancellationAmountLabel.setText("Noise Cancellation Gain:",juce::NotificationType::dontSendNotification);
    addAndMakeVisible (noiseCancellationWetDryLabel);
    noiseCancellationWetDryLabel.setText("Wet/Dry:",juce::NotificationType::dontSendNotification);

    noiseGateGroup.setText("Noise Gate");
    noiseCancellationGroup.setText("Noise Cancellation");
    presetsGroup.setText("Presets");

    addAndMakeVisible(loadButton);

    loadButton.onClick = [&]() {
        audioProcessor.liveAudioScroller.get()->showLoadDialog();
        //hasNoisefilter = false;
    };
    loadButton.setButtonText("Load...");
    loadButton.setConnectedEdges(juce::Button::ConnectedOnRight);
    addAndMakeVisible(saveButton);
    saveButton.setButtonText("Save...");
    saveButton.setConnectedEdges(juce::Button::ConnectedOnRight | juce::Button::ConnectedOnLeft);
    saveButton.onClick = [&]() {
        audioProcessor.liveAudioScroller.get()->showSaveDialog();
    };
    addAndMakeVisible(calibrateButton);
    calibrateButton.setButtonText("Calibrate...");
    calibrateButton.setConnectedEdges(juce::Button::ConnectedOnLeft);
    calibrateButton.onClick = [&] {
        audioProcessor.hasNoisefilter = false;
        this->audioProcessor.liveAudioScroller.get()->resetAccCounter();
    };

    addAndMakeVisible(presetLabel);
    juce::String presetN = audioProcessor.liveAudioScroller.get()->getPresetName();
    if (presetN == "")
        presetLabel.setText("Preset: <not saved>",juce::NotificationType::dontSendNotification);
    else
        presetLabel.setText("Preset: " + presetN,juce::NotificationType::dontSendNotification);

    addAndMakeVisible(stereoEnhancementButton);
    stereoEnhancementButton.setButtonText("Stereo Enhancement");
    stereoEnhancementButton.setName("stereoEnhancementButton");
    stereoEnhancementButton.getToggleStateValue().setValue(audioProcessor.enableStereoEnhancement);
    stereoEnhancementButton.setClickingTogglesState(true);
    stereoEnhancementButton.addListener(this);

    audioProcessor.liveAudioScroller.get()->setCallback([&](){
        auto* lv = audioProcessor.liveAudioScroller.get();
        presetLabel.setText("Preset: "+ lv->getPresetName(),juce::NotificationType::dontSendNotification);
        audioProcessor.hasNoisefilter = false;
        });


#if 0
        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) {
                                            setAudioChannels (granted ? 2 : 0, 2);
                                            audioDeviceManager.initialise (granted ? 2 : 0, 2, nullptr, true, {}, nullptr);
                                           });


        audioDeviceManager.addAudioCallback (liveAudioScroller.get());
#endif

        setSize (800, 600);

        #ifdef REMOVE_SPLASH // Note: You can't redistribute binaries, if JUCE does not a paid license
            this->removeChildComponent(this->getNumChildComponents() - 1);
        #endif


}

AMPAudioProcessorEditor::~AMPAudioProcessorEditor()
{
}

void AMPAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    LiveSpectrumAudioAnalyzer* lv = audioProcessor.liveAudioScroller.get();

    juce::String name = slider->getName();

    if (name == "noiseFloorGainSlider") {
        audioProcessor.noiseFloorGainSliderValue = noiseFloorGainSlider.getValue();
        audioProcessor.noiseGate.setThreshold(juce::Decibels::gainToDecibels<float>(lv->getAvg()) + noiseFloorGainSlider.getValue());
    } else if (name == "noiseFloorAttackSlider") {
        audioProcessor.noiseFloorAttackSliderValue = noiseFloorAttackSlider.getValue();
        audioProcessor.noiseGate.setAttack(noiseFloorAttackSlider.getValue());
    } else if (name == "noiseFloorReleaseSlider") {
        audioProcessor.noiseFloorReleaseSliderValue = noiseFloorReleaseSlider.getValue();
        audioProcessor.noiseGate.setRelease(noiseFloorReleaseSlider.getValue());
    } else if (name == "noiseCancellationWetDrySlider") {
        audioProcessor.noiseCancellationWetDryValue = noiseCancellationWetDrySlider.getValue();
    } else if (name == "noiseCancellationAmountSlider") {
        audioProcessor.noiseCancellationLimiterValue = noiseCancellationAmountSlider.getValue();
        audioProcessor.noiseLimiter.setThreshold(juce::Decibels::gainToDecibels<float>(lv->getPeak()) + audioProcessor.noiseCancellationLimiterValue);
    }
}

void AMPAudioProcessorEditor::buttonStateChanged(juce::Button* button) {

    juce::String name = button->getName();

    if ( name == "noiseCancellationButton" ) {
        audioProcessor.enableNoiseCancellation = noiseCancellationButton.getToggleState();
    } else if (name == "noiseGateButton") {
        audioProcessor.enableNoiseGate = noiseGateButton.getToggleState();
    } else if (name == "stereoEnhancementButton") {
        audioProcessor.enableStereoEnhancement = stereoEnhancementButton.getToggleState();
    }
}


//==============================================================================
void AMPAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void AMPAudioProcessorEditor::resized()
{
    audioProcessor.liveAudioScroller.get()->setBounds(0,0,getWidth(),getHeight()/2);

    juce::Rectangle<int> area (0,getHeight()/2,getWidth(),getHeight()/2);

    noiseGateGroup.setBounds(area.removeFromLeft(150));
    juce::Rectangle<int> areaGate = noiseGateGroup.getBounds();
    areaGate.removeFromLeft(10);
    areaGate.removeFromRight(10);
    areaGate.removeFromTop(30);

    noiseGateButton.setBounds(areaGate.removeFromTop(30));

    areaGate.removeFromTop(20);

    noiseFloorGainLabel.setBounds(areaGate.removeFromTop(15));
    noiseFloorGainSlider.setBounds(areaGate.removeFromTop(65));

    noiseFloorAttackLabel.setBounds(areaGate.removeFromTop(15));
    noiseFloorAttackSlider.setBounds(areaGate.removeFromTop(65));

    noiseFloorReleaseLabel.setBounds(areaGate.removeFromTop(15));
    noiseFloorReleaseSlider.setBounds(areaGate.removeFromTop(65));


    noiseCancellationGroup.setBounds(area.removeFromRight(150));
    juce::Rectangle<int> areaCancellation = noiseCancellationGroup.getBounds();
    areaCancellation.removeFromLeft(10);
    areaCancellation.removeFromRight(10);
    areaCancellation.removeFromTop(30);

    noiseCancellationButton.setBounds(areaCancellation.removeFromTop(30));

    areaCancellation.removeFromTop(20);


    noiseCancellationAmountLabel.setBounds(areaCancellation.removeFromTop(15));
    noiseCancellationAmountSlider.setBounds(areaCancellation.removeFromTop(65));

    noiseCancellationWetDryLabel.setBounds(areaCancellation.removeFromTop(15));
    noiseCancellationWetDrySlider.setBounds(areaCancellation.removeFromTop(65));

    presetsGroup.setBounds(area);
    area.removeFromLeft(10);
    area.removeFromRight(10);
    area.removeFromTop(30);
    juce::Rectangle<int> areaPreset = area.removeFromTop(30);
    calibrateButton.setBounds(areaPreset.removeFromRight(70));
    saveButton.setBounds(areaPreset.removeFromRight(70));
    loadButton.setBounds(areaPreset.removeFromRight(70));
    presetLabel.setBounds(areaPreset);

    area.removeFromTop(20);
    stereoEnhancementButton.setBounds(area.removeFromTop(30));
}
