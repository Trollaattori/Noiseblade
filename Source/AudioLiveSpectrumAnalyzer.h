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


-- This file contains the Spectrum analyzer part of the plugin. --
-- It is both; a widget and a signal processor.                 --

********************************************************************/

#pragma once

#include <math.h>
#include <algorithm>

#define BUF_OFFSET 0

#define CLAMP(val,lo,hi) \
    (val) < (lo) ? (lo) : (val) > (hi) ? (hi) : (val)

class LiveSpectrumAudioAnalyzer  : public juce::Component,
                                   public juce::AudioIODeviceCallback
{
public:
    LiveSpectrumAudioAnalyzer()  : buffer(1, 2048), fft(11), fileChooser("Noise Filter Profile",juce::File::getSpecialLocation (juce::File::userHomeDirectory),"*.fir")
    {

        memset(fftbuffer,0,sizeof(fftbuffer));
        memset(accbuffer,0,sizeof(accbuffer));
        memset(filterbuffer,0,sizeof(filterbuffer));
        memset(accinpbuffer,0,sizeof(accinpbuffer));

    }

    //==============================================================================
    void audioDeviceAboutToStart (juce::AudioIODevice*) override
    {
    }

    void audioDeviceStopped() override
    {
    }

    float getAvg() {
        return avg;
    }


    float getPeak() {
        return peak;
    }

    void paint (juce::Graphics& g)
    {
        g.fillAll (juce::Colour(0x33,0x00,0x20));

        juce::Path myPath,accPath,filterPath;
        float h = (float)getHeight();
        float w = (float)getWidth();
        bool decibels = true;
        bool drawAcc = true;

        myPath.startNewSubPath (0.0f, h);
        filterPath.startNewSubPath (0.0f, h);

        if (drawAcc)
            accPath.startNewSubPath (0.0f, h);

        for (int i=0;i<512;i++) {
            float val = 0.0f;
            val += fftbuffer[BUF_OFFSET+i*2];
            val += fftbuffer[BUF_OFFSET+i*2+1];

            val /= 2.0f;
            if (decibels) {
                myPath.lineTo (w/512.0 * i, h * (0.5f-0.2f*log10(val/1.0f)));
                if (drawAcc)
                    accPath.lineTo(w/512.0 * i, h * (0.5f-0.2f*log10(accbuffer[i*2]/1.0f)));
            } else {
                myPath.lineTo  (w/512.0 * i, h * (1.0f-(val/25.0f)));
                if (drawAcc)
                    accPath.lineTo (w/512.0 * i, h * (1.0f-(accbuffer[i*2]/25.0f)));

            }
                filterPath.lineTo (w/512.0 * i, h * (1.0f-(filterbuffer[i*2])));
        }
        myPath.lineTo (w, h);
        filterPath.lineTo (w, h);

        if (drawAcc)
            accPath.lineTo (w, h);

        myPath.closeSubPath();
        filterPath.closeSubPath();

        if (drawAcc)
            accPath.closeSubPath();


        g.setColour(juce::Colour(0xff,0x00,0xaa));
        g.strokePath (myPath, juce::PathStrokeType (2.0f));
        g.setGradientFill(juce::ColourGradient(juce::Colour(0xaa,0x00,0x55),0,getHeight()/3.0f * 2.0f,juce::Colour(0x55,0x00,0x33),0,getHeight(),false));
        g.fillPath(myPath);

        if (drawAcc){
            g.setColour(juce::Colour(0xaa,0x00,0xff));
            g.strokePath (accPath, juce::PathStrokeType (1.0f));
            g.setGradientFill(juce::ColourGradient(juce::Colour(0x5500aa77),0,getHeight()/3.0f * 2.0f,juce::Colour(0x33005577),0,getHeight(),false));
            g.fillPath(accPath);
        }

        g.setColour(juce::Colour(0xff,0xff,0xff));
        g.strokePath (filterPath, juce::PathStrokeType (1.0f));
        g.setGradientFill(juce::ColourGradient(juce::Colour(0x33ffffff),0,getHeight()/3.0f * 2.0f,juce::Colour(0x77ffffff),0,getHeight(),false));
        g.fillPath(filterPath);


        // You can add your drawing code here!
    }

    void setupImpulseResponse() {
        std::array<float, 1024> a1;
        std::array<float, 1024> b1;

        for (int i=0;i<1024;i++) {
            a1[i] = accbuffer[i];
        }

        for(int i=0;i<1024;i++)
        b1[i] = CLAMP(a1[i]*std::abs(2.0*a1[i] - a1[CLAMP(i-1,0,1023)] - a1[CLAMP(i+1,0,1023)]),0.0,1.0) > 0.002 ? 1.0 : 0.001;

        for(int i=0;i<1024;i++)
        filterbuffer[i] = CLAMP((b1[i] + 0.5*b1[CLAMP(i-1,0,1023)] + 0.5*b1[CLAMP(i+1,0,1023)]),0.0,1.0);

        std::sort(a1.begin(),a1.end());
        avg = a1[512];
        peak = a1[1023];


        //for(int i=0;i<1024;i++)
        //    b1[i] = (b1[i] > (avg/2)) ? 4*b1[i] : 0.00001;

        memcpy(accinpbuffer,filterbuffer,1024*sizeof(float));

        for(int i=0;i<1024;i++)
        accinpbuffer[1024+i] = filterbuffer[1024-i];

        fft.performRealOnlyInverseTransform(&accinpbuffer[0]);
        ready = true;

    }

void maunalAudioFeed (const float** inputChannelData, int numInputChannels,
                                int numberOfSamples)
    {
        for (int i = 0; i < numberOfSamples; ++i)
        {
            float inputSample = 0;

            for (int chan = 0; chan < numInputChannels; ++chan)
                if (const float* inputChannel = inputChannelData[chan])
                    inputSample += inputChannel[i];  // find the sum of all the channels

            (buffer.getWritePointer(0,cursor++))[0] = inputSample;
            if(cursor >= 2048) {
                cursor = 0;
                memcpy(fftbuffer,buffer.getReadPointer(0),sizeof(float)*2048);
                fft.performFrequencyOnlyForwardTransform(&fftbuffer[0]);

                if (accsamples < 64) {
                    if (accsamples > 8) {
                        for (int i=0;i<512;i++) {
                            accbuffer[i*2] = ((accsamples > 10) ? 0.95f : 0.0f) * accbuffer[i*2] + (0.05f) * fftbuffer[BUF_OFFSET+i*2];
                            accbuffer[i*2+1] = ((accsamples > 10) ? 0.95f : 0.0f) * accbuffer[i*2+1] + (0.05f) * fftbuffer[BUF_OFFSET+i*2+1];
                        }
                        accsamples++;
                    } else {
                        accsamples++;
                    }
                } else if (accsamples == 64) {
                    accsamples++;
                    setupImpulseResponse();
                }

                repaint();
                buffer.clear();
            }

        }
    }

    void audioDeviceIOCallback (const float** inputChannelData, int numInputChannels,
                                float** outputChannelData, int numOutputChannels,
                                int numberOfSamples) override
    {
        maunalAudioFeed(inputChannelData,numInputChannels,numberOfSamples);
        // We need to clear the output buffers before returning, in case they're full of junk..
        for (int j = 0; j < numOutputChannels; ++j)
            if (float* outputChannel = outputChannelData[j])
                juce::zeromem (outputChannel, (size_t) numberOfSamples * sizeof (float));
    }

    float* getFIR() {
        return &accinpbuffer[0];
    }

    void resetAccCounter() {
        accsamples = 0;
        avg = 0.0f;
        peak = 0.0f;
        ready = false;
    }

void showLoadDialog() {

    fileChooser.launchAsync(juce::FileBrowserComponent::FileChooserFlags::openMode, [&](const juce::FileChooser& fc) {
                                if (! fc.getResult().existsAsFile())
                                    return;

                                if(fc.getResult().getSize() == (sizeof(float) * 1024)) {
                                    ready = false;
                                    auto result = fc.getResult();
                                    juce::FileInputStream inputStream (result);
                                    for(int i=0;i<1024;i++)
                                        accbuffer[i] = inputStream.readFloat();
                                    setupImpulseResponse();
                                    accsamples = 65; // XXX: set to the max to avoid calibrating
                                    presetChangedCallback();

                                }

                            });
}

bool isReady() {

    return ready;
}

void showSaveDialog() {

    fileChooser.launchAsync(juce::FileBrowserComponent::FileChooserFlags::saveMode, [&](const juce::FileChooser& fc) {
                                if (! fc.getResult().hasWriteAccess())
                                    return;
                                if (fc.getResult().existsAsFile())
                                    fc.getResult().replaceWithData ("",0);

                                juce::FileOutputStream outputStream (fc.getResult());
                                outputStream.write(&accbuffer[0], sizeof(float) * 1024);
                                presetChangedCallback();

                            });
}

void setCallback(std::function<void()> cb) {
    presetChangedCallback = cb;
}


juce::String getPresetName() {
    if (!fileChooser.getResults().size())
        return "";

    return fileChooser.getResult().getFileNameWithoutExtension();
}

juce::String getPresetPath() {
    if (!fileChooser.getResults().size())
        return "";

    return fileChooser.getResult().getFullPathName();
}

void setFromPresetPath(juce::String path) {
    juce::File fp(path);

    if(fp.getSize() == (sizeof(float) * 1024)) {
        ready = false;
        auto result = fp;
        juce::FileInputStream inputStream (result);
        for(int i=0;i<1024;i++)
            accbuffer[i] = inputStream.readFloat();
        setupImpulseResponse();
        accsamples = 65; // XXX: set to the max to avoid calibrating
        presetChangedCallback();
}
}

private:
    juce::AudioBuffer<float> buffer;
    float fftbuffer[4096];
    float accbuffer[1024];
    float filterbuffer[1024];

    float accinpbuffer[4096];
    unsigned accsamples = 0;
    juce::dsp::FFT fft;
    unsigned cursor = 0;
    float avg = 0.0f;
    float peak = 0.0f;

    std::string m_path;
    juce::FileChooser fileChooser;
    bool ready = false;
    std::function<void()> presetChangedCallback = [] {};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LiveSpectrumAudioAnalyzer)
};
