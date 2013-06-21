#include "splineproperty.h"
#include "FrontEnd/EditorWidgets/splineedit.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QDebug>

REGISTER_DEFN_PROPERTYTYPE(SplineProperty);

SplineProperty::SplineProperty(QByteArray* data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString name, className, category;
    stream >> className >> category >> name >> _points >> _coef;

    Q_ASSERT(className == type());
    setCategory(category);
    setName(name);
}

SplineProperty::SplineProperty(QString category, QString name, Initialization init)
{
    setName(name);
    setCategory(category);
    switch (init) {
    case Up:
        addPoint(QPointF(0,0));
        addPoint(QPointF(1,1));
        break;
    case Down:
        addPoint(QPointF(0,1));
        addPoint(QPointF(1,0));
        break;
    case Null:
        addPoint(QPointF(0,0));
        addPoint(QPointF(1,0));
        break;
    case One:
        addPoint(QPointF(0,1));
        addPoint(QPointF(1,1));
        break;
    case None:
        ;
    }

}

QByteArray SplineProperty::toByteArray()
{
    QByteArray array;
    QDataStream stream(&array, QIODevice::WriteOnly);

    stream << type() << category() << name() << _points << _coef;

    return array;
}

qreal SplineProperty::getValue(qreal x)
{
    // a0 + a1(x-x0) + a2(x-x0)(x-x1) + ...

    qreal fac = 1;
    qreal sum = 0;

    for (int i = 0; i < _coef.size(); i++) {
        sum += _coef[i] * fac;
        fac *= (x-_points[i].x());
    }
    return sum;
}

void SplineProperty::calcCoef()
{
    _coef.clear();
    for (int i = 0; i < _points.size(); i++) {
        _coef.append(_points[i].y());
    }

    for (int i = 1; i < _points.size(); i++) {
        for (int j = _points.size() - 1; j >= i; j--) {
            _coef[j] = (_coef[j-1] - _coef[j]) / (_points[j-i].x() - _points[j].x());
        }
    }
}

void SplineProperty::addPoint(QPointF p)
{
    bool overwritten = false;
    for (QPointF point : _points) {
        if (qFuzzyCompare(p.x(), point.x())) {
            point.setX(p.x());
            point.setY(p.y());
            overwritten = true;
        }
    }
    if (!overwritten)
        _points.append(p);
    update();
    emit valueChanged();
}

void SplineProperty::removePoint(int index)
{
    _points.removeAt(index);
    update();
    emit valueChanged();
}

void SplineProperty::update()
{
    calcCoef();
}

void SplineProperty::setPoint(int index, QPointF p)
{
    p = QPointF(p.x() < 0 ? 0 : p.x() > 1 ? 1 : p.x(), p.y() < 0 ? 0 : p.y() > 1 ? 1 : p.y());
    _points[index] = p;
    update();
}

QWidget* SplineProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QString name = props.first()->name();
    SplineEdit* edit = new SplineEdit(((SplineProperty*) props.first()), parent);

    auto updateSplineEdit = [=]() {
        bool multipleValues = false;
        quint64 hash = ((SplineProperty*) props.first())->hash();

        foreach (Property* p, props) {
            SplineProperty* propSpline = (SplineProperty*) p;
            if (propSpline->hash() != hash) {
                multipleValues = true;
            }
        }

        if (multipleValues) {
            edit->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            edit->setStyleSheet("");
        }

    };

    foreach (Property* p, props) {
        SplineProperty* propSpline = (SplineProperty*) p;
        connect(edit, &SplineEdit::pointsChanged, [=]() {
            propSpline->setPoints(edit->data()->points());
        });

        connect(propSpline, &SplineProperty::valueChanged, updateSplineEdit);
    }

    updateSplineEdit();

    if (name.isEmpty()) {
        return edit;
    } else {
        QWidget* w = new QWidget(parent);
        edit->setParent(w);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(edit);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}

quint64 SplineProperty::hash() const
{
    quint64 hash = 0;
    for (int i = 0; i < _points.size(); i++) {
        int x = _points[i].x() * 100000.0;
        int y = _points[i].y() * 100000.0;
        hash += (i+1) * (x + 17*y);
    }
    return hash;
}

void SplineProperty::setPoints(QList<QPointF> points)
{
    _points = points;
    update();
    emit valueChanged();
}
