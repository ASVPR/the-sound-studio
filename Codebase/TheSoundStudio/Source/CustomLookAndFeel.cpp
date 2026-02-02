/*
  ==============================================================================

    CustomLookAndFeel.cpp

    Part of: The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.

  ==============================================================================
*/

#include "CustomLookAndFeel.h"
#include "utility_components/ComboBox.h"
#include "TSSConstants.h"

CustomLookAndFeel::CustomLookAndFeel()
{
//    setColour(TextEditor::textColourId, Colours::darkgrey);
    setColour(Slider::textBoxTextColourId, Colours::lightgrey);
    setColour(Slider::textBoxBackgroundColourId, juce::Colour(TSS::UI::Colors::kSliderBackground));
    setColour(PopupMenu::textColourId, Colours::darkgrey);
    setColour(PopupMenu::highlightedBackgroundColourId, Colours::darkgrey);
    setColour(ComboBox::textColourId, Colours::darkgrey);
    setColour(ComboBox::arrowColourId, Colours::darkgrey);
    
    setColour(juce::Label::textWhenEditingColourId, Colours::darkgrey);
    
    Typeface::Ptr AssistantLight   = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    fontLight = std::make_unique<Font>(AssistantLight);
    
}

Label* CustomLookAndFeel::createSliderTextBox (Slider& slider)
{

    auto* l = LookAndFeel_V2::createSliderTextBox (slider);

    if (getCurrentColourScheme() == LookAndFeel_V4::getGreyColourScheme() && (slider.getSliderStyle() == Slider::LinearBar
                                                                              || slider.getSliderStyle() == Slider::LinearBarVertical))
    {
        l->setColour (Label::textColourId, Colours::black.withAlpha (0.7f));
    }

    l->setFont(28 * scaleFactor);

    return l;
}

void CustomLookAndFeel::drawButtonBackground (Graphics&g, Button&button, const Colour& backgroundColour,
                                   bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    g.setColour(backgroundColour);
    g.fillAll();
}

void CustomLookAndFeel::drawToggleButton (Graphics&g, ToggleButton&button,
                               bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto fontSize = 38; //jmin (15.0f, button.getHeight() * 1.f);
    auto tickWidth = fontSize * 1.1f * scaleFactor;

    drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 shouldDrawButtonAsHighlighted,
                 shouldDrawButtonAsDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    
//    Typeface::Ptr AssistantLight   = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
//    Font fontLight(AssistantLight);
    
    fontLight->setHeight(38 * scaleFactor);
    
    g.setFont (*fontLight);

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    g.drawFittedText (button.getButtonText(),
                      button.getLocalBounds().withTrimmedLeft (roundToInt (tickWidth) + 10)
                                             .withTrimmedRight (2),
                      Justification::centredLeft, 10);
}

void CustomLookAndFeel::drawTickBox (Graphics& g, Component& component,
                                  float x, float y, float w, float h,
                                  const bool ticked,
                                  const bool isEnabled,
                                  const bool shouldDrawButtonAsHighlighted,
                                  const bool shouldDrawButtonAsDown)
{
    ignoreUnused (isEnabled, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
    
    Rectangle<float> tickBounds (x, y, w, h);
    
    g.setColour (component.findColour (ToggleButton::tickDisabledColourId));
    g.drawRoundedRectangle (tickBounds, 4.0f, 1.0f);
    
    if (ticked)
    {
        g.setColour (component.findColour (ToggleButton::tickColourId));
        auto tick = getTickShape (0.75f);
        g.fillPath (tick, tick.getTransformToScaleToFit (tickBounds.reduced (4, 5).toFloat(), false));
    }
}

void CustomLookAndFeel::drawButtonText (Graphics& g, TextButton& button, bool /*shouldDrawButtonAsHighlighted*/, bool /*shouldDrawButtonAsDown*/)
{
    Font font (getTextButtonFont (button, button.getHeight() * 0.75f));
    g.setFont (font);
    g.setColour (button.findColour (button.getToggleState() ? TextButton::textColourOnId
                                    : TextButton::textColourOffId)
                 .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
    
    const int yIndent = jmin (4, button.proportionOfHeight (0.3f));
    const int cornerSize = jmin (button.getHeight(), button.getWidth()) / 2;
    
    const int fontHeight = roundToInt (font.getHeight() * 0.6f) * scaleFactor;
    const int leftIndent  = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnLeft() ? 4 : 2));
    const int rightIndent = jmin (fontHeight, 2 + cornerSize / (button.isConnectedOnRight() ? 4 : 2));
    const int textWidth = button.getWidth() - leftIndent - rightIndent;
    
    if (textWidth > 0)
        g.drawFittedText (button.getButtonText(),
                          leftIndent, yIndent, textWidth, button.getHeight() - yIndent * 2,
                          Justification::centred, 2);
}

