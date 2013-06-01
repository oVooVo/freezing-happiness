#include "objectmanager.h"
#include <QHBoxLayout>

ObjectManager::ObjectManager(QWidget *parent) :
    Manager(parent)
{
    _tree = new ObjectTree(parent);
    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins(0,0,0,0);
    l->addWidget(_tree);
    setLayout(l);
}

void ObjectManager::setProject(Project *project)
{
    _tree->setProject(project);
    Manager::setProject(project);
}
