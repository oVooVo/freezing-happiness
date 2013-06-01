#ifndef PROPERTYMANAGER_H
#define PROPERTYMANAGER_H
#include <QTabWidget>
#include <FrontEnd/Manager/manager.h>
#include <QLabel>

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
    void updateProperties();
    
};

#endif // PROPERTYMANAGER_H