#ifndef RENDEROPTIONS_H
#define RENDEROPTIONS_H

#include <QSize>
#include <QDir>
#include <QObject>
#include <QColor>
#include <QRect>

class RenderOptions
{
public:
    RenderOptions();
    ~RenderOptions();

public:
    QWidget* configurator();
    QSize size() const { return _size; }
    QString filePath() const { return _filePath; }
    QColor backgroundColor() const { return _backgroundColor; }
    QPointF offset() const { return _offset; }
    qreal scale() const { return _scale; }

private:
    QSize _size = QSize(800, 600);
    QString _filePath = QDir::homePath().append(QDir::separator()).append(QObject::tr("untitled.png"));
    QColor _backgroundColor = QColor(Qt::white);
    QPointF _offset = QPointF();
    qreal _scale = 1;

    QWidget* _configurator;
    void createConfigurator();


};

#endif // RENDEROPTIONS_H
