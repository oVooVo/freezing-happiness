#ifndef MATHUTILITY_H
#define MATHUTILITY_H

#include <QTransform>
#include "qmath.h"

class MathUtility
{
public:
    static qreal dist(qreal x, qreal y);
    static qreal dist(QPointF x, QPointF y = QPointF(0,0));
    static QPointF translation(QTransform t);
    static qreal rotation(QTransform t);
    static qreal scalation(QTransform t);
    static bool isOrthogonal(QTransform t);
    static qreal aCos(qreal t);
    static qreal aSin(qreal t);
    static QTransform mult(QTransform x, QTransform y);

private:
    MathUtility();
};

#endif // MATHUTILITY_H
