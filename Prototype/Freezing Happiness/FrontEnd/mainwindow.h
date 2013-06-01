#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "BackEnd/Objects/object.h"
#include "BackEnd/projectcontainer.h"
#include "Manager/objectmanager.h"
#include "FrontEnd/viewport.h"
#include "BackEnd/projectcontainer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void addManager(Manager* manager);
    
private:
    Ui::MainWindow *ui;
    ProjectContainer* _pc;
    QString _filepath;
    QList<Manager*> _managers;

private slots:
    void updateActiveProject(Project* project);
    void load(bool createNew = false);
    void save();
};

#endif // MAINWINDOW_H
