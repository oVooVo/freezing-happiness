#ifndef GLVIEWPORT_H
#define GLVIEWPORT_H

#include <FrontEnd/Manager/manager.h>
#include "FrontEnd/psrgrabber.h"
#include <BackEnd/project.h>
#include "BackEnd/Objects/point.h"


class Viewport : public Manager
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent = 0);
    void setLeftButtonDownPosition(QPointF pos);
    void setMousePosition(QPointF pos);
    Object::Mode mode() const { return _psr->mode(); }
    PSRGrabber::Position positionMode() const { return _psr->positionMode(); }
    PSRGrabber::Rotation rotationMode() const { return _psr->rotationMode(); }
    bool particularMode() const { return _psr->particularMode(); }
    void setProject(Project *project);

public slots:
    void setMode(Object::Mode mode) { _psr->setMode(mode); }
    void setPositionMode(PSRGrabber::Position position) { _psr->setPositionMode(position); }
    void setRotationMode(PSRGrabber::Rotation rotation) { _psr->setRotationMode(rotation); }
    void setParticularMode(bool on) { _psr->setParticularMode(on); }
    void setCustomPositionObject(Object* o) { _psr->setCustomPositionObject(o); }
    void setCustomRotationObject(Object* o) { _psr->setCustomRotationObject(o); }
    void update();

protected:
    void paintEvent(QPaintEvent *);
    bool eventFilter(QObject *o, QEvent *e);
    void updateSelection() { update(); }
    void updateStructure() { update(); }
    void updateObject(Object *) { update(); }


private:
    PSRGrabber* _psr;
    Object* clickedObject(QPoint pos, bool askObject);
    bool isInEnvironmentOf(QPointF pos, Object* o);
    bool isInEnvironmentOf(QPointF p1, QPointF p2);
    void newPoint(QPoint pos);

    QPointF _lastPos;
    Point* _grabbedPoint = 0;
    bool _tangentA;

private slots:
    void emitModeChanged();

signals:
    void modeChanged();

};

#endif // GLVIEWPORT_H
