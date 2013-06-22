#ifndef CLONER_H
#define CLONER_H

#include "object.h"

class Cloner : public Object
{
    Q_OBJECT
public:
    Cloner(Project *project, QString name = "Cloner");
    void customDraw(QPainter &p);
    bool valid() const;
    QList<QPointF> matrices();
    Object* convert();

protected:
    bool drawChildren() const { return false; }
    void emitObjectChanged();
    void connectPropertyTriggers();

private:
    REGISTER_DECL_OBJECTTYPE(Cloner);
    static const QStringList MODES;
    static const QStringList LINEAR_PROPERTIES;
    static const QStringList CIRCLE_PROPERTIES;
    static const QStringList SPLINE_PROPERTIES;
    void alignLinear();
    void alignCircle();
    void alignSpline();
    void updatePropertiesVisibility();
    void updateMatrices();
    bool _dirtyMatrices = true;
    QList<QMatrix> _matrices;
};

#endif // CLONER_H
