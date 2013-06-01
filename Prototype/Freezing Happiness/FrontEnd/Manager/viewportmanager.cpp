#include "viewportmanager.h"
#include <QDebug>
#include <QVBoxLayout>

ViewportManager::ViewportManager(QWidget* parent) : Manager(parent)
{
    _viewPort = new Viewport(this);
    _navigationToolbar = new NavigationToolbar(parent);
    _navigationToolbar->setViewport(_viewPort);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_navigationToolbar);
    layout->addWidget(_viewPort);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

void ViewportManager::updateSelection()
{
    _viewPort->update();
}

void ViewportManager::updateStructure()
{
   _viewPort->update();
}

void ViewportManager::updateObject(Object *)
{
    _viewPort->update();
}

void ViewportManager::setProject(Project *project)
{
    _navigationToolbar->setViewport(_viewPort);
    _navigationToolbar->setProject(project);
    _viewPort->setProject(project);
    Manager::setProject(project);
}
