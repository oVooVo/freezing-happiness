#include <QDebug>
#include "psrgrabber.h"
#include <QPainter>
#include <qmath.h>
#include <BackEnd/mathutility.h>
#include <QCursor>
#include <QApplication>

#define TRANS_LENGTH 100
#define ROT_RADIUS 75
#define SCALE_LENGTH 71
#define CORE_SIZE 5
#define THRESHOLD 5

PSRGrabber::PSRGrabber(QWidget* parent)
{
    _particularMode = false;
    _rootManipulator = false;
    _project = 0;
    _isLiveMode = false;
    _mode = Object::ObjectMode;
    _pos = ObjectPosition;
    _rot = ObjectRotation;
    _parent = parent;
    _customPosition = 0;
    _customRotation = 0;
}

void PSRGrabber::paint(QPainter &p)
{

    if (_hidden || _rootManipulator || !_project) return;

    p.save();
    p.translate(_position);
    p.rotate(-_rotation * 180 * M_1_PI);

    QColor x = Qt::red;
    QColor y = Qt::blue;
    QColor rot = Qt::green;
    QColor scale = Qt::darkYellow;
    QColor selected = QColor(255, 255, 0);

    if (!_rootManipulator) {
        switch (_action) {
        case Move:
            x = selected;
            y = selected;
            break;
        case MoveX:
            x = selected;
            break;
        case MoveY:
            y = selected;
            break;
        case Rotate:
            rot = selected;
            break;
        case Scale:
            scale = selected;
            break;
        case None:
        default:
            break;
        }
    }

    //translation
    p.setPen(x);
    p.drawLine(QPoint(), QPoint(TRANS_LENGTH, 0));
    p.setPen(y);
    p.drawLine(QPoint(), QPoint(0, TRANS_LENGTH));
    p.drawEllipse(QPoint(), CORE_SIZE, CORE_SIZE);
    //rotation
    p.setPen(rot);
    p.drawEllipse(QPoint(), ROT_RADIUS, ROT_RADIUS);

    //scalation
    p.setPen(scale);
    p.drawLine(QPoint(), QPoint(-SCALE_LENGTH, -SCALE_LENGTH));
    p.fillRect(-SCALE_LENGTH - 3, -SCALE_LENGTH -3, 6, 6, scale);

    p.restore();
}

PSRGrabber::Action PSRGrabber::action(QPointF pos) const
{
    qreal distance = MathUtility::dist(pos, _position);
    if (distance <= CORE_SIZE) return Move;
    if (distance <= TRANS_LENGTH) {
        QPointF tester = pos - _position;
        tester = QPointF( tester.x() * qCos(-_rotation) + tester.y() * qSin(-_rotation),
                         -tester.x() * qSin(-_rotation) + tester.y() * qCos(-_rotation));
        if (tester.x() > 0 && tester.y() > 0) {
            if (fuzzyEqual(tester.x())) return  MoveY;
            if (fuzzyEqual(tester.y())) return  MoveX;
        }
    }
    if (fuzzyEqual(distance, ROT_RADIUS)) {
        return PSRGrabber::Rotate;
    }
    if (distance <= SCALE_LENGTH) {
        QPointF tester = pos - _position;
        tester = QPointF( tester.x() * qCos(-_rotation) + tester.y() * qSin(-_rotation),
                         -tester.x() * qSin(-_rotation) + tester.y() * qCos(-_rotation));
        if (fuzzyEqual(tester.x(), tester.y()) && tester.x() < 0 && tester.y() < 0)
            return Scale;
    }
    return None;
}

bool PSRGrabber::fuzzyEqual(qreal a, qreal b)
{
    return (a - THRESHOLD < b && a + THRESHOLD > b);
}

void PSRGrabber::setProject(Project* project)
{
    if (_project) {
        disconnect(_project, SIGNAL(selectionChanged()), this, SLOT(calculatePosition()));
        disconnect(_project, SIGNAL(selectionChanged()), this, SLOT(calculateRotation()));
    }
    _project = project;
    if (_project) {
        connect(_project, SIGNAL(selectionChanged()), this, SLOT(calculatePosition()));
        connect(_project, SIGNAL(selectionChanged()), this, SLOT(calculateRotation()));
    }
    calculatePosition();
    calculateRotation();

}

