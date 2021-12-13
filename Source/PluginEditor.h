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


#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class AMPAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Slider::Listener, public juce::Button::Listener
{
public:
    AMPAudioProcessorEditor (AMPAudioProcessor&);
    ~AMPAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
	void sliderValueChanged(juce::Slider* slider) override;
    void buttonClicked(juce::Button* slider) override {};
	void buttonStateChanged(juce::Button* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AMPAudioProcessor& audioProcessor;

    juce::GroupComponent noiseGateGroup;
    juce::GroupComponent noiseCancellationGroup;
    juce::GroupComponent presetsGroup;

    juce::TextButton noiseGateButton;
    juce::TextButton noiseCancellationButton;

    juce::Slider noiseFloorGainSlider;
    juce::Slider noiseFloorAttackSlider;
    juce::Slider noiseFloorReleaseSlider;

    juce::Label noiseFloorGainLabel;
    juce::Label noiseFloorAttackLabel;
    juce::Label noiseFloorReleaseLabel;

    juce::Slider noiseCancellationAmountSlider;
    juce::Slider noiseCancellationWetDrySlider;

    juce::Label noiseCancellationAmountLabel;
    juce::Label noiseCancellationWetDryLabel;

    juce::Label presetLabel;
    juce::TextButton loadButton;
    juce::TextButton saveButton;
    juce::TextButton calibrateButton;

    juce::ToggleButton stereoEnhancementButton;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AMPAudioProcessorEditor)
};
