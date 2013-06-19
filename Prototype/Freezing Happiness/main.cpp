#include <BackEnd/mathutility.h>
#include <QCoreApplication>
#include <BackEnd/project.h>
#include <QDebug>
#include <iostream>
#include <QFile>
#include <QApplication>
#include <BackEnd/Objects/empty.h>
#include <BackEnd/Objects/root.h>
#include <QPointF>
#include <QTransform>
#include <qmath.h>
#include <FrontEnd/mainwindow.h>
#include "FrontEnd/splineedit.h"
#include "BackEnd/Properties/splineproperty.h"

void saveProject(Project *p);
Project* loadProject();
void testBackend();
void printList(QList<Object *> list);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //MainWindow w;
    //w.show();

    //testBackend();


    SplineEdit* splineEdit = new SplineEdit(new SplineProperty("Test", "TTEst"), 0);
    splineEdit->data()->addPoint(QPointF(0,1));
    splineEdit->data()->addPoint(QPointF(0.5,0.5));
    splineEdit->data()->addPoint(QPointF(1,1));
    splineEdit->show();

    return a.exec();
}

void saveProject(Project* p)
{
    QFile file("/home/pascal/test");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);   // we will serialize the data into the file
    out << p;
}

Project* loadProject()
{
    QFile file("/home/pascal/test");
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);    // read the data serialized from the file
    Project* p;
    in >> p;
    return p;
}

void printList(QList<Object *> list)
{
    QString s = "(";
    foreach (Object* o, list) {
        s = s.append(o->name()).append(",");
    }
    qDebug() << s.append(")");
}

void testBackend()
{
    Project* p = new Project();
    p->setRoot(new Root(p));
    Empty* Dieter = new Empty(p, "Dieter");
    Empty* Detlef = new Empty(p, "Detlef");
    Empty* Dagmar = new Empty(p, "Dagmar");
    //Empty* Dagobert = new Empty(p, "Dagobert");
    Detlef->setTreeParent(Dieter);
    Dagmar->setTreeParent(Dieter);
    Dieter->setLocalePosition(QPointF(100,100));
    Detlef->setLocalePosition(QPointF(150,150));
    Dieter->setLocaleRotation(2);

    Dieter->setSelected(true);
    qDebug() << p->visualize();

    saveProject(p);
    delete p;
    p = loadProject();

    qDebug() << p->visualize();
    printList(p->objects());
    printList(p->selectedParents());
    printList(p->selectedChildren());
    printList(p->selectedObjects());
}
