/*
  ==============================================================================

    HighQualityMeter.cpp
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#include "HighQualityMeter.h"

HighQualityMeter::HighQualityMeter (const int numPorts)
    : m_iPortCount (numPorts),
      m_iPeakFalloff (1)
{
    m_ppValues = new HighQualityMeterValue*[m_iPortCount];

    for (int i = 0; i < m_iPortCount; i++)
    {
        m_ppValues[i] = new HighQualityMeterValue (this);
        addAndMakeVisible (m_ppValues[i]);
    }

    m_fScale = 100.0f;

    m_levels[0] = 0;
    m_levels[1] = 0;
    m_levels[2] = 0;
    m_levels[3] = 0;
    m_levels[4] = 0;

    m_colors[0] = Colours::red;
    m_colors[1] = Colours::red;
    m_colors[2] = Colours::orange;
    m_colors[3] = Colours::yellow;
    m_colors[4] = Colours::green;
    m_colors[5] = Colours::black;
    m_colors[6] = Colours::white;
}

HighQualityMeter::~HighQualityMeter()
{
    for (int i = 0; i < m_iPortCount; i++)
        delete m_ppValues[i];

    delete[] m_ppValues;
}

int HighQualityMeter::iec_scale (const float dB) const
{
    float fDef = 1.0;

    if (dB < -70.0)
        fDef = 0.0;
    else if (dB < -60.0)
        fDef = (dB + 70.0) * 0.0025;
    else if (dB < -50.0)
        fDef = (dB + 60.0) * 0.005 + 0.025;
    else if (dB < -40.0)
        fDef = (dB + 50.0) * 0.0075 + 0.075;
    else if (dB < -30.0)
        fDef = (dB + 40.0) * 0.015 + 0.15;
    else if (dB < -20.0)
        fDef = (dB + 30.0) * 0.02 + 0.3;
    else 
        fDef = (dB + 20.0) * 0.025 + 0.5;

    return (int) (fDef * m_fScale);
}

int HighQualityMeter::iec_level (const int index) const
{
    if (index >= 0 && index < LevelCount)
        return m_levels[index];
    return 0;
}

int HighQualityMeter::portCount () const
{
    return m_iPortCount;
}

void HighQualityMeter::setPeakFalloff (const int iPeakFalloff)
{
    m_iPeakFalloff = iPeakFalloff;
}

int HighQualityMeter::getPeakFalloff () const
{
    return m_iPeakFalloff;
}

void HighQualityMeter::peakReset ()
{
    for (int i = 0; i < m_iPortCount; i++)
        if (m_ppValues[i]) m_ppValues[i]->peakReset ();
}

void HighQualityMeter::refresh ()
{
    for (int i = 0; i < m_iPortCount; i++)
        if (m_ppValues[i]) m_ppValues[i]->refresh ();
}

void HighQualityMeter::setValue (const int iPort, const float fValue)
{
    if (iPort >= 0 && iPort < m_iPortCount && m_ppValues[iPort])
        m_ppValues[iPort]->setValue (fValue);
}

const Colour& HighQualityMeter::color (const int index) const
{
    if (index >= 0 && index < ColorCount)
        return m_colors[index];
    return m_colors[ColorBack];
}

void HighQualityMeter::paint (Graphics& g)
{
}

void HighQualityMeter::resized ()
{
    m_fScale = (float) getHeight();

    m_levels[0] = iec_scale (0.0);
    m_levels[1] = iec_scale (-3.0);
    m_levels[2] = iec_scale (-6.0);
    m_levels[3] = iec_scale (-10.0);
    m_levels[4] = iec_scale (-20.0);

    if (m_iPortCount > 0)
    {
        int iWidth = getWidth() / m_iPortCount;

        for (int i = 0; i < m_iPortCount; i++)
            if (m_ppValues[i]) m_ppValues[i]->setBounds (i * iWidth, 0, iWidth, getHeight());
    }
}

HighQualityMeterValue::HighQualityMeterValue (HighQualityMeter *pMeter)
    : m_pMeter (pMeter),
      m_fValue (-100.0f),
      m_iValueHold (0),
      m_fValueDecay (0.0f),
      m_iPeak (0),
      m_iPeakHold (0),
      m_fPeakDecay (0.0f),
      m_iPeakColor (0)
{
}

HighQualityMeterValue::~HighQualityMeterValue()
{
}

void HighQualityMeterValue::setValue (const float fValue)
{
    m_fValue = fValue;
}

void HighQualityMeterValue::refresh()
{
    if (m_pMeter == nullptr) return;

    int iValue = m_pMeter->iec_scale (m_fValue);

    if (iValue > m_iValueHold)
    {
        m_iValueHold = iValue;
        m_fValueDecay = 0.0f;
    }
    else
    {
        m_fValueDecay += 1.0f;
        m_iValueHold -= (int) (m_fValueDecay * 0.1f);
        if (m_iValueHold < 0) m_iValueHold = 0;
    }

    if (iValue > m_iPeakHold)
    {
        m_iPeakHold = iValue;
        m_fPeakDecay = 0.0f;
    }
    else if (m_pMeter->getPeakFalloff() > 0)
    {
        m_fPeakDecay += 0.05f;
        m_iPeakHold -= (int) (m_fPeakDecay);
        if (m_iPeakHold < 0) m_iPeakHold = 0;
    }

    repaint();
}

void HighQualityMeterValue::peakReset()
{
    m_iPeakHold = 0;
    repaint();
}

void HighQualityMeterValue::paint (Graphics& g)
{
    if (m_pMeter == nullptr) return;

    int h = getHeight();
    int w = getWidth();

    if (h <= 0 || w <= 0) return;

    g.fillAll (m_pMeter->color (HighQualityMeter::ColorBack));

    int iVal = m_iValueHold;
    if (iVal > h) iVal = h;
    
    for (int y = 0; y < iVal; ++y)
    {
        int colorIdx = HighQualityMeter::Color10dB;
        if (y >= m_pMeter->iec_level(0)) colorIdx = HighQualityMeter::ColorOver;
        else if (y >= m_pMeter->iec_level(1)) colorIdx = HighQualityMeter::Color0dB;
        else if (y >= m_pMeter->iec_level(2)) colorIdx = HighQualityMeter::Color3dB;
        else if (y >= m_pMeter->iec_level(3)) colorIdx = HighQualityMeter::Color6dB;

        g.setColour (m_pMeter->color (colorIdx));
        g.drawHorizontalLine (h - y - 1, 1.0f, (float) w - 1.0f);
    }

    if (m_iPeakHold > 0)
    {
        int yPeak = m_iPeakHold;
        if (yPeak >= h) yPeak = h - 1;

        g.setColour (m_pMeter->color (HighQualityMeter::ColorFore));
        g.drawHorizontalLine (h - yPeak - 1, 1.0f, (float) w - 1.0f);
    }
}

void HighQualityMeterValue::resized ()
{
}
