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
class TSSApplication  : public JUCEApplication
{
public:
    //==============================================================================
    TSSApplication() {}

    const String getApplicationName() override       { return ProjectInfo::projectName; }
    const String getApplicationVersion() override    { return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override       { return true; }

    //==============================================================================
    void initialise (const String& commandLine) override
    {
        // Setup error logging to file ("1_Add Error Log")
        initialiseErrorLogging();

        Logger::writeToLog("TSSApplication::initialise - starting main window");
        try
        {
            mainWindow = std::make_unique<MainWindow>(getApplicationName());
            Logger::writeToLog("TSSApplication::initialise - main window created");
        }
        catch (const std::exception& e)
        {
            Logger::writeToLog(String("TSSApplication::initialise - exception creating MainWindow: ") + e.what());
            throw;
        }
    }

    void shutdown() override
    {
        Logger::writeToLog("TSSApplication::shutdown - destroying main window");
        mainWindow.reset(); // (deletes our window)

        Logger::writeToLog("TSSApplication::shutdown - clearing logger");
        Logger::setCurrentLogger(nullptr);
        fileLogger.reset();
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
            Logger::writeToLog("MainWindow::ctor - begin");
            // Use native title bar for better macOS integration
            setUsingNativeTitleBar (true);
            
           #if JUCE_IOS || JUCE_ANDROID
            setFullScreen (true);
           #else
            // Set window to be resizable with constraints
            setResizable (true, true);
            setResizeLimits (800, 600, 3840, 2160); // Min: 800x600, Max: 4K
            
            // Set initial size based on screen size
            auto& desktop = Desktop::getInstance();
            auto screenBounds = desktop.getDisplays().getPrimaryDisplay()->userArea;
            int initialWidth = jmin(1920, (int)(screenBounds.getWidth() * 0.8f));
            int initialHeight = jmin(1080, (int)(screenBounds.getHeight() * 0.8f));
            centreWithSize (initialWidth, initialHeight);
           #endif

            // Create main component AFTER window sizing is set
            Logger::writeToLog("MainWindow::ctor - creating MainComponent");
            auto* mainComp = new MainComponent();
            Logger::writeToLog("MainWindow::ctor - MainComponent created");
            setContentOwned (mainComp, true);
            Logger::writeToLog("MainWindow::ctor - content set");
            
            // Adjust window size based on MainComponent's actual scaled dimensions
            int windowWidth = mainComp->mainWidth * mainComp->scaleFactor;
            int windowHeight = mainComp->mainHeight * mainComp->scaleFactor;
            
           #if !JUCE_IOS && !JUCE_ANDROID
            // Update to proper size now that we have the component
            centreWithSize (windowWidth, windowHeight);
            Logger::writeToLog("MainWindow::ctor - window sized");
           #endif
            
            // Attach OpenGL context AFTER content is set
            openGLContext.attachTo(*getTopLevelComponent());
            Logger::writeToLog("MainWindow::ctor - OpenGL attached");

            // Ensure window appears on screen and is brought to front
            setVisible (true);
            toFront (true);
            setAlwaysOnTop (false);
            
            // Make sure window is fully displayed
            repaint();
            Logger::writeToLog("MainWindow::ctor - end");
        }
        
        
        

        void closeButtonPressed() override
        {
            Logger::writeToLog("MainWindow::closeButtonPressed - systemRequestedQuit");
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
    std::unique_ptr<FileLogger> fileLogger;
    
    void initialiseErrorLogging()
    {
        // Create logs directory
        const File logsDir("/Users/zivelovitch/Documents/TSS/Logs");
        if (! logsDir.exists())
            logsDir.createDirectory();

        // Create a date-stamped error log file in the specified directory
        // Example: /Users/zivelovitch/Documents/TSS/Logs/TSS_2025-09-10_15-14-25_Error.txt
        const auto dateStamp = Time::getCurrentTime().formatted("%Y-%m-%d_%H-%M-%S");
        const File logFile = logsDir.getChildFile("TSS_" + dateStamp + "_Error.txt");
        fileLogger = std::make_unique<FileLogger>(logFile, String());
        Logger::setCurrentLogger(fileLogger.get());
    }
    
    std::unique_ptr<MainWindow> mainWindow;
    
    
};

//==============================================================================
// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION (TSSApplication)
