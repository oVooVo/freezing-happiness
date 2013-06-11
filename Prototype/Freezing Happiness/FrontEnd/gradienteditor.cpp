#include "gradienteditor.h"
#include <QFormLayout>

GradientEditor::GradientEditor()
{
    _gradientEdit = new GradientEdit(this);
    _positionSpinBox = new QDoubleSpinBox(this);
    _positionSpinBox->setRange(0,1);
    _positionSpinBox->setSingleStep(0.01);
    _angleSpinBox = new QDoubleSpinBox(this);
    _angleSpinBox->setRange(0,360);
    _modeComboBox = new QComboBox(this);
    _modeComboBox->addItems(QStringList() << tr("Linear") << tr("Radial") << tr("Conical"));

    QFormLayout* settingsLayout = new QFormLayout();
    settingsLayout->addRow(tr("Position:"), _positionSpinBox);
    settingsLayout->addRow(tr("Angle:"), _angleSpinBox);
    settingsLayout->addRow(tr("Type:"), _modeComboBox);

    QHBoxLayout* base = new QHBoxLayout();
    base->addWidget(_gradientEdit);
    base->addLayout(settingsLayout);
    setLayout(base);

    void (QDoubleSpinBox:: *valueChangedDouble)(double) = &QDoubleSpinBox::valueChanged;

    connect(_positionSpinBox, valueChangedDouble, [this]() {_gradientEdit->setCurrentPosition(_positionSpinBox->value());});
    connect(_gradientEdit, &GradientEdit::positionChanged, [this](double r) {_positionSpinBox->setValue(r);});
}

QGradient* GradientEditor::gradient()
{
    QGradient* g;
    switch (_modeComboBox->currentIndex()) {
    case 0:
        g = new QLinearGradient();
        break;
    case 1:
        g = new QRadialGradient();
        break;
    case 2:
        g = new QConicalGradient();
        break;
    default:
        return 0;
    }

    for (QPair<QColor, qreal> p : _gradientEdit->gradient()) {
        g->setColorAt(p.second, p.first);
    }

    return g;
}
