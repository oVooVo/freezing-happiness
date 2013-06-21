#include <QFileDialog>
#include "rendermanager.h"
#include "ui_rendermanager.h"
#include <QFileInfo>
#include <QPainter>

RenderManager::RenderManager(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::RenderManager)
{
    ui->setupUi(this);
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(saveImage()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(saveImageAs()));
    connect(ui->actionDelete, SIGNAL(triggered()), this, SLOT(removeImage()));
    connect(ui->listWidget, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(selectionChanged(QListWidgetItem*)));
}

RenderManager::~RenderManager()
{
    delete ui;
}

void RenderManager::addImage(QImage image, bool save, QString filepath)
{
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    QString file = filepath.isEmpty() ? QString() : filepath;

    item->setText(file.isEmpty() ? tr("untitled") : QFileInfo(file).fileName());
    _files.insert(item, file);
    _images.insert(item, image);
    item->setSelected(true);
    if (!_label) _label = new QLabel(this);
    _label->clear();
    _label->setPixmap(QPixmap::fromImage(image));
    ui->scrollArea->setWidget(_label);
    if (save) {
        item->setSelected(true);
        saveImage();
    }
}

void RenderManager::selectionChanged(QListWidgetItem* item)
{
    if (!item) return;
    _label->clear();

    _label->setPixmap(QPixmap::fromImage(_images[item]));
}

void RenderManager::removeImage()
{
    QListWidgetItem* item = 0;
    if (ui->listWidget->selectedItems().isEmpty()) {
        return;
    } else {
        item = ui->listWidget->selectedItems().first();
    }
    _files.remove(item);
    _images.remove(item);
    ui->listWidget->removeItemWidget(item);
    delete item;
}

void RenderManager::saveImage(bool saveAs)
{
    QListWidgetItem* item = 0;
    if (ui->listWidget->selectedItems().isEmpty()) {
        return;
    } else {
        item = ui->listWidget->selectedItems().first();
    }
    QString filepath = _files.value(item, QString());
    if (filepath.isEmpty() || saveAs) {
        filepath = QFileDialog::getSaveFileName(this, "Save Image", QDir::homePath());
    }
    if (filepath.isEmpty()) return;

    _images.value(item).save(filepath);
    _files.insert(item, filepath);
    item->setText(QFileInfo(filepath).fileName());
}
