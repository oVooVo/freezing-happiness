#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "FrontEnd/viewport.h"
#include "BackEnd/Objects/empty.h"
#include "BackEnd/Objects/root.h"
#include "BackEnd/Objects/instance.h"
#include <QDebug>
#include <QHBoxLayout>
#include "navigationtoolbar.h"
#include <QKeyEvent>
#include <QTimer>
#include <BackEnd/Objects/spline.h>
#include <FrontEnd/Manager/viewportmanager.h>
#include <FrontEnd/Manager/propertymanager.h>
#include <QFileDialog>
#include <qsettings.h>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qApp->setOrganizationName("Hallo");
    qApp->setApplicationName("Welt");

    QSettings s;
    s.beginGroup("UI");
    restoreGeometry(s.value("geo").toByteArray());
    restoreState(s.value("status").toByteArray());
    s.endGroup();

    installEventFilter(this);
    _pc = 0;
    load(true);

    ObjectManager* t = new ObjectManager(this);
    addManager(t);
    ViewportManager* v = new ViewportManager(this);
    addManager(v);
    PropertyManager* propManager = new PropertyManager(this);
    addManager(propManager);


    ui->objectTreeWidget->setWidget(t);
    ui->attributeWidget->setWidget(propManager);
    setCentralWidget(v);

    connect(ui->actionLoad, SIGNAL(triggered()), this, SLOT(load()));
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionNew, &QAction::triggered, [this]() { load(true); });
    connect(ui->actionEmpty, &QAction::triggered, [=]() { new Empty(_pc->project()); } );
    connect(ui->actionSpline, &QAction::triggered, [=]() { new Spline(_pc->project()); } );
    connect(ui->actionInstance, &QAction::triggered, [=]() { new Instance(_pc->project()); } );


}

MainWindow::~MainWindow()
{
    QSettings s;
    s.beginGroup("UI");
    s.setValue("geo", saveGeometry());
    s.setValue("status", saveState());
    s.endGroup();
    delete ui;
}

void MainWindow::updateActiveProject(Project *project)
{
    foreach (Manager* m, _managers) {
        m->setProject(project);
    }

    connect(ui->actionUndo, SIGNAL(triggered()), project, SLOT(undo()));
    connect(ui->actionRedo, SIGNAL(triggered()), project, SLOT(redo()));
    connect(ui->actionDuplicate_Selected, SIGNAL(triggered()), project, SLOT(duplicateSelected()));
}

void MainWindow::addManager(Manager *manager)
{
    if (_pc) {
        manager->setProject(_pc->project());
    } else {
        manager->setProject(0);
    }
    _managers.append(manager);
}

void MainWindow::load(bool createNew)
{
    if (_filepath.isEmpty()) {
        _filepath = QDir::homePath();
    }
    if (!createNew) {
        _filepath = QFileDialog::getOpenFileName(this, tr("Open Project"), _filepath);
    }
    Project* p = 0;
    if (!createNew && !_filepath.isEmpty()) {
        QFile file(_filepath);
        file.open(QIODevice::ReadOnly);
        if (!file.exists()) {
            qDebug() << "file does not exists."; return;
        } else if (!file.isReadable()) {
            qDebug() << "file is not readable."; return;
        } else {
            QDataStream stream(&file);
            stream >> p;
        }
        file.close();
    } else if (createNew) {
        p = new Project();
        p->setRoot(new Root(p));
    }
    if (p) {

        disconnect(_pc);
        disconnect(ui->actionEmpty);

        p->setRecordHistory(true);
        delete _pc;
        _pc = new ProjectContainer(p);
        updateActiveProject(p);

        ui->actionUndo->setEnabled(_pc->canUndo());
        ui->actionRedo->setEnabled(_pc->canRedo());
        connect(ui->actionUndo, SIGNAL(triggered()), _pc->project(), SLOT(undo()));
        connect(ui->actionRedo, SIGNAL(triggered()), _pc->project(), SLOT(redo()));
        connect(_pc, SIGNAL(canRedoChanged(bool)), ui->actionRedo, SLOT(setEnabled(bool)));
        connect(_pc, SIGNAL(canUndoChanged(bool)), ui->actionUndo, SLOT(setEnabled(bool)));
        connect(_pc, SIGNAL(projectChanged(Project*)), this, SLOT(updateActiveProject(Project*)));
    }
}

void MainWindow::save()
{
    if (_filepath.isEmpty()) {
        _filepath = QDir::homePath();
    }
    QString saveString = QFileDialog::getSaveFileName(this, tr("Save Project"), _filepath);
    if (!saveString.isEmpty()) {
        QFile file(saveString);
        file.open(QIODevice::WriteOnly);
        if (!file.isWritable()) {
            qDebug() << "File is not writable.";
        } else {
            QDataStream stream(&file);
            stream << _pc->project();
            _filepath = QDir(saveString).absolutePath();
        }
        file.close();
    }
}
