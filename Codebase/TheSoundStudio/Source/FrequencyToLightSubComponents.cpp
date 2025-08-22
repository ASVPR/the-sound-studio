/*
  ==============================================================================

    FrequencyToLightSubComponents.cpp
    Created: 17 Oct 2019 11:42:22am
    Author:  Gary Jones

  ==============================================================================
*/

#include "FrequencyToLightSubComponents.h"

#pragma mark ColourOutputComponent

ColourOutputComponent::ColourOutputComponent()
{
    isGradient          = false;
    backgroundColour    = Colours::green;
}

ColourOutputComponent::~ColourOutputComponent(){}

void ColourOutputComponent::paint (Graphics&g)
{
    if (isGradient)
    {
        ColourGradient c(chordGradient);
        g.setGradientFill(c);
//        g.fillAll();
        g.fillRect(0, 0, getWidth(), getHeight());
    }
    else
    {
        g.fillAll(backgroundColour);
    }
    
}

void ColourOutputComponent::setBackgroundColour(Colour newColour)
{
    backgroundColour = newColour;
    repaint();
}

#pragma mark ShortcutColourComponent

ShortcutColourComponent::ShortcutColourComponent()
{
    // Font
    Typeface::Ptr AssistantLight     = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold   = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    imageAddIcon            = ImageCache::getFromMemory(BinaryData::ShortcutAdd2x_png, BinaryData::ShortcutAdd2x_pngSize);
    
    backgroundImage         = ImageCache::getFromMemory(BinaryData::ShortcutBackground2x_png, BinaryData::ShortcutBackground2x_pngSize);
    
    imageSettings           = ImageCache::getFromMemory(BinaryData::settings2x_png, BinaryData::settings2x_pngSize);
    
    imageDelete             = ImageCache::getFromMemory(BinaryData::CloseButton2x_png, BinaryData::CloseButton2x_pngSize);
    
    int shortcutWidth       = 180;
    int shortcutHeight      = 140;
    
    int insetButton = 33;
    
    button_Add = new ImageButton();
    button_Add->setTriggeredOnMouseDown(true);
    button_Add->setImages (false, true, true,
                           imageAddIcon, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageAddIcon, 0.75, Colour (0x00000000));
    button_Add->setBounds(insetButton, insetButton, shortcutWidth - (insetButton*2), shortcutHeight - (insetButton*2));
    addAndMakeVisible(button_Add);
    
    int colourInset = 5;
    colourOutputComponent = new ColourOutputComponent();
    colourOutputComponent->setBounds(colourInset,colourInset, shortcutWidth - (colourInset*2), shortcutHeight-(colourInset*2));
    colourOutputComponent->setBackgroundColour(Colours::darkgrey);
    addAndMakeVisible(colourOutputComponent);
    
    label_ColourCode = new Label("","some colour code");
    addAndMakeVisible(label_ColourCode);
    
    label_ColourCode->setBounds(0,0,shortcutWidth, shortcutHeight);
    label_ColourCode->setJustificationType(Justification::centred);
    
    int baseY = 110;
    button_OpenSettings = new ImageButton();
    button_OpenSettings->setTriggeredOnMouseDown(true);
    button_OpenSettings->setImages (false, true, true,
                           imageSettings, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageSettings, 0.75, Colour (0x00000000));
    button_OpenSettings->setBounds(10, baseY, 20, 20);
    addAndMakeVisible(button_OpenSettings);
    
    
    button_Delete = new ImageButton();
    button_Delete->setTriggeredOnMouseDown(true);
    button_Delete->setImages (false, true, true,
                           imageDelete, 0.999f, Colour (0x00000000),
                           Image(), 1.000f, Colour (0x00000000),
                           imageDelete, 0.75, Colour (0x00000000));
    button_Delete->setBounds(150, 10, 20, 20);
    addAndMakeVisible(button_Delete);
    
    
    setState(false);

}

ShortcutColourComponent::~ShortcutColourComponent(){ }

