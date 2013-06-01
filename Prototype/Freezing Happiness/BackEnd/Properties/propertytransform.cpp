#include <QDoubleSpinBox>
#include "propertytransform.h"
#include <QDebug>
#include <BackEnd/mathutility.h>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <BackEnd/project.h>

#define MAX_RANGE -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
#define POS_RANGE 0.001, std::numeric_limits<qreal>::max()

REGISTER_DEFN_PROPERTYTYPE(PropertyTransform);

PropertyTransform::PropertyTransform(QString category, QString name, QTransform t)
{
    _transform = t;
    setCategory(category);
    setName(name);
}

PropertyTransform::PropertyTransform(QByteArray *data)
{
    QString className, category, name;
    QDataStream in(data, QIODevice::ReadOnly);
    qreal a11, a12, a13, a21, a22, a23, a31, a32, a33;
    in >> className >> category >> name
       >> a11 >> a12 >> a13
       >> a21 >> a22 >> a23
       >> a31 >> a32 >> a33;
    _transform = QTransform(a11, a12, a13,
                            a21, a22, a23,
                            a31, a32, a33);
    setCategory(category);
    setName(name);
    Q_ASSERT(className == type());
}

QByteArray PropertyTransform::toByteArray()
{
    QByteArray array;
    QDataStream out(&array, QIODevice::WriteOnly);
    out << type() << category() << name();
    out << _transform.m11() << _transform.m12() << _transform.m13()
        << _transform.m21() << _transform.m22() << _transform.m23()
        << _transform.m31() << _transform.m32() << _transform.m33();
    return array;
}

void PropertyTransform::setTransform(QTransform t)
{
    qreal x = position().x();
    qreal y = position().y();
    qreal s = scalation();
    qreal r = rotation();

    if (t == _transform) {
        return;
    }

    project()->createNewUndoRecord();

    _transform = t;

    if (x != position().x()) emit xPositionChanged();
    if (y != position().y()) emit yPositionChanged();
    if (r != rotation()) emit rotationChanged();
    if (s != scalation()) emit scalationChanged();
    emit valueChanged();
}


QString PropertyTransform::toString() const
{
    return QString("PT[%1,%2,%3|%4,%5,%6|%7,%8,%9]")
            .arg(_transform.m11()).arg(_transform.m12()).arg(_transform.m13())
            .arg(_transform.m21()).arg(_transform.m22()).arg(_transform.m23())
            .arg(_transform.m31()).arg(_transform.m32()).arg(_transform.m33());
}


QTransform PropertyTransform::transform() const
{
    return _transform;
}


void PropertyTransform::setPosition(QPointF pos)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), pos.x(), pos.y(), t.m33());
    setTransform(t);
}

void PropertyTransform::setX(qreal x)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), x, t.m32(), t.m33());
    setTransform(t);
}

void PropertyTransform::setY(qreal y)
{
    QTransform t = transform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), t.m31(), y, t.m33());
    setTransform(t);
}

