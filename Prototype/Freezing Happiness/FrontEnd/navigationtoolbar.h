#ifndef NAVIGATIONTOOLBAR_H
#define NAVIGATIONTOOLBAR_H

#include <QWidget>
#include "FrontEnd/viewport.h"
#include "FrontEnd/EditorWidgets/objectedit.h"

namespace Ui {
class NavigationToolbar;
}

class NavigationToolbar : public QWidget
{
    Q_OBJECT
    
public:
    explicit NavigationToolbar(QWidget *parent = 0);
    ~NavigationToolbar();
    void setViewport(Viewport* viewport);
    void setProject(Project* project);

    
private:
    Ui::NavigationToolbar *ui;
    Viewport* _viewport;
    Project* _project;
    ObjectEdit* _positionEdit;
    ObjectEdit* _rotationEdit;

private slots:
    void updateComboBoxes();
    void onModeComboBoxIndexChanged(int index);
    void onPositionComboBoxIndexChanged(int index);
    void onOrientationComboBoxIndexChanged(int index);
    void onParticularButtonCheckedChanged(bool checked);
    void onCustomRotationEditChanged();
    void onCustomPositionEditChanged();

};

#endif // NAVIGATIONTOOLBAR_H
