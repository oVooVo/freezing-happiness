#ifndef MANAGER_H
#define MANAGER_H

#include <BackEnd/project.h>
#include <QWidget>
#include <QDebug>

class Manager : public QWidget
{
    Q_OBJECT
public:
    Manager(QWidget* parent = 0);
    Project* project() const { return _project; }
    virtual void setProject(Project* project);

protected slots:
    virtual void updateSelection() {}
    virtual void updateStructure() {}
    virtual void updateObject(Object*) {}

private:
    Project* _project;
};

#endif // MANAGER_H
