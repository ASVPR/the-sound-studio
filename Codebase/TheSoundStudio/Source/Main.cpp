/*
  ==============================================================================

    Main.cpp
    Author: Ziv Elovitch

    The Sound Studio - Main application entry point.
    
    This file contains the JUCE application startup code and main window setup.
    Handles window initialization, sizing, and visibility management to ensure
    proper application launch and user interface display.

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "MainComponent.h"

//==============================================================================
class ASVPRApplication  : public JUCEApplication
{
public:
    //==============================================================================
    ASVPRApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..

        mainWindow = std::make_unique<MainWindow>(getApplicationName());
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..

        mainWindow.reset(); // (deletes our window)
    }

    //==============================================================================
    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted (const String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    //==============================================================================
    /*
        This class implements the desktop window that contains an instance of
        our MainComponent class.
    */
    class MainWindow    : public DocumentWindow
    {
    public:
        MainWindow (String name)  : DocumentWindow (name,
                                                    Desktop::getInstance().getDefaultLookAndFeel()
                                                                          .findColour (ResizableWindow::backgroundColourId),
                                                    DocumentWindow::closeButton + DocumentWindow::minimiseButton + DocumentWindow::maximiseButton)
        {
            // Use native title bar for better macOS integration
            setUsingNativeTitleBar (true);
            
           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            // Set a default window size first
            setResizable (true, true);
            centreWithSize (1000, 800); // Set reasonable default size
           #endif

            // Create main component AFTER window sizing is set
            auto* mainComp = new MainComponent();
            setContentOwned (mainComp, true);
            
            // Adjust window size based on MainComponent's actual scaled dimensions
            int windowWidth = mainComp->mainWidth * mainComp->scaleFactor;
            int windowHeight = mainComp->mainHeight * mainComp->scaleFactor;
            
           #if !JUCE_IOS && !JUCE_ANDROID
            // Update to proper size now that we have the component
            centreWithSize (windowWidth, windowHeight);
           #endif
            
            // Attach OpenGL context AFTER content is set
            openGLContext.attachTo(*getTopLevelComponent());

            // Ensure window appears on screen and is brought to front
            setVisible (true);
            toFront (true);
            setAlwaysOnTop (false);
            
            // Make sure window is fully displayed
            repaint();
        }
        
        
        

        void closeButtonPressed() override
        {
            // This is called when the user tries to close this window. Here, we'll just
            // ask the app to quit when this happens, but you can change this to do
            // whatever you need.
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

        /* Note: Be careful if you override any DocumentWindow methods - the base
           class uses a lot of them, so by overriding you might break its functionality.
           It's best to do all your work in your content component instead, but if
           you really have to override any DocumentWindow methods, make sure your
           subclass also calls the superclass's method.
        */

    private:
        OpenGLContext openGLContext;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
    
    
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (ASVPRApplication)
