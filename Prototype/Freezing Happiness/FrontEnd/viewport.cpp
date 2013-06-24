#include "viewport.h"
#include <QPainter>
#include <qmath.h>
#include <QEvent>
#include <QMouseEvent>
#include <QDebug>
#include "BackEnd/mathutility.h"
#include <QShortcut>
#include <QEventLoop>
#include <FrontEnd/combobox.h>
#include <BackEnd/Objects/empty.h>
#include "BackEnd/Objects/point.h"
#include "BackEnd/Tags/pointtag.h"

Viewport::Viewport(QWidget *parent) : Manager(parent)
{
    installEventFilter(this);
    _psr = new PSRGrabber(this);
    connect(_psr, SIGNAL(modeChanged()), this, SLOT(emitModeChanged()));
    connect(_psr, SIGNAL(modeChanged()), this, SLOT(update()));
}


bool Viewport::eventFilter(QObject *o, QEvent *e)
{
    if (project()) {
        if (e->type() == QEvent::MouseButtonDblClick || e->type() == QEvent::MouseButtonPress
                          || e->type() == QEvent::MouseButtonRelease || e->type() == QEvent::MouseMove) {
            QMouseEvent* mouseEvent = (QMouseEvent*) e;
            if (mouseEvent->modifiers() == Qt::NoModifier) {
                _psr->setRootManipulationMode(false);
                if (e->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
                    _psr->setLiveMode(true);
                    project()->createNewUndoRecord();
                    _psr->setLeftButtonDownPosition(mouseEvent->pos());
                    _lastPos = mouseEvent->pos();
                    _grabbedPoint = 0;
                    for (Object* object : project()->selectedObjects()) {
                        if (object->type() == "Point" || object->hasTag("PointTag")) {
                            Point* point = (Point*) object;
                            if (isInEnvironmentOf(
                                        point->inherits("Point") ? point->globaleCtrlA() : ((PointTag*) point->tag("PointTag"))->globaleCtrlA(),
                                        mouseEvent->pos())) {
                                _grabbedPoint = point;
                                _tangentA = true;
                                break;
                            } else if (isInEnvironmentOf(
                                         point->inherits("Point") ? point->globaleCtrlB() : ((PointTag*) point->tag("PointTag"))->globaleCtrlB(),
                                         mouseEvent->pos())) {
                                _grabbedPoint = point;
                                _tangentA = false;
                                break;
                            }
                        }
                    }
                } else if (e->type() == QEvent::MouseMove) {
                    project()->setRecordHistory(false);
                    if (_grabbedPoint) {
                        //TODO non grabbed tangent behaves badly
                        if (_tangentA) {
                            if (_grabbedPoint->inherits("Point")) {
                                Point* point = (Point*) _grabbedPoint;
                                point->setGlobaleCtrlA(point->globaleCtrlA() + mouseEvent->pos() - _lastPos);
                                point->setGlobaleCtrlB(point->globaleCtrlB() - mouseEvent->pos() + _lastPos);
                            } else if (_grabbedPoint->hasTag("PointTag")) {
                                PointTag* tag = (PointTag*) _grabbedPoint->tag("PointTag");
                                tag->setGlobaleCtrlA(tag->globaleCtrlA() + mouseEvent->pos() - _lastPos);
                                tag->setGlobaleCtrlB(tag->globaleCtrlB() - mouseEvent->pos() + _lastPos);
                            } else {
                                Q_ASSERT(false);
                            }
                        } else {
                            if (_grabbedPoint->inherits("Point")) {
                                Point* point = (Point*) _grabbedPoint;
                                point->setGlobaleCtrlB(point->globaleCtrlB() + mouseEvent->pos() - _lastPos);
                                point->setGlobaleCtrlA(point->globaleCtrlA() - mouseEvent->pos() + _lastPos);
                            } else if (_grabbedPoint->hasTag("PointTag")) {
                                PointTag* tag = (PointTag*) _grabbedPoint->tag("PointTag");
                                tag->setGlobaleCtrlB(tag->globaleCtrlB() + mouseEvent->pos() - _lastPos);
                                tag->setGlobaleCtrlA(tag->globaleCtrlA() - mouseEvent->pos() + _lastPos);
                            } else {
                                Q_ASSERT(false);
                            }
                        }
                        _lastPos = mouseEvent->pos();
                    } else {
                        _psr->setMousePosition(mouseEvent->pos());
                    }
                    project()->setRecordHistory(true);
                } else if (e->type() == QEvent::MouseButtonRelease && mouseEvent->button() == Qt::LeftButton) {
                    _psr->resetActions();
                    _psr->setLiveMode(false);
                    foreach (Object* o, project()->selectedChildren()) {
                        o->emitObjectChanged();
                    }
                } else if (e->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::RightButton) {
                    Object* cObject = clickedObject(mouseEvent->pos(), true);
                    project()->blockSignals(true);
                    foreach (Object* o, project()->selectedObjects()) {
                        o->setSelected(false);
                    }
                    if (cObject) {
                        cObject->setSelected(true);
                    }
                    project()->blockSignals(false);
                    project()->emitSelectionChanged();
                } else if (e->type() == QEvent::MouseButtonDblClick && mouseEvent->button() == Qt::RightButton) {
                    Object* cObject = clickedObject(mouseEvent->pos(), false);
                    project()->blockSignals(true);
                    foreach (Object* o, project()->selectedObjects()) {
                        o->setSelected(false);
                    }
                    if (cObject) {
                        cObject->setSelected(true);
                    }
                    project()->blockSignals(false);
                    project()->emitSelectionChanged();
                }
            } else if (mouseEvent->modifiers() == Qt::ControlModifier) {
                if (e->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::RightButton) {
                    Object* cObject = clickedObject(mouseEvent->pos(), true);
                    if (cObject) {
                        cObject->setSelected(!cObject->isSelected());
                    }
                } else if (e->type() == QEvent::MouseButtonDblClick && mouseEvent->button() == Qt::RightButton) {
                    Object* cObject = clickedObject(mouseEvent->pos(), false);
                    if (cObject) {
                        cObject->setSelected(!cObject->isSelected());
                    }
                } else if (e->type() == QEvent::MouseButtonPress && mouseEvent->button() == Qt::LeftButton) {
                    newPoint(mouseEvent->pos());
                }
            } else if (mouseEvent->modifiers() == Qt::AltModifier) {
                _psr->setRootManipulationMode(true);
                if (e->type() == QEvent::MouseButtonPress) {
                    project()->createNewUndoRecord();
                    _psr->setLiveMode(true);
                    if (mouseEvent->button() == Qt::LeftButton) {
                        _psr->setAction(PSRGrabber::Move);
                    } else if (mouseEvent->button() == Qt::RightButton) {
                        _psr->setAction(PSRGrabber::Scale);
                    }
                    _psr->setLeftButtonDownPosition(mouseEvent->pos());
                } else if (e->type() == QEvent::MouseMove) {
                    project()->setRecordHistory(false);
                    _psr->setMousePosition(mouseEvent->pos());
                    project()->setRecordHistory(true);
                } else if (e->type() == QEvent::MouseButtonRelease) {
                    _psr->resetActions();
                    _psr->setLiveMode(false);
                }
            } else if (mouseEvent->modifiers() == Qt::ShiftModifier) {
                if (e->type() == QEvent::MouseMove) {
                    project()->setRecordHistory(false);
                    if (_grabbedPoint) {
                        if (_tangentA) {
                            if (_grabbedPoint->inherits("Point")) {
                                Point* point = (Point*) _grabbedPoint;
                                point->setGlobaleCtrlA(point->globaleCtrlA() + mouseEvent->pos() - _lastPos);
                            } else if (_grabbedPoint->hasTag("PointTag")) {
                                PointTag* tag = (PointTag*) _grabbedPoint->tag("PointTag");
                                tag->setGlobaleCtrlA(tag->globaleCtrlA() + mouseEvent->pos() - _lastPos);
                            }
                        } else {
                            if (_grabbedPoint->inherits("Point")) {
                                Point* point = (Point*) _grabbedPoint;
                                point->setGlobaleCtrlB(point->globaleCtrlB() + mouseEvent->pos() - _lastPos);
                            } else if (_grabbedPoint->hasTag("PointTag")) {
                                PointTag* tag = (PointTag*) _grabbedPoint->tag("PointTag");
                                tag->setGlobaleCtrlB(tag->globaleCtrlB() + mouseEvent->pos() - _lastPos);
                            }                        }
                        _lastPos = mouseEvent->pos();
                    } else {
                        _psr->setMousePosition(mouseEvent->pos());
                    }
                    project()->setRecordHistory(true);
                }
            }
        }
    }
    if (e->type() == QEvent::MouseButtonRelease) {
        _grabbedPoint = 0;
        update();
    } else if (e->type() == QEvent::FocusOut) {
        _grabbedPoint = 0;
    }
    return QWidget::eventFilter(o, e);

}