void ShortcutColourComponent::resized()
{
    int shortcutWidth       = 180;
    int shortcutHeight      = 140;
    
    int insetButton = 33;
    
    button_Add->setBounds(insetButton * scaleFactor, insetButton * scaleFactor, (shortcutWidth - (insetButton*2))  * scaleFactor, (shortcutHeight - (insetButton*2)) * scaleFactor);
    
    int colourInset = 5;
    
    colourOutputComponent->setBounds(colourInset * scaleFactor,colourInset * scaleFactor, (shortcutWidth - (colourInset*2)) * scaleFactor, (shortcutHeight-(colourInset*2)) * scaleFactor);
    
    label_ColourCode->setBounds(0,0,shortcutWidth * scaleFactor, shortcutHeight * scaleFactor);
    label_ColourCode->setBounds(0,0,shortcutWidth * scaleFactor, shortcutHeight * scaleFactor);
    
    int baseY = 110 * scaleFactor;
    button_OpenSettings->setBounds(10 * scaleFactor, baseY, 20 * scaleFactor, 20 * scaleFactor);
    button_Delete->setBounds(150 * scaleFactor, 10 * scaleFactor, 20 * scaleFactor, 20 * scaleFactor);
    
    
}

#pragma mark  ManipulationPopupComponent

ManipulationPopupComponent::ManipulationPopupComponent(){
    
    shortcutRef = 0;
    
    Typeface::Ptr AssistantLight        = Typeface::createSystemTypefaceFor(BinaryData::AssistantLight_ttf, BinaryData::AssistantLight_ttfSize);
    Typeface::Ptr AssistantSemiBold     = Typeface::createSystemTypefaceFor(BinaryData::AssistantSemiBold_ttf, BinaryData::AssistantSemiBold_ttfSize);
    Font fontSemiBold(AssistantSemiBold);
    Font fontLight(AssistantLight);
    
    fontSemiBold.setHeight(33);
    fontLight.setHeight(33);
    
    imageMainBackground             = ImageCache::getFromMemory(BinaryData::FrequencyToLightPopupBackground_png, BinaryData::FrequencyToLightPopupBackground_pngSize);
    
    imageStopButton                 = ImageCache::getFromMemory(BinaryData::Button_Stop_png, BinaryData::Button_Stop_pngSize);
    imageStartButton                = ImageCache::getFromMemory(BinaryData::Button_Start_png, BinaryData::Button_Start_pngSize);
    
    imageBlueButtonNormal           = ImageCache::getFromMemory(BinaryData::BlueButton_Normal_png, BinaryData::BlueButton_Normal_pngSize);
    imageBlueButtonSelected         = ImageCache::getFromMemory(BinaryData::BlueButton_Selected_png, BinaryData::BlueButton_Selected_pngSize);
    
    imageCloseButton         = ImageCache::getFromMemory(BinaryData::ShortcutClose2x_png, BinaryData::ShortcutClose2x_pngSize);
    
    imageSaveButton         = ImageCache::getFromMemory(BinaryData::SaveSettingsButton_png, BinaryData::SaveSettingsButton_pngSize);
    
    imageBlueCheckButtonNormal      = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Normal_Max_png, BinaryData::Button_Checkbox_Normal_Max_pngSize);
    imageBlueCheckButtonSelected    = ImageCache::getFromMemory(BinaryData::Button_Checkbox_Selected_Max_png, BinaryData::Button_Checkbox_Selected_Max_pngSize);
    
    int multY = 666;
    // toggle between these two
    button_Multiplication = new ImageButton();
    button_Multiplication->setTriggeredOnMouseDown(true);
    button_Multiplication->setImages (false, true, true,
                                      imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                      Image(), 1.000f, Colour (0x00000000),
                                      imageBlueButtonSelected, 1.0, Colour (0x00000000));
    
    button_Multiplication->setBounds(591, multY, 31, 31);
    addAndMakeVisible(button_Multiplication);
    
    button_Division = new ImageButton();
    button_Division->setTriggeredOnMouseDown(true);
    button_Division->setImages (false, true, true,
                                imageBlueButtonNormal, 0.999f, Colour (0x00000000),
                                Image(), 1.000f, Colour (0x00000000),
                                imageBlueButtonSelected, 1.0, Colour (0x00000000));
    
    button_Division->setBounds(822, multY, 31, 31);
    addAndMakeVisible(button_Division);
    
    multY += 43;
    
    textEditorMultiplication = new TextEditor("");
    textEditorMultiplication->setReturnKeyStartsNewLine(false);
    textEditorMultiplication->setMultiLine(false);
    textEditorMultiplication->setInputRestrictions(6, "0123456789.");
    textEditorMultiplication->setText("1.0");
    
    textEditorMultiplication->setLookAndFeel(&lookAndFeel);
    textEditorMultiplication->setJustification(Justification::centred);
    textEditorMultiplication->setFont(fontSemiBold);
    //    textEditorMultiplication->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorMultiplication->applyFontToAllText(fontSemiBold);
    textEditorMultiplication->applyColourToAllText(Colours::lightgrey);
    textEditorMultiplication->setBounds(633, multY, 111, 35);
    addAndMakeVisible(textEditorMultiplication);
    
    textEditorDivision= new TextEditor("");
    textEditorDivision->setReturnKeyStartsNewLine(false);
    textEditorDivision->setMultiLine(false);
    textEditorDivision->setInputRestrictions(6, "0123456789.");
    textEditorDivision->setText("1.0");
    
    textEditorDivision->setLookAndFeel(&lookAndFeel);
    textEditorDivision->setJustification(Justification::centred);
    textEditorDivision->setFont(fontSemiBold);
    //    textEditorDivision->setColour(TextEditor::textColourId, Colours::darkgrey);
    textEditorDivision->applyFontToAllText(fontSemiBold);
    textEditorDivision->applyColourToAllText(Colours::lightgrey);
    textEditorDivision->setBounds(864, multY, 111, 35);
    addAndMakeVisible(textEditorDivision);
    
    button_Close = new ImageButton();
    button_Close->setTriggeredOnMouseDown(true);
    button_Close->setImages (false, true, true,
                             imageCloseButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageCloseButton, 0.75, Colour (0x00000000));
    
    button_Close->setBounds(1048, 520, 28, 28);
    addAndMakeVisible(button_Close);
    
    button_Save = new ImageButton();
    button_Save->setTriggeredOnMouseDown(true);
    button_Save->setImages (false, true, true,
                             imageSaveButton, 0.999f, Colour (0x00000000),
                             Image(), 1.000f, Colour (0x00000000),
                             imageSaveButton, 0.75, Colour (0x00000000));
    
    button_Save->setBounds(668, 818, 235, 65);
    addAndMakeVisible(button_Save);
}

ManipulationPopupComponent::~ManipulationPopupComponent() { }

void ManipulationPopupComponent::resized()
{
    int multY = 666;
    
    button_Multiplication->setBounds(591 * scaleFactor, multY* scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    button_Division->setBounds(822 * scaleFactor, multY* scaleFactor, 31 * scaleFactor, 31 * scaleFactor);
    
    multY += 43;
    
    textEditorMultiplication->setBounds(633 * scaleFactor, multY* scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorMultiplication->setFont(33  * scaleFactor);
    
    textEditorDivision->setBounds(864 * scaleFactor, multY* scaleFactor, 111 * scaleFactor, 35 * scaleFactor);
    textEditorDivision->setFont(33  * scaleFactor);
    
    button_Close->setBounds(1048 * scaleFactor, 520 * scaleFactor, 28 * scaleFactor, 28 * scaleFactor);
    button_Save->setBounds(668 * scaleFactor, 818 * scaleFactor, 235 * scaleFactor, 65 * scaleFactor);
    
}
