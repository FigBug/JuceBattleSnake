#include "MainComponent.h"
#include "Miranda.h"

//==============================================================================
MainComponent::MainComponent()
{
    bs::Webserver::Options o;
    o.ports = {23543};
    
    snakePit = std::make_unique<bs::SnakePit>();
    snakePit->addSnake (*new Miranda());
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
}

void MainComponent::resized()
{
}