void PropertyTransform::setScalation(qreal scale)
{
    QTransform t = transform();
    qreal s = scale / scalation();
    t.setMatrix(t.m11()*s, t.m12()*s, t.m13(),
                t.m21()*s, t.m22()*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setTransform(t);
}

void PropertyTransform::setRotation(qreal r)
{
    QTransform t = transform();
    qreal s = scalation();
    t.setMatrix(qCos(r)*s, -qSin(r)*s, t.m13(),
                qSin(r)*s,  qCos(r)*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setTransform(t);
}

QPointF PropertyTransform::position() const
{
    return MathUtility::translation(transform());
}

qreal PropertyTransform::rotation() const
{
    return MathUtility::rotation(transform());
}

qreal PropertyTransform::scalation() const
{
    return MathUtility::scalation(transform());
}

QWidget* PropertyTransform::createWidget(QList<Property *> props, QWidget *parent)
{
    QWidget* w = new QWidget(parent);
    QList<QDoubleSpinBox*> spinBoxes;
    for (int i = 0; i < 4; i++) {
        spinBoxes.append(new QDoubleSpinBox(w));
        if (i == 0 || i == 1 || i == 2) {
            spinBoxes.last()->setRange(MAX_RANGE);
        } else {
            spinBoxes.last()->setRange(POS_RANGE);
        }
    }
    spinBoxes[2]->setSuffix(" °");

    QLabel* xl = new QLabel(tr("x:"), w);
    QLabel* yl = new QLabel(tr("y:"), w);
    QLabel* rl = new QLabel(tr("Rotation:"), w);
    QLabel* sl = new QLabel(tr("Scalation:"), w);
    QLabel* header = new QLabel(props.first()->name(), w);

    QHBoxLayout* xh = new QHBoxLayout();
    xh->addWidget(xl); xh->addWidget(spinBoxes[0]);
    QHBoxLayout* yh = new QHBoxLayout();
    yh->addWidget(yl); yh->addWidget(spinBoxes[1]);
    QHBoxLayout* rh = new QHBoxLayout();
    rh->addWidget(rl); rh->addWidget(spinBoxes[2]);
    QHBoxLayout* sh = new QHBoxLayout();
    sh->addWidget(sl); sh->addWidget(spinBoxes[3]);

    QGridLayout* g = new QGridLayout();
    g->addLayout(xh, 0, 0);
    g->addLayout(yh, 1, 0);
    g->addLayout(sh, 0, 1);
    g->addLayout(rh, 1, 1);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(header);
    layout->addLayout(g);

    w->setLayout(layout);



    auto updateQDSB = [spinBoxes, props](int i) {
        qreal value = ((PropertyTransform*) props[0])->values()[i];
        bool consistent = true;
        foreach (Property* property, props) {
            PropertyTransform* propTrans = ((PropertyTransform*) property);
            if (!qFuzzyCompare(value, propTrans->values()[i])) {
                consistent = false;
            }
        }
        if (!consistent) {
            spinBoxes[i]->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            spinBoxes[i]->setStyleSheet("");
        }
        if (i == 2) {
            spinBoxes[i]->setValue(value * 180 * M_1_PI);
        } else {
            spinBoxes[i]->setValue(value);
        }
    };

    auto updateX = [updateQDSB]() { updateQDSB(0); };
    auto updateY = [updateQDSB]() { updateQDSB(1); };
    auto updateR = [updateQDSB]() { updateQDSB(2); };
    auto updateS = [updateQDSB]() { updateQDSB(3); };

    foreach (Property* property, props) {
        PropertyTransform* propTrans = ((PropertyTransform*) property);
        connect(spinBoxes[0], &QDoubleSpinBox::editingFinished, [=]() { propTrans->setX(spinBoxes[0]->value());} );
        connect(spinBoxes[1], &QDoubleSpinBox::editingFinished, [=]() { propTrans->setY(spinBoxes[1]->value());} );
        connect(spinBoxes[2], &QDoubleSpinBox::editingFinished, [=]() { propTrans->setRotation(spinBoxes[2]->value() * M_PI /180.0);} );
        connect(spinBoxes[3], &QDoubleSpinBox::editingFinished, [=]() { propTrans->setScalation(spinBoxes[3]->value() > 0.001 ? spinBoxes[3]->value() : 0.001);} );
        connect(propTrans, &PropertyTransform::xPositionChanged, updateX );
        connect(propTrans, &PropertyTransform::yPositionChanged, updateY );
        connect(propTrans, &PropertyTransform::rotationChanged, updateR );
        connect(propTrans, &PropertyTransform::scalationChanged, updateS );

    }
    for (int i = 0; i < 4; i++) {
        updateQDSB(i);
    }
    return w;

}

QList<qreal> PropertyTransform::values() const
{
    QList<qreal> list;
    list << position().x();
    list << position().y();
    list << rotation();
    list << scalation();
    return list;
}
