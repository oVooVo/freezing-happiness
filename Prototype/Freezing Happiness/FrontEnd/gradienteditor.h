#ifndef GRADIENTEDITOR_H
#define GRADIENTEDITOR_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QSpinBox>
#include <QLabel>
#include "EditorWidgets/gradientedit.h"
#include <QGradient>
#include <QPushButton>


class GradientEditor : public QWidget
{
public:
    GradientEditor();
    QGradient* gradient();

private:
    GradientEdit* _gradientEdit;
    QDoubleSpinBox* _positionSpinBox;
    QDoubleSpinBox* _angleSpinBox;
    QComboBox* _modeComboBox;

};

#endif // GRADIENTEDITOR_H
