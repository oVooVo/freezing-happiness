#ifndef VIEWPORTMANAGER_H
#define VIEWPORTMANAGER_H

#include "FrontEnd/Manager/manager.h"
#include "FrontEnd/viewport.h"
#include "FrontEnd/navigationtoolbar.h"

class ViewportManager : public Manager
{
public:
    ViewportManager(QWidget *parent = 0);
    void setProject(Project *project);

protected slots:
    void updateSelection();
    void updateStructure();
    void updateObject(Object *);

private:
    Viewport* _viewPort;
    NavigationToolbar* _navigationToolbar;

};

#endif // VIEWPORTMANAGER_H