void PSRGrabber::calculatePosition()
{
    if (!_project) {
        _hidden = true;
        return;
    } else {
        _hidden = false;
    }

    if (_project->selectedParents().isEmpty() && !_rootManipulator) {
        _hidden = true;
        return;
    }

    switch (_pos) {
    case PSRGrabber::ObjectPosition: {
        if (_project->selectedParents().isEmpty()) {
            _position = _project->root()->globalePosition();
        } else {
            _position = QPointF(0, 0);
            foreach (Object* o, _project->selectedParents()) {
                _position += o->globalePosition();
            }
            _position /= _project->selectedParents().length();
        }
        break;
    }
    case PSRGrabber::ParentPosition:
        if (_project->selectionsParent()) {
            _position = _project->selectionsParent()->globalePosition();
        } else {
            _position = _project->root()->globalePosition();
        }
        break;
    case PSRGrabber::CustomPosition:
        if (_customPosition) {
            _position = _customPosition->globalePosition();
        } else {
            _position = _project->root()->globalePosition();
        }
        break;
    case PSRGrabber::RootPosition:
        _position = _project->root()->globalePosition();
        break;
    }
}

void PSRGrabber::calculateRotation()
{
    if (!_project) {
        _hidden = true;
        return;
    } else {
        _hidden = false;
    }

    if (_project->selectedParents().isEmpty() && !_rootManipulator) {
        _hidden = true;
        return;
    }
    switch (_rot) {
    case PSRGrabber::ObjectRotation:
        if (_project->selectedParents().size() == 1)  {
            _rotation = _project->selectedParents().first()->globaleRotation();
        } else {
            _rotation = _project->root()->globaleRotation();
        }
        break;
    case PSRGrabber::RootRotation:
        _rotation = _project->root()->globaleRotation();
        break;
    case PSRGrabber::ParentRotation:
        if (_project->selectionsParent()) {
            _position = _project->selectionsParent()->globalePosition();
        } else {
            _position = _project->root()->globalePosition();
        }
        break;
    case PSRGrabber::CustomRotation:
        if (_customRotation) {
            _rotation = _customRotation->globaleRotation();
        } else {
            _rotation = _project->root()->globaleRotation();
        }
        break;
    }
}

void PSRGrabber::setLeftButtonDownPosition(QPointF pos)
{
    if (!_project) return;
    if (!_rootManipulator) {
        _action = action(pos);
    } else {
        QApplication::setOverrideCursor(QCursor(Qt::BlankCursor));
    }
    _formerMousePosition = pos;
    _leftButtonDownPosition = pos;
    if (_action == Rotate) {
        _leftButtonDownObjectsRotations.clear();
        foreach (Object* object, _project->selectedParents()) {
            _leftButtonDownObjectsRotations.insert(object, object->globaleRotation());
        }
        _leftButtonDownPsrRotation = _rotation;
    } else if (_action == Scale) {
        _leftButtonDownObjectsScalation.clear();
        foreach (Object* object, _project->selectedParents()) {
            _leftButtonDownObjectsScalation.insert(object, object->globaleScalation());
        }
    }

}

