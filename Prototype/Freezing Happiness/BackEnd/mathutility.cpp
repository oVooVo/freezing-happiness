#include "mathutility.h"
#include <QDebug>

MathUtility::MathUtility()
{
}

qreal MathUtility::dist(qreal x, qreal y)
{
    return qAbs(x-y);
}

qreal MathUtility::dist(QPointF x, QPointF y)
{
    return qSqrt((x.x()-y.x())*(x.x()-y.x())+(x.y()-y.y())*(x.y()-y.y()));
}


QPointF MathUtility::translation(QTransform t)
{
    return QPointF(t.m31(), t.m32());
}

qreal MathUtility::rotation(QTransform t)
{
    qreal s = scalation(t);
    QPointF rotation = QPointF(qAtan2(t.m21()/s, t.m11()/s), -qAtan2(t.m12()/s, t.m22()/s));

    if (!qFuzzyCompare(rotation.x(), rotation.y())) {
        qDebug() << "non-orthogonal axes";
        //Q_ASSERT(false);
    }

    return rotation.x();
}

qreal MathUtility::scalation(QTransform t)
{
    QPointF scalation = QPointF(dist(QPointF(t.m11(), t.m21())), dist(QPointF(t.m12(), t.m22())));
    if (!qFuzzyCompare(scalation.x(), scalation.y())) {
        qDebug() << "non-equal scalations";
        //Q_ASSERT(false);
    }
    return scalation.x();

}

bool MathUtility::isOrthogonal(QTransform t)
{
    return qFuzzyIsNull(t.m11() * t.m12() + t.m21() * t.m22());
}


qreal MathUtility::aCos(qreal t)
{
    if (t <= -1) {
        return qAcos(-1);
    } else if (t >= 1) {
        return qAcos(1);
    } else {
        return qAcos(t);
    }
}

qreal MathUtility::aSin(qreal t)
{
    if (t <= -1) {
        return qAsin(-1);
    } else if (t >= 1) {
        return qAsin(1);
    } else {
        return qAsin(t);
    }
}

QTransform MathUtility::mult(QTransform x, QTransform y)
{
    Q_ASSERT_X(false, "MathUtility::mult", "use QTransform::operator* instead!");

    qreal a, b, c,
          d, e, f,
          g, h, i;

    a = x.m11()*y.m11() + x.m12()*y.m21() + x.m13()*y.m31();
    b = x.m11()*y.m12() + x.m12()*y.m22() + x.m13()*y.m32();
    c = x.m11()*y.m13() + x.m12()*y.m23() + x.m13()*y.m33();

    d = x.m21()*y.m11() + x.m22()*y.m21() + x.m23()*y.m31();
    e = x.m21()*y.m12() + x.m22()*y.m22() + x.m23()*y.m32();
    f = x.m21()*y.m13() + x.m22()*y.m23() + x.m23()*y.m33();

    g = x.m31()*y.m11() + x.m32()*y.m21() + x.m33()*y.m31();
    h = x.m31()*y.m12() + x.m32()*y.m22() + x.m33()*y.m32();
    i = x.m31()*y.m13() + x.m32()*y.m23() + x.m33()*y.m33();


    return QTransform(a, b, c, d, e, f, g, h, i);
}











