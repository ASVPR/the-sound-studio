/*
  ==============================================================================

    PluginWindow.h
    The Sound Studio
    Copyright (c) 2026 Ziv Elovitch. All rights reserved.
    all right reserves... - Ziv Elovitch

    Licensed under the MIT License. See LICENSE file for details.

  ==============================================================================
*/

#pragma once
#ifndef PLUGINWINDOW_H_INCLUDED
#define PLUGINWINDOW_H_INCLUDED


#include <iostream>
#include "JuceHeader.h"

/** A desktop window containing a plugin's UI. */
class PluginWindow  : public DocumentWindow
{
    PluginWindow (Component* const uiComp,
                  AudioProcessor * owner_,
                  const bool isGeneric_,
                  int ref);
    
public:
  //  static PluginWindow* getWindowFor (AudioProcessorGraph::Node* node,
  //                                     bool useGenericView);
    
    static PluginWindow* openWindowFor (AudioProcessor * node,
                                       bool useGenericView, int ref);
    
    static void closeCurrentlyOpenWindowsFor (int ref);
    
    static void closeAllCurrentlyOpenWindows();
    
    ~PluginWindow();
    
    void moved();
    
    void closeButtonPressed();
    
    int slotRef;
private:
    AudioProcessor * owner;
    bool isGeneric;
    
//    const AudioProcessorGraph::Node::Ptr node;
};

/** A desktop window containing a plugin's UI. */
//class PluginWindow3  : public DocumentWindow
//{
//    PluginWindow3 (Component* const uiComp,
//                  AudioProcessor * owner_,
//                  const bool isGeneric_)
//    {
//        
//    }
//    
//public:
//  //  static PluginWindow* getWindowFor (AudioProcessorGraph::Node* node,
//  //                                     bool useGenericView);
//    
//    static PluginWindow3* openWindowFor (AudioProcessor * node,
//                                       bool useGenericView);
//    
//    static void closeCurrentlyOpenWindowsFor (const uint32 nodeId);
//    
//    static void closeAllCurrentlyOpenWindows();
//    
//    ~PluginWindow3()
//    {
//        activePluginWindows.removeFirstMatchingValue (this);
//        clearContentComponent();
//    }
//    
//    void moved()
//    {
//       // owner->properties.set ("uiLastX", getX());
//       // owner->properties.set ("uiLastY", getY());
//    }
//    
//    void closeButtonPressed()
//    {
//        delete this;
//    }
//    
//private:
//    AudioProcessor * owner;
//    bool isGeneric;
//    
////    const AudioProcessorGraph::Node::Ptr node;
//};






class PluginWindow2  : public DocumentWindow
{
public:
    enum class Type
    {
        normal = 0,
        generic,
        programs,
        audioIO,
        debug,
        numTypes
    };

    PluginWindow2 (AudioProcessorGraph::Node* n, Type t, OwnedArray<PluginWindow2>& windowList)
       : DocumentWindow (n->getProcessor()->getName(),
                         LookAndFeel::getDefaultLookAndFeel().findColour (ResizableWindow::backgroundColourId),
                         DocumentWindow::minimiseButton | DocumentWindow::closeButton),
         activeWindowList (windowList),
         node (n), type (t)
    {
        setSize (400, 300);

        if (auto* ui = createProcessorEditor (*node->getProcessor(), type))
            setContentOwned (ui, true);

       #if JUCE_IOS || JUCE_ANDROID
        auto screenBounds = Desktop::getInstance().getDisplays().getTotalBounds (true).toFloat();
        auto scaleFactor = jmin ((screenBounds.getWidth() - 50) / getWidth(), (screenBounds.getHeight() - 50) / getHeight());

        if (scaleFactor < 1.0f)
            setSize ((int) (getWidth() * scaleFactor), (int) (getHeight() * scaleFactor));

        setTopLeftPosition (20, 20);
       #else
        setTopLeftPosition (node->properties.getWithDefault (getLastXProp (type), Random::getSystemRandom().nextInt (500)),
                            node->properties.getWithDefault (getLastYProp (type), Random::getSystemRandom().nextInt (500)));
       #endif

        node->properties.set (getOpenProp (type), true);

        setVisible (true);
    }