void PSRGrabber::setMousePosition(QPointF pos)
{
    if (!_project) return;

    QList<Object*> subjects;
    if (_rootManipulator) {
        subjects = QList<Object*>() << _project->root();
    } else {
        subjects = _project->selectedParents();
    }

    if (_rootManipulator) {
        _mode = Object::ObjectMode;
    }


    if (_action == PSRGrabber::Move) {
        QPointF translation = pos - _formerMousePosition;
        foreach (Object* o, subjects) {
            o->setGlobalePosition(o->globalePosition() + translation, _mode);
        }
        _position += translation;
    } else if (_action == PSRGrabber::MoveX) {
        QPointF translation = constraint(pos - _formerMousePosition, _rotation);
        foreach (Object* o, subjects) {
            o->setGlobalePosition(o->globalePosition() + translation, _mode);
        }
        _position += translation;
    } else if (_action == PSRGrabber::MoveY) {
        QPointF translation = constraint(pos - _formerMousePosition, _rotation + M_PI_2);
        foreach (Object* o, subjects) {
            o->setGlobalePosition(o->globalePosition() + translation, _mode);
        }
        _position += translation;
    } else if (_action == PSRGrabber::Rotate) {
        foreach (Object* o, subjects) {
            qreal rotation = _leftButtonDownObjectsRotations[o]
                    + qAtan2(_position.y() - _leftButtonDownPosition.y(), _position.x() - _leftButtonDownPosition.x())
                    - qAtan2(_position.y() - pos.y(), _position.x() - pos.x());
            if (_particularMode) {
                o->setGlobaleRotation(rotation, _mode);
            } else {
                o->setGlobaleRotation(rotation, _mode, _position);
            }
        }
        qreal rotation = _leftButtonDownPsrRotation
                + qAtan2(_position.y() - _leftButtonDownPosition.y(), _position.x() - _leftButtonDownPosition.x())
                - qAtan2(_position.y() - pos.y(), _position.x() - pos.x());
        _rotation = rotation;
    } else if (_action == PSRGrabber::Scale) {
        if (_rootManipulator) {
            qreal scalation = pos.x()/ _formerMousePosition.x();
            foreach (Object* o, subjects) {
                qreal newScalation = qAbs(o->globaleScalation() * scalation);
                if (newScalation > 0.1) {
                    o->setGlobaleScalation(newScalation, _mode, _leftButtonDownPosition);
                }
            }
        } else {
            qreal oldDist = MathUtility::dist(_position, _formerMousePosition);
            qreal newDist = MathUtility::dist(_position, pos);
            qreal scalation = newDist / oldDist;
            if (oldDist > 0.0001 && newDist > 0.0001) {
                foreach (Object* o, subjects) {
                    qreal newScalation = o->globaleScalation() * scalation;
                    if (newScalation > 0.0001) {
                        if (_particularMode) {
                            o->setGlobaleScalation(newScalation, _mode);
                        } else {
                            o->setGlobaleScalation(newScalation, _mode, _position);
                        }
                    }
                }
            }
        }
    }
    if (_rootManipulator) {
        _formerMousePosition = _leftButtonDownPosition;
        QCursor::setPos(_parent->mapToGlobal(_leftButtonDownPosition.toPoint()));
    }
    else {
        _formerMousePosition = pos;
    }
}


QPointF PSRGrabber::constraint(QPointF pos, qreal angle)
{
    qreal dist = qCos(angle) * pos.x() - qSin(angle) * pos.y();
    return QPointF(dist * qCos(angle), -dist * qSin(angle));
}

void PSRGrabber::resetActions()
{
    _action = None;
    calculatePosition();
    calculateRotation();
    _rootManipulator = false;
    QApplication::restoreOverrideCursor();
}

void PSRGrabber::setMode(Object::Mode mode)
{
    if (_mode == mode) return;
    _mode = mode;
    emit modeChanged();
}

void PSRGrabber::setPositionMode(PSRGrabber::Position position)
{
    if (_pos == position) return;
    _pos = position;
    calculatePosition();
    emit modeChanged();
}

void PSRGrabber::setRotationMode(PSRGrabber::Rotation rotation)
{
    if (_rot == rotation) return;
    _rot = rotation;
    calculateRotation();
    emit modeChanged();
}

void PSRGrabber::setParticularMode(bool on)
{
    if (_particularMode == on) return;
    _particularMode = on;
    emit modeChanged();
}

void PSRGrabber::setCustomPositionObject(Object* object)
{
    if (_customPosition == object) return;
    _customPosition = object;
    calculatePosition();
    emit modeChanged();
}

void PSRGrabber::setCustomRotationObject(Object* object)
{
    if (_customRotation == object) return;
    _customRotation = object;
    calculateRotation();
    emit modeChanged();
}

void PSRGrabber::update()
{
    if (!_isLiveMode)
    {
        calculatePosition();
        calculateRotation();
    }
}