void Viewport::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    p.fillRect(0,0,width(), height(), Qt::white);
    if (!project()) return;

    project()->paint(p);
    p.resetTransform();
    _psr->paint(p);
}

void Viewport::emitModeChanged()
{
    emit modeChanged();
}

void Viewport::setProject(Project *project)
{
    Manager::setProject(project);
    _psr->setProject(project);
}

void Viewport::update()
{
    _psr->update();
    QWidget::update();
}

Object* Viewport::clickedObject(QPoint pos, bool askObject)
{
    if (!project()) return 0;

    QList<Object*> objects;
    foreach (Object* cand, project()->objects()) {
        if (isInEnvironmentOf(pos, cand)) {
            if (askObject) {
                objects.append(cand);
            } else {
                return cand;
            }
        }
    }
    if (!askObject || objects.isEmpty()) {
        return 0;
    } else if (objects.size() == 1) {
        return objects[0];
    }

    /*
    ComboBox* box = new ComboBox();
    QStringList items;
    foreach (Object* object, objects) {
        items << object->name();
    }

    box->addItems(items);
    box->setCurrentIndex(-1);
    box->move(QCursor::pos());
    box->setWindowFlags(Qt::FramelessWindowHint);
    box->show();
    Object* returnedObject = 0;
    void (QComboBox:: *currentIndexChangedInt)(int) = &QComboBox::currentIndexChanged;
    connect(box, currentIndexChangedInt, [&](const int &index){ returnedObject = objects[index]; } );
    QEventLoop loop;
    connect(box, SIGNAL(currentIndexChanged(int)), &loop, SLOT(quit()));
    connect(box, SIGNAL(lostFocus()), &loop, SLOT(quit()));
    loop.exec();   
    delete box;
    return returnedObject;
    */
    return objects[0];
}

bool Viewport::isInEnvironmentOf(QPointF pos, Object *o)
{
    if (!o) return false;
    return isInEnvironmentOf(pos, o->globalePosition());
}

bool Viewport::isInEnvironmentOf(QPointF p1, QPointF p2)
{
    if ((p1 - p2).manhattanLength() < 10) {
        return true;
    } else {
        return false;
    }
}

void Viewport::newPoint(QPoint pos)
{
    project()->addPoint(pos);
}



