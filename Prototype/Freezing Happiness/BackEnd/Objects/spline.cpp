#include "spline.h"
#include "point.h"
#include "BackEnd/Tags/pointtag.h"
#include "BackEnd/Properties/boolproperty.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/Tags/styletag.h"
#include <QEvent>
#include <QDebug>
#include <qmath.h>
#include "BackEnd/Objects/cloner.h"
#include "BackEnd/mathutility.h"

REGISTER_DEFN_OBJECTTYPE(Spline);

QPointF getCtrlA(Object* object);
QPointF getCtrlB(Object* object);

Spline::Spline(Project* project, QString name) : PathObject(project, name)
{
    addProperty("closed", new BoolProperty("Spline", "Closed", false));
    addProperty("Interpolation", new SelectProperty("Spline", tr("Interpolation"), 0, QStringList() << tr("Linear") << tr("Tangent")));
    addProperty("smooth", new BoolProperty("Spline", tr("Smooth"), false, true));
    polish();
    installEventFilter(this);
}

void Spline::childrenHasChanged()
{
    emitObjectChanged();
    updatePath();
}

void Spline::updatePath()
{
    _points.clear();
    QList<Object*> extras;
    for (Object* child : directChildren()) {
        if (child->type() == "Point") {
            _points.append((Point*) child);
        } else if (child->hasTag("PointTag")) {
           extras.append(child);
        }
    }
    qSort(extras.begin(), extras.end(), [](Object* a, Object* b)
        { return ((PointTag*) a->tag("PointTag"))->index() > ((PointTag*) b->tag("PointTag"))->index(); } );
    for (Object* extraObject : extras) {
        _points.insert(((PointTag*) extraObject->tag("PointTag"))->index(), extraObject);
    }

    if (((BoolProperty*) properties()["closed"])->value() && !_points.isEmpty()) {
        _points.append(_points.first());
    }

    if (!_points.isEmpty()) {
        _path = QPainterPath(_points.first()->localePosition());
        for (int i = 1; i < _points.size(); i++) {
            if (((SelectProperty*) properties()["Interpolation"])->currentIndex() == 0) {
                _path.lineTo(_points[i]->localePosition());
            } else if (((SelectProperty*) properties()["Interpolation"])->currentIndex() == 1) {
                _path.cubicTo(getCtrlB(_points[i-1]),
                              getCtrlA(_points[i]),
                              _points[i]->localePosition());
            }
        }
    } else {
        _path = QPainterPath();
    }
}

bool Spline::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && (e->type() == QEvent::ChildAdded || e->type() == QEvent::ChildRemoved)) {
        updatePath();
    }
    return Object::eventFilter(o,e);
}

QTransform Spline::getLocaleTransformAt(qreal pos)
{
    qreal eps = 0.001;
    qreal s1 = pos - eps;
    qreal s2 = pos + eps;

    if (s1 < 0) {
        s1 = 0;
        s2 = eps;
    } else if (s2 > 1) {
        s1 = 1-eps;
        s2 = 1;
    }


    QPointF p = _path.pointAtPercent(pos);
    QPointF sek1 = _path.pointAtPercent(s1);
    QPointF sek2 = _path.pointAtPercent(s2);
    qreal r = qAtan2((sek1.y() - sek2.y()),
                     (sek1.x() - sek2.x()));
    QTransform t;
    t.translate(p.x(), p.y());
    t.rotate(r * 180 * M_1_PI);
    return t;
}

QPointF getCtrlA(Object* object)
{
    if (object->inherits("Point")) {
        return ((Point*) object)->ctrlA();
    } else if (object->hasTag("PointTag")) {
        return ((PointTag*) object->tag("PointTag"))->ctrlA();
    } else {
        Q_ASSERT(false);
        return QPointF();
    }
}

QPointF getCtrlB(Object* object)
{
    if (object->inherits("Point")) {
        return ((Point*) object)->ctrlB();
    } else if (object->hasTag("PointTag")) {
        return ((PointTag*) object->tag("PointTag"))->ctrlB();
    } else {
        Q_ASSERT(false);
        return QPointF();
    }
}

void Spline::smoothTangents()
{
    for (int i = 0; i < _points.size(); i++) {
        int j = i-1;
        int k = i+1;
        if (i == 0 || i == _points.size() - 1)  {
            if (((BoolProperty* ) properties()["closed"])->value()) {
                if (i == 0) {
                    j = _points.size() - 1;
                } else if (i == _points.size() - 1) {
                    k = 0;
                }
            } else {
                if (_points[i]->inherits("Point")) {
                    ((Point*) _points[i])->setLocaleCtrlA(QPointF());
                    ((Point*) _points[i])->setLocaleCtrlB(QPointF());
                } else if (_points[i]->hasTag("PointTag")) {
                    ((PointTag*) _points[i]->tag("PointTag"))->setLocaleCtrlA(QPointF());
                    ((PointTag*) _points[i]->tag("PointTag"))->setLocaleCtrlB(QPointF());
                } else {
                    Q_ASSERT(false);
                }
                continue;
            }
        }
        qreal diffX = _points[j]->localePosition().x() - _points[k]->localePosition().x();
        qreal diffY = _points[j]->localePosition().y() - _points[k]->localePosition().y();
        QPointF aA = _points[j]->localePosition() - _points[i]->localePosition();
        QPointF aB = _points[k]->localePosition() - _points[i]->localePosition();
        qreal a = MathUtility::dist(aA) + MathUtility::dist(aB);
        QPointF ctrlA = _points[i]->localePosition() + a * 0.168 * QPointF(diffX, diffY) / MathUtility::dist(QPointF(diffX, diffY));
        QPointF ctrlB = _points[i]->localePosition() - a * 0.168 * QPointF(diffX, diffY) / MathUtility::dist(QPointF(diffX, diffY));
        if (_points[i]->inherits("Point")) {
            ((Point*) _points[i])->setLocaleCtrlA(ctrlA);
            ((Point*) _points[i])->setLocaleCtrlB(ctrlB);
        } else if (_points[i]->hasTag("PointTag")) {
            ((PointTag*) _points[i]->tag("PointTag"))->setLocaleCtrlA(ctrlA);
            ((PointTag*) _points[i]->tag("PointTag"))->setLocaleCtrlB(ctrlB);
        } else {
            Q_ASSERT(false);
        }
    }
    if (((BoolProperty*) properties()["closed"])->value()) {
    }
}

void Spline::connectPropertyTriggers()
{
    connect(((BoolProperty*) properties()["smooth"]), &BoolProperty::valueChanged, [=]() {
        blockSignals(true);
        smoothTangents();
        blockSignals(false);
        emitObjectChanged();
    });
}
