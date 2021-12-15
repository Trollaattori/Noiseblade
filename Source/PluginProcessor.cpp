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


#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
AMPAudioProcessor::AMPAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
        liveAudioScroller.reset (new LiveSpectrumAudioAnalyzer());
        //audioDeviceManager.addAudioCallback (liveAudioScroller.get());
        noiseGate.setRatio(3);
        noiseLimiter.setRelease(5);
        noiseLimiter.setAttack(5);
        noiseLimiter.setRatio(std::numeric_limits<float>::max());

#if 0

        juce::RuntimePermissions::request (juce::RuntimePermissions::recordAudio,
                                           [&] (bool granted) {
                                            setAudioChannels (granted ? 1 : 0, 1);
                                            audioDeviceManager.initialise (granted ? 1 : 0, 1, nullptr, true, {}, nullptr);
                                           });


        audioDeviceManager.addAudioCallback (liveAudioScroller.get());
#endif
}

AMPAudioProcessor::~AMPAudioProcessor()
{
}

//==============================================================================
const juce::String AMPAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AMPAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AMPAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AMPAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AMPAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AMPAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AMPAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AMPAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String AMPAudioProcessor::getProgramName (int index)
{
    return {};
}

void AMPAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void AMPAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    spec.sampleRate = sampleRate;
    spec.numChannels = 1;
    spec.maximumBlockSize = samplesPerBlock;
    noiseGate.prepare(spec);
    noiseLimiter.prepare(spec);
}

void AMPAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AMPAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AMPAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    LiveSpectrumAudioAnalyzer* lv = liveAudioScroller.get();

    const float* buffers[] = {buffer.getReadPointer(0)};
    lv->maunalAudioFeed(buffers,1,buffer.getNumSamples());

    if(lv->isReady() && !hasNoisefilter) {
        noiseGate.setThreshold(juce::Decibels::gainToDecibels<float>(lv->getAvg()) + noiseFloorGainSliderValue);
        noiseLimiter.setThreshold(juce::Decibels::gainToDecibels<float>(lv->getPeak()) + noiseCancellationLimiterValue);
        impbuffers[0] = lv->getFIR();
        impulseBuffer = juce::AudioBuffer<float>(impbuffers,1,1024);
        fir.loadImpulseResponse(std::move(impulseBuffer), spec.sampleRate, juce::dsp::Convolution::Stereo::no, juce::dsp::Convolution::Trim::no, juce::dsp::Convolution::Normalise::no );
        fir.prepare(spec);
        hasNoisefilter = true;
    }

    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::ProcessContextReplacing<float> context{ block };

    juce::AudioBuffer<float>* noiseF = new juce::AudioBuffer<float>(1, buffer.getNumSamples());
    juce::dsp::AudioBlock<float> blockN (*noiseF);
    juce::dsp::ProcessContextNonReplacing<float> contextD( block, blockN );

    juce::dsp::ProcessContextReplacing<float> contextN ( blockN );


    if (enableNoiseGate)
        noiseGate.process(context);

    float* wp = buffer.getWritePointer(0);

    if ( enableNoiseCancellation && hasNoisefilter) {
        fir.process(contextD);
        noiseLimiter.process(contextN);

        const float* rp = noiseF->getReadPointer(0);
        double coeff = noiseCancellationWetDryValue;
        for(int i=0;i<(buffer.getNumSamples());i++) {
                wp[i] -= coeff*rp[i];
        }
    }

    int channelFactor = enableStereoEnhancement ? -1 : 1;
    for(int i=1;i<buffer.getNumChannels();i++) {
        float* cp = buffer.getWritePointer(i);

        for(int i=0;i<buffer.getNumSamples();i++) {
            cp[i] = channelFactor*wp[i];
        }
    }

}

//==============================================================================
bool AMPAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AMPAudioProcessor::createEditor()
{
    return new AMPAudioProcessorEditor (*this);
}

//==============================================================================
void AMPAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AMPAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AMPAudioProcessor();
}
