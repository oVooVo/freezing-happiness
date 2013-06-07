#include "renderoptions.h"
#include <QDebug>
#include <QWidget>
#include <QFormLayout>

RenderOptions::RenderOptions()
{
    _configurator = 0;
}

RenderOptions::~RenderOptions()
{
    if (_configurator)
        delete _configurator;
}

QWidget* RenderOptions::configurator()
{
    if (!_configurator)
        createConfigurator();
    return _configurator;
}

void RenderOptions::createConfigurator()
{
    _configurator = new QWidget();

}
