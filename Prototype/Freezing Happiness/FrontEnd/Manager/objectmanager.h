#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include "manager.h"
#include "FrontEnd/objecttree.h"

class ObjectManager : public Manager
{
    Q_OBJECT
public:
    explicit ObjectManager(QWidget *parent = 0);
    void setProject(Project *project);

private:
    ObjectTree* _tree;
    
};

#endif // OBJECTMANAGER_H
