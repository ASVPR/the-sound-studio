/*
  ==============================================================================

    PluginWindow.cpp
    Created: 27 Aug 2015 1:00:39pm
    Author:  Gary Jones

  ==============================================================================
*/

#include "PluginWindow.h"

class PluginWindow;
static Array <PluginWindow*> activePluginWindows;

PluginWindow::PluginWindow (Component* const uiComp,
                            AudioProcessor * owner_,
                            const bool isGeneric_, int ref)
: DocumentWindow (uiComp->getName(), Colours::lightblue,
                  DocumentWindow::minimiseButton | DocumentWindow::closeButton),
owner (owner_),
isGeneric (isGeneric_)
{
    slotRef = ref;
    
    setSize (400, 300);
    
    setContentOwned (uiComp, true);
    
 /*   setTopLeftPosition (owner->properties.getWithDefault ("uiLastX", Random::getSystemRandom().nextInt (500)),
                        owner->properties.getWithDefault ("uiLastY", Random::getSystemRandom().nextInt (500)));
  */
    setVisible (true);
    
    activePluginWindows.add (this);
}

void PluginWindow::closeCurrentlyOpenWindowsFor (int ref)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
    {
        if (activePluginWindows.getUnchecked(i)->slotRef == ref)
        {
            delete activePluginWindows.getUnchecked(i);
        }
    }
}

void PluginWindow::closeAllCurrentlyOpenWindows()
{
    if (activePluginWindows.size() > 0)
    {
        for (int i = activePluginWindows.size(); --i >= 0;)
            delete activePluginWindows.getUnchecked(i);
        
//        Component dummyModalComp;
//        dummyModalComp.enterModalState();
//        MessageManager::getInstance()->runDispatchLoopUntil (50);
    }
}

/*
PluginWindow* PluginWindow::getWindowFor (AudioProcessorGraph::Node* node,
                                          bool useGenericView)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node
            && activePluginWindows.getUnchecked(i)->isGeneric == useGenericView)
            return activePluginWindows.getUnchecked(i);
    
    AudioProcessorEditor* ui = nullptr;
    
    if (! useGenericView)
    {
        ui = node->getProcessor()->createEditorIfNeeded();
        
        if (ui == nullptr)
            useGenericView = true;
    }
    
    if (useGenericView)
        ui = new GenericAudioProcessorEditor (node->getProcessor());
    
    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast <AudioPluginInstance*> (node->getProcessor()))
            ui->setName (plugin->getName());
        
        return new PluginWindow (ui, node, useGenericView);
    }
    
    return nullptr;
}
 */

PluginWindow* PluginWindow::openWindowFor (AudioProcessor * node,
                                          bool useGenericView, int ref)
{
    for (int i = activePluginWindows.size(); --i >= 0;)
        if (activePluginWindows.getUnchecked(i)->owner == node
            && activePluginWindows.getUnchecked(i)->isGeneric == useGenericView)
            return activePluginWindows.getUnchecked(i);
    
    AudioProcessorEditor* ui = nullptr;
    
    if (! useGenericView)
    {
        ui = node->createEditorIfNeeded();
        
        if (ui == nullptr)
            useGenericView = true;
    }
    
    if (useGenericView)
        ui = new GenericAudioProcessorEditor (node);
    
    if (ui != nullptr)
    {
        if (AudioPluginInstance* const plugin = dynamic_cast <AudioPluginInstance*> (node))
            ui->setName (plugin->getName());
        
        return new PluginWindow (ui, node, useGenericView, ref);
    }
    
    return nullptr;
}

PluginWindow::~PluginWindow()
{
    activePluginWindows.removeFirstMatchingValue (this);
    clearContentComponent();
}

void PluginWindow::moved()
{
   // owner->properties.set ("uiLastX", getX());
   // owner->properties.set ("uiLastY", getY());
}

void PluginWindow::closeButtonPressed()
{
    delete this;
}