Font CustomLookAndFeel::getTextButtonFont (TextButton&button, int buttonHeight)
{
    // fonts
//    Typeface::Ptr AssistantLight   = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
//    Font fontLight(AssistantLight);
//
    fontLight->setHeight(38 * scaleFactor);
//        fontLight.setUnderline(true);
    
    return *fontLight;
}


// Progress Bar
//==============================================================================
void CustomLookAndFeel::drawProgressBar (Graphics&, ProgressBar&, int width, int height, double progress, const String& textToShow)
{
    
}
bool CustomLookAndFeel::isProgressBarOpaque (ProgressBar&)     { return false; }

// ComboBox
//==============================================================================
void CustomLookAndFeel::drawComboBox (Graphics&g, int width, int height, bool isButtonDown,
                   int buttonX, int buttonY, int buttonW, int buttonH,
                   ComboBox&box)
{
    int cornerSize = 4;
    auto y = 0;
    Rectangle<int> boxBounds (0, y, width, height);

    if (auto c = dynamic_cast<asvpr::ComboBox*>(&box))
    {
        if(c->isShowingLabel())
        {
            g.setColour(juce::Colours::white);

            g.setFont(13);

            g.drawFittedText(c->getName(),
                             boxBounds.removeFromTop(20), juce::Justification::centredLeft, 1);
            y = 20;
            height = height - 20;
        }
    }

    Colour c(TSS::UI::Colors::kComboBoxBackground);

    g.setColour (c);
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    g.setColour (box.findColour (ComboBox::outlineColourId));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

    Rectangle<int> arrowZone (width - 20, y, 15, height);
    Path path;
    path.startNewSubPath (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo (static_cast<float> (arrowZone.getCentreX()), arrowZone.getCentreY() + 3.0f);
    path.lineTo (arrowZone.getRight() - 3.0f, arrowZone.getCentreY() - 2.0f);
    path.lineTo (arrowZone.getX() + 3.0f, arrowZone.getCentreY() - 2.0f);

    g.setColour ({51, 51, 51});

    g.fillPath (path);

}




Label* CustomLookAndFeel::createComboBoxTextBox (ComboBox&box)
{
    return new Label (String(), String());
}

void CustomLookAndFeel::positionComboBoxText (ComboBox&box, Label& label)
{

    auto bounds = juce::Rectangle<int>(5, 1,
                                       box.getWidth() - 30,
                                       box.getHeight() - 2);

    if (auto c = dynamic_cast<asvpr::ComboBox*>(&box))
    {
        if(c->isShowingLabel())
        {
            bounds.translate(0, 20);
            bounds.setHeight(bounds.getHeight() - 20);
        }
    }

    label.setBounds (bounds);
    
    label.setJustificationType(Justification::centredLeft);
    
    label.setColour(Label::ColourIds::textColourId, {51, 51, 51});

    label.setFont (getComboBoxFont (box));
}

Font CustomLookAndFeel::getComboBoxFont (ComboBox&c)
{
    fontLight->setHeight(38 * scaleFactor);
    
    return *fontLight;
}

Font CustomLookAndFeel::getPopupMenuFont()
{
    // fonts
//    Typeface::Ptr AssistantLight   = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
//    Font fontLight(AssistantLight);
//
    fontLight->setHeight(38 * scaleFactor);
    
    return *fontLight;
}

/** Fills the background of a popup menu component. */
void CustomLookAndFeel::drawPopupMenuBackground (Graphics&g, int width, int height)
{
    g.fillAll (Colour(TSS::UI::Colors::kPopupMenuBackground));
    ignoreUnused (width, height);
}

void CustomLookAndFeel::drawPopupMenuItem (Graphics&g, const Rectangle<int>& area,
                                bool isSeparator, bool isActive, bool isHighlighted,
                                bool isTicked, bool hasSubMenu,
                                const String& text,
                                const String& shortcutKeyText,
                                const Drawable* icon,
                                const Colour* textColourToUse)
{
    if (isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (roundToInt ((r.getHeight() * 0.5f) - 0.5f));

        g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.3f));
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour (PopupMenu::textColourId)
                                                      : *textColourToUse);

        auto r  = area.reduced (1);

        if (isHighlighted && isActive)
        {
            g.setColour (findColour (PopupMenu::highlightedBackgroundColourId));
            g.fillRect (r);

            g.setColour (findColour (PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }

        r.reduce (jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = r.getHeight();

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (roundToInt (maxFontHeight)).toFloat();

        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft (roundToInt (maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (text, r, Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}



// Rotary Slider
//==============================================================================
    void CustomLookAndFeel::drawRotarySlider (Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle,
                           float rotaryEndAngle, Slider& slider)
{
    auto outline = juce::Colour(TSS::UI::Colors::kSliderOutline);
    auto fill    = juce::Colour(TSS::UI::Colors::kSliderFill);

    auto bounds = Rectangle<int> (x, y, width, height).toFloat().reduced (10);

    auto radius = jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto lineW = 6;//jmin (8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;

    Path backgroundArc;
    backgroundArc.addCentredArc (bounds.getCentreX(),
                                 bounds.getCentreY(),
                                 arcRadius,
                                 arcRadius,
                                 0.0f,
                                 rotaryStartAngle,
                                 rotaryEndAngle,
                                 true);

    g.setColour (outline);
    g.strokePath (backgroundArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));

    if (slider.isEnabled())
    {
        Path valueArc;
        valueArc.addCentredArc (bounds.getCentreX(),
                                bounds.getCentreY(),
                                arcRadius,
                                arcRadius,
                                0.0f,
                                rotaryStartAngle,
                                toAngle,
                                true);

        g.setColour (fill);
        g.strokePath (valueArc, PathStrokeType (lineW, PathStrokeType::curved, PathStrokeType::rounded));
    }

    auto thumbWidth = lineW * 1.8f;
    Point<float> thumbPoint (bounds.getCentreX() + arcRadius * std::cos (toAngle - MathConstants<float>::halfPi),
                             bounds.getCentreY() + arcRadius * std::sin (toAngle - MathConstants<float>::halfPi));

    g.setColour (juce::Colour(TSS::UI::Colors::kSliderFill));
    g.fillEllipse (Rectangle<float> (thumbWidth, thumbWidth).withCentre (thumbPoint));
}

// Tooltip
//==============================================================================
void CustomLookAndFeel::drawTooltip (Graphics&g, const String& text, int width, int height)
{
    Rectangle<int> bounds (width, height);
    auto cornerSize = 5.0f;
    
    g.setColour (findColour (TooltipWindow::backgroundColourId));
    g.fillRoundedRectangle (bounds.toFloat(), cornerSize);
    
    g.setColour (findColour (TooltipWindow::outlineColourId));
    g.drawRoundedRectangle (bounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);
    
    const float tooltipFontSize = 13.0f;
    const int maxToolTipWidth = 400;
    
    AttributedString s;
    s.setJustification (Justification::centred);
    s.append (text, Font (tooltipFontSize, Font::bold), findColour (TooltipWindow::textColourId));
    
    TextLayout tl;
    tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
    
    tl.draw (g, { static_cast<float> (width), static_cast<float> (height) });
    
    // hacked it back into life !
    
//        LookAndFeelHelpers::layoutTooltipText (text, findColour (TooltipWindow::textColourId))
//        .draw (g, { static_cast<float> (width), static_cast<float> (height) });
    
}




void CustomLookAndFeel::fillTextEditorBackground (Graphics& g, int width, int height, TextEditor& editor)

{
    g.setColour(juce::Colour(TSS::UI::Colors::kTextEditorBackground));
    g.fillRoundedRectangle(editor.getLocalBounds().toFloat(), 5);
}
void CustomLookAndFeel::drawTextEditorOutline (Graphics&, int width, int height, TextEditor&)
{
    
}
