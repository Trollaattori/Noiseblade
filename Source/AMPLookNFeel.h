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


-- This Visiual Theme of the Plugin. --

********************************************************************/

#ifndef AMPLOOKNFEEL_H
#define AMPLOOKNFEEL_H

#include <JuceHeader.h>

class AmpLookAndFeel  : public juce::LookAndFeel_V4 {
public:
    AmpLookAndFeel() {
        setColour (juce::ResizableWindow::backgroundColourId, juce::Colour(0x11,0x00,0x05));
        setColour (juce::TextButton::buttonColourId , juce::Colour(0x33,0x00,0x11));
        setColour (juce::TextButton::buttonOnColourId , juce::Colour(0xff,0x99,0xaa));
        setColour (juce::TextButton::textColourOffId , juce::Colour(0xff,0x77,0xaa));
        setColour (juce::TextButton::textColourOnId , juce::Colours::black);
        setColour (juce::Label::textColourId, juce::Colour(0xff,0x99,0xaa));

        setColour (juce::Slider::thumbColourId, juce::Colour(0xee,0x30,0x88));

        setColour (juce::Slider::rotarySliderFillColourId, juce::Colour(0x77,0x33,0x44));
        setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour(0x88,0x44,0x55));

        setColour (juce::GroupComponent::textColourId, juce::Colour(0xff,0x99,0xaa));
        setColour (juce::GroupComponent::outlineColourId, juce::Colour(0xff,0x99,0xaa));
    }

private:

//Copied From LookAndFeel_V4 - removed focus alpha and borders
virtual void drawButtonBackground (juce::Graphics& g,
                                           juce::Button& button,
                                           const juce::Colour& backgroundColour,
                                           bool shouldDrawButtonAsHighlighted,
                                           bool shouldDrawButtonAsDown) override
{
    auto cornerSize = 6.0f;
    auto bounds = button.getLocalBounds().toFloat().reduced (0.5f, 0.5f);

    auto baseColour = backgroundColour.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
        baseColour = baseColour.contrasting (shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour (baseColour);

    auto flatOnLeft   = button.isConnectedOnLeft();
    auto flatOnRight  = button.isConnectedOnRight();
    auto flatOnTop    = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom)
    {
        juce::Path path;
        path.addRoundedRectangle (bounds.getX(), bounds.getY(),
                                  bounds.getWidth(), bounds.getHeight(),
                                  cornerSize, cornerSize,
                                  ! (flatOnLeft  || flatOnTop),
                                  ! (flatOnRight || flatOnTop),
                                  ! (flatOnLeft  || flatOnBottom),
                                  ! (flatOnRight || flatOnBottom));

        g.fillPath (path);

        //g.setColour (button.findColour (juce::ComboBox::outlineColourId));
        //g.strokePath (path, juce::PathStrokeType (1.0f));
    }
    else
    {
        if(button.isEnabled()) {
            g.setColour (baseColour.withMultipliedAlpha(0.7));
            g.fillRoundedRectangle (bounds.reduced(3.0), cornerSize);
            g.setColour (baseColour.withMultipliedAlpha(0.4));
            g.fillRoundedRectangle (bounds.reduced(2.0), cornerSize);

            g.setColour (baseColour.withMultipliedAlpha(0.1));
            g.fillRoundedRectangle (bounds, cornerSize);
        }
        g.setColour (baseColour);
        g.fillRoundedRectangle (bounds.reduced(4.0), cornerSize);

        //g.setColour (button.findColour (juce::ComboBox::outlineColourId));
        //g.drawRoundedRectangle (bounds, cornerSize, 1.0f);
    }
}



};

#endif // AMPLOOKNFEEL_H
