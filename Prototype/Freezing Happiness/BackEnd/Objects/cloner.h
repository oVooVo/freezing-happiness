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

protected:
    bool drawChildren() const { return false; }
    void emitObjectChanged();

private:
    REGISTER_DECL_OBJECTTYPE(Cloner);
    static const QStringList MODES;
    static const QStringList LINEAR_PROPERTIES;
    static const QStringList CIRCLE_PROPERTIES;
    void alignLinear(QPainter &p) const;
    void alignCircle(QPainter &p) const;
    void updatePropertiesVisibility();
};

#endif // CLONER_H
