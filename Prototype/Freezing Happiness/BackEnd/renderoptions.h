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
    QSize resolution() const { return _resolution; }
    QString filePath() const { return _filePath; }
    void showConfigurator();
    bool saveFile() const { return _saveFile; }

private:
    QSize _resolution = QSize(800, 600);

    QString _filePath = QDir::homePath().append(QDir::separator()).append(QObject::tr("untitled.png"));
    bool _saveFile;


    static const QStringList dontSaveButtonCaption;
    static const QStringList useCameraButtonCaption;

    QWidget* _configurator;
    void setUpConfigurator();

};

#endif // RENDEROPTIONS_H
