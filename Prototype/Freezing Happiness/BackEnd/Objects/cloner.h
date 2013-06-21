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

protected:
    bool drawChildren() const { return false; }
    void emitObjectChanged();
    void connectVisibilityTriggers();

private:
    REGISTER_DECL_OBJECTTYPE(Cloner);
    static const QStringList MODES;
    static const QStringList LINEAR_PROPERTIES;
    static const QStringList CIRCLE_PROPERTIES;
    void alignLinear();
    void alignCircle();
    void updatePropertiesVisibility();
    void updateMatrices();
    bool _dirtyMatrices = true;
    QList<QMatrix> _matrices;
};

#endif // CLONER_H
