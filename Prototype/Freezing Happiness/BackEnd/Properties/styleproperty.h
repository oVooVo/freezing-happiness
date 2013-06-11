#ifndef STYLEPROPERTY_H
#define STYLEPROPERTY_H

#include "property.h"

class StyleProperty : public Property
{
    Q_OBJECT
public:
    static const QStringList PEN_STYLES;
    static const QStringList BRUSH_STYLES;

    StyleProperty(QByteArray* data);
    StyleProperty(QString category, QString name);
    QByteArray toByteArray();
    QString toString() const;
    static QWidget* createWidget(QList<Property *> props, QWidget *parent);

    Qt::PenStyle penStyle() const { return _penStyle; }
    Qt::BrushStyle brushStyle() const { return _brushStyle; }
    qreal width() const { return _width; }
    QColor fillColor() const { return _fillColor; }
    QColor drawColor() const { return _drawColor; }

    void setPenStyle(Qt::PenStyle penStyle);
    void setBrushStyle(Qt::BrushStyle brushStyle);
    void setWidth(qreal width);
    void setFillColor(QColor c);
    void setDrawColor(QColor c);

signals:
    void penStyleChanged(Qt::PenStyle);
    void brushStyleChanged(Qt::BrushStyle);
    void widthChanged(qreal);
    void fillColorChanged(QColor);
    void drawColorChanged(QColor);


private:
    QColor _drawColor;
    QColor _fillColor;
    qreal _width;
    Qt::PenStyle _penStyle;
    Qt::BrushStyle _brushStyle;
    REGISTER_DECL_PROPERTYTYPE(StyleProperty);

};

#endif // STYLEPROPERTY_H