    ~PluginWindow2() override
    {
        clearContentComponent();
    }

    void moved() override
    {
        node->properties.set (getLastXProp (type), getX());
        node->properties.set (getLastYProp (type), getY());
    }

    void closeButtonPressed() override
    {
        node->properties.set (getOpenProp (type), false);
        activeWindowList.removeObject (this);
    }

    static String getLastXProp (Type type)    { return "uiLastX_" + getTypeName (type); }
    static String getLastYProp (Type type)    { return "uiLastY_" + getTypeName (type); }
    static String getOpenProp  (Type type)    { return "uiopen_"  + getTypeName (type); }

    OwnedArray<PluginWindow2>& activeWindowList;
    const AudioProcessorGraph::Node::Ptr node;
    const Type type;

private:
    float getDesktopScaleFactor() const override     { return 1.0f; }

    static AudioProcessorEditor* createProcessorEditor (AudioProcessor& processor,
                                                        PluginWindow2::Type type)
    {
        if (type == PluginWindow2::Type::normal)
        {
            if (auto* ui = processor.createEditorIfNeeded())
                return ui;

            type = PluginWindow2::Type::generic;
        }

        if (type == PluginWindow2::Type::generic)  return new GenericAudioProcessorEditor (processor);
        if (type == PluginWindow2::Type::programs) return new ProgramAudioProcessorEditor (processor);
//        if (type == PluginWindow2::Type::audioIO)  return new IOConfigurationWindow (processor);
//        if (type == PluginWindow2::Type::debug)    return new PluginDebugWindow (processor);

        jassertfalse;
        return {};
    }

    static String getTypeName (Type type)
    {
        switch (type)
        {
            case Type::normal:     return "Normal";
            case Type::generic:    return "Generic";
            case Type::programs:   return "Programs";
            case Type::audioIO:    return "IO";
            case Type::debug:      return "Debug";
            default:               return {};
        }
    }

    //==============================================================================
    struct ProgramAudioProcessorEditor  : public AudioProcessorEditor
    {
        ProgramAudioProcessorEditor (AudioProcessor& p)  : AudioProcessorEditor (p)
        {
            setOpaque (true);

            addAndMakeVisible (panel);

            Array<PropertyComponent*> programs;

            auto numPrograms = p.getNumPrograms();
            int totalHeight = 0;

            for (int i = 0; i < numPrograms; ++i)
            {
                auto name = p.getProgramName (i).trim();

                if (name.isEmpty())
                    name = "Unnamed";

                auto pc = new PropertyComp (name, p);
                programs.add (pc);
                totalHeight += pc->getPreferredHeight();
            }

            panel.addProperties (programs);

            setSize (400, jlimit (25, 400, totalHeight));
        }

        void paint (Graphics& g) override
        {
            g.fillAll (Colours::grey);
        }

        void resized() override
        {
            panel.setBounds (getLocalBounds());
        }

    private:
        struct PropertyComp  : public PropertyComponent,
                               private AudioProcessorListener
        {
            PropertyComp (const String& name, AudioProcessor& p)  : PropertyComponent (name), owner (p)
            {
                owner.addListener (this);
            }

            ~PropertyComp() override
            {
                owner.removeListener (this);
            }

            void refresh() override {}
            void audioProcessorChanged (AudioProcessor*, const ChangeDetails& details) override {}
            void audioProcessorParameterChanged (AudioProcessor*, int, float) override {}

            AudioProcessor& owner;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PropertyComp)
        };

        PropertyPanel panel;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ProgramAudioProcessorEditor)
    };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow2)
};



#endif  // PLUGINWINDOW_H_INCLUDED
