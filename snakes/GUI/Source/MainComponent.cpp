#include "MainComponent.h"
#include "GUI.h"

//==============================================================================
MainComponent::MainComponent()
{
    bs::Webserver::Options o;
    o.ports = {7779};
    
    snakePit = std::make_unique<bs::SnakePit>();
    snakePit->addSnakeEggs ([this] () { return new GUI(*this); });
    snakePit->start (o);
    
    setSize (600, 400);
}

MainComponent::~MainComponent()
{
}

//==============================================================================
void MainComponent::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    
    if (images.size() > 0)
    {
        auto img = images[0];
        g.setImageResamplingQuality (Graphics::lowResamplingQuality);
        auto rc = Rectangle<float> (0, 0, img.getWidth() * 25, img.getHeight() * 25);
        g.drawImage (img, rc);
    }
}

void MainComponent::resized()
{
}

void MainComponent::mouseDown (const MouseEvent&)
{
    if (images.size() > 0)
        images.remove (0);
    repaint();
}
