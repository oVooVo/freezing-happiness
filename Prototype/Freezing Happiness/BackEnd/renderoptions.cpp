#include <QColorDialog>
#include "renderoptions.h"
#include <QDebug>
#include <QWidget>
#include <QFormLayout>
#include <QObject>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <qmath.h>
#include <QFileDialog>
#include <BackEnd/mathutility.h>

const QStringList RenderOptions::dontSaveButtonCaption
    = QStringList() << QObject::tr("don't save") << QObject::tr ("save");
const QStringList RenderOptions::useCameraButtonCaption
    = QStringList() << QObject::tr ("Ignore cameras") << QObject::tr("Use camera");

RenderOptions::RenderOptions()
{
    _configurator = 0;
}

RenderOptions::~RenderOptions()
{
    if (_configurator)
        delete _configurator;
}

void RenderOptions::showConfigurator()
{
    if (!_configurator) {
        setUpConfigurator();
    }
    _configurator->show();
}

void RenderOptions::setUpConfigurator()
{
    auto tr = [](const char *s, const char *c = 0, int n = -1) { return QObject::tr(s, c, n); };

    _configurator = new QWidget();
    QFormLayout* formLayout = new QFormLayout();


    QHBoxLayout* fileLayout = new QHBoxLayout();
    QLineEdit* filePathEdit = new QLineEdit(filePath(), _configurator);
    QPushButton* browseButton = new QPushButton("...", _configurator);
    fileLayout->addWidget(filePathEdit);
    fileLayout->addWidget(browseButton);
    QPushButton* dontSaveButton = new QPushButton(dontSaveButtonCaption[1], _configurator);
    dontSaveButton->setCheckable(true);
    dontSaveButton->setChecked(true);
    formLayout->addRow(dontSaveButton);
    formLayout->addRow(tr("File:"), fileLayout);

    //== Resolution ==
    QHBoxLayout* resolutionLayout = new QHBoxLayout();
    QSpinBox* xResolutionSpinBox = new QSpinBox(_configurator);
    xResolutionSpinBox->setRange(0,300000);
    xResolutionSpinBox->setValue(resolution().width());
    QSpinBox* yResolutionSpinBox = new QSpinBox(_configurator);
    yResolutionSpinBox->setRange(0,300000);
    yResolutionSpinBox->setValue(resolution().height());
    resolutionLayout->addWidget(xResolutionSpinBox);
    resolutionLayout->addWidget(yResolutionSpinBox);
    formLayout->addRow(tr("Resolution:"), resolutionLayout);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(formLayout);

    _configurator->setLayout(layout);

    QObject::connect(filePathEdit, &QLineEdit::editingFinished, [=](){
       _filePath = filePathEdit->text();
    });
    QObject::connect(browseButton, &QPushButton::clicked, [=]() {
        QString newFilePath = QFileDialog::getSaveFileName(_configurator, tr("Save image"), _filePath);
        if (!newFilePath.isEmpty()) {
            _filePath = newFilePath;
        }
    });
    QObject::connect(dontSaveButton, &QPushButton::clicked, [=](bool checked) {
        if (checked) {
            dontSaveButton->setText(dontSaveButtonCaption[1]);
            _saveFile = true;
        } else {
            dontSaveButton->setText(dontSaveButtonCaption[0]);
            _saveFile = false;
        }
        filePathEdit->setEnabled(_saveFile);
        browseButton->setEnabled(_saveFile);
    });
    QObject::connect(xResolutionSpinBox, &QSpinBox::editingFinished, [=]() {
        _resolution.setWidth(xResolutionSpinBox->value());
    });
    QObject::connect(yResolutionSpinBox, &QSpinBox::editingFinished, [=]() {
        _resolution.setHeight(yResolutionSpinBox->value());
    });
}
