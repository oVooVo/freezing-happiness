#include "navigationtoolbar.h"
#include "ui_navigationtoolbar.h"
#include <QDebug>

NavigationToolbar::NavigationToolbar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NavigationToolbar)
{
    ui->setupUi(this);
    _viewport = 0;
    _positionEdit = new ObjectEdit(this);
    _rotationEdit = new ObjectEdit(this);
    _positionEdit->setEnabled(false);
    _rotationEdit->setEnabled(false);

    ((QGridLayout*) layout())->addWidget(_positionEdit, 1, 4);
    ((QGridLayout*) layout())->addWidget(_rotationEdit, 1, 5);
    connect(ui->modeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onModeComboBoxIndexChanged(int)));
    connect(ui->positionComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onPositionComboBoxIndexChanged(int)));
    connect(ui->orientationComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onOrientationComboBoxIndexChanged(int)));
    connect(ui->particularButton, SIGNAL(toggled(bool)), this, SLOT(onParticularButtonCheckedChanged(bool)));
    connect(_positionEdit, SIGNAL(statusChanged()), this, SLOT(onCustomPositionEditChanged()));
    connect(_rotationEdit, SIGNAL(statusChanged()), this, SLOT(onCustomRotationEditChanged()));
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

}

NavigationToolbar::~NavigationToolbar()
{
    delete ui;
}

void NavigationToolbar::setViewport(Viewport* viewport)
{
    _viewport = viewport;
}

void NavigationToolbar::setProject(Project *project)
{
    _project = project;
    _positionEdit->setProject(project);
    _rotationEdit->setProject(project);
}

void NavigationToolbar::updateComboBoxes()
{
    switch (_viewport->mode()) {
    case Object::ObjectMode:
        ui->modeComboBox->setCurrentIndex(0); break;
    case Object::AxesMode:
        ui->modeComboBox->setCurrentIndex(1); break;
    default: Q_ASSERT(false);
    }

    switch (_viewport->positionMode()) {
    case PSRGrabber::ObjectPosition:
        ui->positionComboBox->setCurrentIndex(0); break;
    case PSRGrabber::ParentPosition:
        ui->positionComboBox->setCurrentIndex(1); break;
    case PSRGrabber::CustomPosition:
        ui->positionComboBox->setCurrentIndex(2); break;
    case PSRGrabber::RootPosition:
        ui->positionComboBox->setCurrentIndex(3); break;
    default: Q_ASSERT(false);
    }

    switch (_viewport->rotationMode()) {
    case PSRGrabber::ObjectRotation:
        ui->orientationComboBox->setCurrentIndex(0); break;
    case PSRGrabber::ParentRotation:
        ui->orientationComboBox->setCurrentIndex(1); break;
    case PSRGrabber::CustomRotation:
        ui->orientationComboBox->setCurrentIndex(2); break;
    case PSRGrabber::RootRotation:
        ui->orientationComboBox->setCurrentIndex(3); break;
    default: Q_ASSERT(false);
    }

    ui->particularButton->setChecked(_viewport->particularMode());
}

void NavigationToolbar::onModeComboBoxIndexChanged(int index)
{
    if (!_viewport) return;
    if (index == 0) {
        _viewport->setMode(Object::ObjectMode);
    } else if (index == 1) {
        _viewport->setMode(Object::AxesMode);
    } else {
        Q_ASSERT(false);
    }
}

void NavigationToolbar::onPositionComboBoxIndexChanged(int index)
{
    if (!_viewport) return;
    if (index == 0) {
        _positionEdit->setEnabled(false);
        _viewport->setPositionMode(PSRGrabber::ObjectPosition);
    } else if (index == 1) {
        _positionEdit->setEnabled(false);
        _viewport->setPositionMode(PSRGrabber::ParentPosition);
    } else if (index == 2) {
        _positionEdit->setEnabled(true);
        _viewport->setPositionMode(PSRGrabber::CustomPosition);
    } else if (index == 3) {
        _positionEdit->setEnabled(false);
        _viewport->setPositionMode(PSRGrabber::RootPosition);
    } else {
        Q_ASSERT(false);
    }
}

void NavigationToolbar::onOrientationComboBoxIndexChanged(int index)
{
    if (!_viewport) return;
    if (index == 0) {
        _rotationEdit->setEnabled(false);
        _viewport->setRotationMode(PSRGrabber::ObjectRotation);
    } else if (index == 1) {
        _rotationEdit->setEnabled(false);
        _viewport->setRotationMode(PSRGrabber::ParentRotation);
    } else if (index == 2) {
        _rotationEdit->setEnabled(true);
        _viewport->setRotationMode(PSRGrabber::CustomRotation);
    } else if (index == 3) {
        _rotationEdit->setEnabled(false);
        _viewport->setRotationMode(PSRGrabber::RootRotation);
    } else {
        Q_ASSERT(false);
    }
}

void NavigationToolbar::onParticularButtonCheckedChanged(bool checked)
{
    _viewport->setParticularMode(checked);
}

void NavigationToolbar::onCustomPositionEditChanged()
{
    if (_positionEdit->isEmpty()) {
        _viewport->setCustomPositionObject(0);
    } else {
        _viewport->setCustomPositionObject(_project->getObject(_positionEdit->id()));
    }
}

void NavigationToolbar::onCustomRotationEditChanged()
{
    if (_rotationEdit->isEmpty()) {
        _viewport->setCustomRotationObject(0);
    } else {
        _viewport->setCustomRotationObject(_project->getObject(_rotationEdit->id()));
    }
}

