#ifndef STYLETAG_H
#define STYLETAG_H

#include "tag.h"
#include <QPainter>

class StyleTag : public Tag
{
    Q_OBJECT
public:

    static const QStringList PEN_STYLES;
    static const QStringList BRUSH_STYLES;

    StyleTag(Object* owner = 0, QByteArray *data = 0);
    QByteArray toByteArray();
    QString toString() const;
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);

    Qt::PenStyle penStyle() const { return _penStyle; }
    Qt::BrushStyle brushStyle() const { return _brushStyle; }
    qreal width() const { return _width; }
    QColor fillColor() const { return _fillColor; }
    QColor drawColor() const { return _drawColor; }
    bool isGlobal() const { return _globale; }
    qreal xOffset() const { return _xOff; }
    qreal yOffset() const { return _yOff; }
    qreal rotation() const { return _rotation; }
    qreal scalation() const { return _scale; }
    QMatrix transform() const;

    void setPenStyle(Qt::PenStyle penStyle);
    void setBrushStyle(Qt::BrushStyle brushStyle);
    void setWidth(qreal width);
    void setFillColor(QColor c);
    void setDrawColor(QColor c);
    void setXOffset(qreal x);
    void setYOffset(qreal y);
    void setRotation(qreal r);
    void setScalation(qreal s);
    void setIsGlobal(bool g);

signals:
    void penStyleChanged(Qt::PenStyle);
    void brushStyleChanged(Qt::BrushStyle);
    void widthChanged(qreal);
    void fillColorChanged(QColor);
    void drawColorChanged(QColor);
    void xOffsetChanged(qreal);
    void yOffsetChanged(qreal);
    void rotationChanged(qreal);
    void scalationChanged(qreal);
    void isGlobaleChanged(bool);



private:
    QColor _drawColor = Qt::black;
    QColor _fillColor = Qt::white;
    qreal _width = 1;
    Qt::PenStyle _penStyle = Qt::SolidLine;
    Qt::BrushStyle _brushStyle = Qt::NoBrush;
    qreal _xOff = 0;
    qreal _yOff = 0;
    qreal _scale = 1;
    qreal _rotation = 0;
    bool _globale = true;
    qreal matrix(int i);

    REGISTER_DECL_TAGTYPE(StyleTag);
};

#endif // STYLETAG_H
