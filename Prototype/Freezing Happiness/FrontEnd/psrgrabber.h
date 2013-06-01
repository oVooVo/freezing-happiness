#ifndef PSRGRABBER_H
#define PSRGRABBER_H

#include <QPoint>
#include <QPainter>
#include "BackEnd/Objects/object.h"
#include "BackEnd/project.h"
#include <QWidget>

class PSRGrabber : public QObject
{
    Q_OBJECT
public:
    enum Action { None,     //perform no action
                  Move,     //move freely
                  MoveX,    //move restricted to x-Axis
                  MoveY,    //move restricted to y-Axis
                  Rotate,   //rotate
                  Scale     //scale
                };
    enum Position { ObjectPosition,     //Set grabber's position to mean of all selected Objects.
                    ParentPosition,     //Set grabber's position to first common parent's position.
                    CustomPosition,     //Set grabber's position to position of a custom object.
                    RootPosition        //Set grabber's position to root's position.
                  };
    enum Rotation { ObjectRotation,     //Set grabber's rotation to selected Object's rotation.
                                //If more then one Object is selected, set grabber's rotation to root's direction.
                    ParentRotation,     //Set grabber's direction to first common parent's rotation.
                    RootRotation,       //Set grabber's direction to root's direction.
                    CustomRotation      //Set grabber's direction to direction of a custom object.
                  };


    PSRGrabber(QWidget* parent);

    void paint(QPainter &p);
    void setProject(Project* project);
    Project* project() const { return _project; }
    void setLeftButtonDownPosition(QPointF pos);
    void setMousePosition(QPointF pos);
    void resetActions();
    void setAction(Action action) { _action = action; }
    void setRootManipulationMode(bool rootManipulationMode) { _rootManipulator = rootManipulationMode; }
    Object::Mode mode() const { return _mode; }
    PSRGrabber::Position positionMode() const { return _pos; }
    PSRGrabber::Rotation rotationMode() const { return _rot; }
    bool particularMode() const { return _particularMode; }
    void setLiveMode(bool on) { _isLiveMode = on; }

    Object* customPositionObject() const { return _customPosition; }

public slots:
    void setMode(Object::Mode mode);
    void setPositionMode(PSRGrabber::Position position);
    void setRotationMode(PSRGrabber::Rotation rotation);
    void setParticularMode(bool on);
    void setCustomPositionObject(Object* object);
    void setCustomRotationObject(Object* object);
    void update();

private slots:
    void calculatePosition();
    void calculateRotation();

private:
    Action _action;
    Object::Mode _mode;
    Position _pos;
    Rotation _rot;
    bool _particularMode;

    Object* _customPosition;
    Object* _customRotation;

    QPointF _leftButtonDownPosition;
    QPointF _formerMousePosition;
    QMap<Object*, qreal> _leftButtonDownObjectsRotations;
    QMap<Object*, qreal> _leftButtonDownObjectsScalation;

    qreal _leftButtonDownPsrRotation;
    static bool fuzzyEqual(qreal a, qreal b = 0);

    Action action(QPointF pos) const;
    bool _hidden;

    QPointF _position;
    qreal _rotation;
    Project* _project;

    QPointF constraint(QPointF pos, qreal angle);

    bool _rootManipulator;
    bool _isLiveMode;
    QWidget* _parent;

signals:
    void modeChanged();

};

#endif // PSRGRABBER_H
