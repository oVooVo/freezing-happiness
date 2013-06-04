#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H
#include <QTabWidget>
#include <FrontEnd/Manager/manager.h>
#include <QLabel>
#include "BackEnd/Tags/tag.h"

class PropertyManager : public Manager
{
    Q_OBJECT
public:
    explicit PropertyManager(QWidget *parent = 0);
    
protected slots:
    void updateSelection() { updateProperties(); }
    void updateStructure() { updateProperties(); }

private:
    QTabWidget* _tabWidget;
    QLabel* _objectsLabel;
    QList<QList<Tag*>> _tagMap;
    void updateProperties();
    
};

#endif // PROPERTYMANAGER_H
