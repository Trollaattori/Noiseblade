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


-- This file contains the processing part of the plugin. --

********************************************************************/

#pragma once

#include <JuceHeader.h>
#include "AudioLiveSpectrumAnalyzer.h"
//==============================================================================
/**
*/
class AMPAudioProcessor  : public juce::AudioProcessor
{
    friend class AMPAudioProcessorEditor;

public:
    //==============================================================================
    AMPAudioProcessor();
    ~AMPAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::unique_ptr<LiveSpectrumAudioAnalyzer> liveAudioScroller;

private:
    juce::dsp::NoiseGate<float> noiseGate;
    juce::dsp::Convolution fir;
    juce::dsp::Compressor<float> noiseLimiter;
    juce::dsp::ProcessSpec spec;

    float* impbuffers[1];
    juce::AudioBuffer<float> impulseBuffer;

    juce::AudioDeviceManager audioDeviceManager;
    bool hasNoisefilter = false;


    juce::AudioParameterFloat* noiseFloorGainSliderValue; //= 5.0f;
    juce::AudioParameterFloat* noiseCancellationWetDryValue; // = 1.0f;
    juce::AudioParameterFloat* noiseCancellationLimiterValue; //= 0.0f;

    juce::AudioParameterFloat* noiseFloorAttackSliderValue; //= 2.0f;
    juce::AudioParameterFloat* noiseFloorReleaseSliderValue; //= 25.0f;

    juce::AudioParameterBool* enableNoiseGate; //= true;
    juce::AudioParameterBool* enableNoiseCancellation; //= true;
    juce::AudioParameterBool* enableStereoEnhancement; // = false;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AMPAudioProcessor)
};
