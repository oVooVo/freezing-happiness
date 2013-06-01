#include "manager.h"
#include <QDebug>

Manager::Manager(QWidget *parent) : QWidget(parent)
{
    _project = 0;
}

void Manager::setProject(Project *project)
{
    _project = project;
    if (_project) {
        connect(_project, SIGNAL(structureChanged()), this, SLOT(updateStructure()));
        connect(_project, SIGNAL(objectChanged(Object*)), this, SLOT(updateObject(Object*)));
        connect(_project, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
        updateStructure();
        updateSelection();
    }
}
