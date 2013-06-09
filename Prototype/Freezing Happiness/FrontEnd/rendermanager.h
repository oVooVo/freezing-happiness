#ifndef RENDERMANAGER_H
#define RENDERMANAGER_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QLabel>
#include <QFile>

namespace Ui {
class RenderManager;
}

class RenderManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit RenderManager(QWidget *parent = 0);
    void addImage(QImage image, bool save = false, QString filepath = QString());

    ~RenderManager();
private slots:
    void removeImage();
    void saveImage(bool saveAs = false);
    void saveImageAs() { saveImage(true); }
    void selectionChanged(QListWidgetItem*);

private:
    Ui::RenderManager *ui;
    QMap<QListWidgetItem*, QImage> _images;
    QMap<QListWidgetItem*, QString> _files;
    QLabel* _label = 0;
};

#endif // RENDERMANAGER_H
