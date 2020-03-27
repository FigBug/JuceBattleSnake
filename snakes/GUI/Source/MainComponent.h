#pragma once

#include <JuceHeader.h>

//==============================================================================
class MainComponent   : public Component
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    void mouseDown (const MouseEvent&) override;

    Array<Image> images;
private:
    //==============================================================================
    std::unique_ptr<bs::SnakePit> snakePit;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
