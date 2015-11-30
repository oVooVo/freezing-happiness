#include "mathutility.h"
#include <QDebug>
#include <QColor>

#ifdef EXPRTK
#include <exprtk.hpp>
#endif

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
    return qAtan2(t.m21(), t.m11());
}

qreal MathUtility::scalation(QTransform t)
{
    return dist(QPointF(t.m11(), t.m21()));

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
    qreal t11 = x.m11() * y.m11() + x.m12() * y.m21();
    qreal t12 = x.m11() * y.m12() + x.m12() * y.m22();

    qreal t21 = x.m21() * y.m11() + x.m22() * y.m21();
    qreal t22 = x.m21() * y.m12() + x.m22() * y.m22();

    qreal t31 = x.m31() * y.m11() + x.m32() * y.m21() + y.m31();
    qreal t32 = x.m31() * y.m12() + x.m32() * y.m22() + y.m32();

    return QTransform(t11, t12, 0, t21, t22, 0, t31, t32, 1);
}

QString MathUtility::colorToString(QColor color)
{
    return QString("rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue());
}

#ifdef EXPRTK
QList<qreal> MathUtility::parse(QString s, QList<qreal> xs)
{
    std::string expression_string = s.toStdString();
    qreal x;
    exprtk::symbol_table<qreal> symbol_table;
    symbol_table.add_variable("x",x);
    symbol_table.add_constants();

    exprtk::expression<qreal> expression;
    expression.register_symbol_table(symbol_table);

    exprtk::parser<qreal> parser;
    parser.compile(expression_string,expression);

    QList<qreal> results;
    foreach (x , xs)
    {
       results.append(expression.value());
    }
    return results;
}
#endif






