#include "styleproperty.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QLabel>
#include <QHBoxLayout>
#include <QCheckBox>
#include <qmath.h>

#define MAX_RANGE -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
#define POS_RANGE 0.001, std::numeric_limits<qreal>::max()

REGISTER_DEFN_PROPERTYTYPE(StyleProperty);

const QStringList StyleProperty::PEN_STYLES = QStringList() << tr("Nothing") <<  tr("solid") << tr("Dashed") << tr("Dotted") << tr("Dash Dotte") << tr("Dash dot dotted");
const QStringList StyleProperty::BRUSH_STYLES = QStringList() << tr("Nothing") << tr("solid") << tr("Extremely dense") << tr("Very dense") << tr("Somewhat dense")
                                                              << tr("Half dense") << tr("Somewhat sparse") << tr ("Very sparse") << tr("Extremely sparse") << tr("Horizontal") << tr("Vertical")
                                                              << tr("Cross") << tr("Backward diagonal") << tr("Forward diagonal") << tr ("Cross diagonal");

StyleProperty::StyleProperty(QString category, QString name)
{
    setCategory(category);
    setName(name);
}

StyleProperty::StyleProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    quint8 brush, pen;
    QString className, category, name;
    qreal x, y, r, s;
    quint8 g;

    stream >> className >> category >> name >> brush >> pen >> _width >> _fillColor >> _drawColor
           >> x >> y >> r >> s >> g;
    setXOffset(x);
    setYOffset(y);
    setRotation(r);
    setScalation(s);
    setIsGlobal((bool) g);
    setCategory(category);
    setName(name);
    Q_ASSERT(type() == className);

    _brushStyle = (Qt::BrushStyle) brush;
    _penStyle = (Qt::PenStyle) pen;

}

QByteArray StyleProperty::toByteArray()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << type() << category() << name() << (quint8) _brushStyle << (quint8) _penStyle << _width << _fillColor << _drawColor
           << xOffset() << yOffset() << rotation() << scalation() << (quint8) isGlobal();

    return data;
}

QString StyleProperty::toString() const
{
    return  QString("%1(%2)").arg(this->type()).arg((quint64) this, 0, 16);
}

void StyleProperty::setPenStyle(Qt::PenStyle penStyle)
{
    if (_penStyle == penStyle) return;

    _penStyle = penStyle;
    emit valueChanged();
    emit penStyleChanged(_penStyle);
}

void StyleProperty::setBrushStyle(Qt::BrushStyle brushStyle)
{
    if (_brushStyle == brushStyle) return;

    _brushStyle = brushStyle;
    emit valueChanged();
    emit brushStyleChanged(_brushStyle);
}

void StyleProperty::setWidth(qreal width)
{
    if (_width == width) return;

    _width = width;
    emit valueChanged();
    emit widthChanged(_width);
}

void StyleProperty::setFillColor(QColor c)
{
    if (_fillColor == c) return;
    _fillColor = c;
    emit valueChanged();
    emit fillColorChanged(_fillColor);
}

void StyleProperty::setDrawColor(QColor c)
{
    if (_drawColor == c) return;
    _drawColor = c;
    emit valueChanged();
    emit drawColorChanged(_drawColor);
}

void StyleProperty::setXOffset(qreal x)
{
    if (_xOff == x) return;

    _xOff = x;
    emit xOffsetChanged(x);
}

void StyleProperty::setYOffset(qreal y)
{
    if (_yOff == y) return;

    _yOff = y;
    emit yOffsetChanged(y);
}

void StyleProperty::setRotation(qreal r)
{
    if (_rotation == r) return;

    _rotation = r;
    emit rotationChanged(r);
}

void StyleProperty::setScalation(qreal s)
{
    if (_scale == s) return;

    _scale = s;
    emit scalationChanged(s);
}

void StyleProperty::setIsGlobal(bool s)
{
    if (_globale == s) return;

    _globale = s;
    emit isGlobaleChanged(s);
}

QWidget* StyleProperty::createWidget(QList<Property *> props, QWidget *parent)
{

    QWidget* container = new QWidget(parent);
    QString name = props.first()->name();
    QPushButton* brushColorButton = new QPushButton(container);
    //brushColorButton->setFlat(true);
    QPushButton* penColorButton = new QPushButton(container);
    //penColorButton->setFlat(true);

    QComboBox* penStyleCB = new QComboBox(container);
    penStyleCB->addItems(PEN_STYLES);

    QComboBox* brushStyleCB = new QComboBox(container);
    brushStyleCB->addItems(BRUSH_STYLES);

    QDoubleSpinBox* widthSB = new QDoubleSpinBox(container);
    widthSB->setRange(0,100);
    widthSB->setSingleStep(0.1);

    QHBoxLayout* drawLayout = new QHBoxLayout();
    QHBoxLayout* fillLayout = new QHBoxLayout();

    drawLayout->addWidget(new QLabel(tr("Pen:"), container));
    drawLayout->addWidget(penColorButton);
    drawLayout->addWidget(penStyleCB);
    drawLayout->addWidget(widthSB);

    fillLayout->addWidget(new QLabel(tr("Brush:"), container));
    fillLayout->addWidget(brushColorButton);
    fillLayout->addWidget(brushStyleCB);

    QCheckBox* useGlobalMatrixCheckBox = new QCheckBox(tr("Global"), container);
    QDoubleSpinBox* xOffsetSP = new QDoubleSpinBox(container);
    xOffsetSP->setPrefix("x = ");
    xOffsetSP->setRange(MAX_RANGE);
    QDoubleSpinBox* yOffsetSP = new QDoubleSpinBox(container);
    yOffsetSP->setPrefix("y = ");
    yOffsetSP->setRange(MAX_RANGE);
    QDoubleSpinBox* rotationSP = new QDoubleSpinBox(container);
    rotationSP->setSuffix(tr(" °"));
    rotationSP->setRange(0,360);
    QDoubleSpinBox* scalationSP = new QDoubleSpinBox(container);
    scalationSP->setRange(POS_RANGE);

    QHBoxLayout* matrixLayout = new QHBoxLayout();
    matrixLayout->addWidget(xOffsetSP);
    matrixLayout->addWidget(yOffsetSP);
    matrixLayout->addWidget(rotationSP);
    matrixLayout->addWidget(scalationSP);


    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(drawLayout);
    layout->addLayout(fillLayout);
    layout->addWidget(useGlobalMatrixCheckBox);
    layout->addLayout(matrixLayout);
    container->setLayout(layout);


    auto updateCB = [=](bool i) {
        bool multipleValues = false;
        quint8 index =  (i? (quint8)((StyleProperty*) props.first())->brushStyle() : (quint8)((StyleProperty*) props.first())->penStyle());

        foreach (Property* p, props) {
            StyleProperty* sp = (StyleProperty*) p;
            if ((i? (quint8)sp->brushStyle() : (quint8)sp->penStyle()) != index) {
                multipleValues = true;
            }
        }
        if (i) brushStyleCB->setCurrentIndex(index);
        else  penStyleCB->setCurrentIndex(index);

        if (multipleValues) {
            if (i) brushStyleCB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
            else penStyleCB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            if (i) brushStyleCB->setStyleSheet("");
            else penStyleCB->setStyleSheet("");
        }
    };

    auto updateColorButton = [=](bool i) {
        //bool multipleValues = false;
        QColor color = (i? ((StyleProperty*) props.first())->fillColor() : ((StyleProperty*) props.first())->drawColor());

        /*foreach (Property* p, props) {
            StyleProperty* sp = (StyleProperty*) p;
            if ((i? sp->fillColor() : sp->drawColor()) != color) {
                multipleValues = true;
            }
        }*/

        if (i) brushColorButton->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue()));
        else penColorButton->setStyleSheet(QString("background-color:rgb(%1,%2,%3)").arg(color.red()).arg(color.green()).arg(color.blue()));
        //TODO multiple values???
    };

    auto updateSpinBox = [=]() {
        bool multipleValues = false;
        double value = ((StyleProperty*) props.first())->width();

        foreach (Property* p, props) {
            StyleProperty* sp = (StyleProperty*) p;
            if (sp->width() != value) {
                multipleValues = true;
            }
        }
        widthSB->setValue(value);

        if (multipleValues) {
            widthSB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            widthSB->setStyleSheet("");
        }
    };

    auto updateMatrix = [=](int i) {
        bool multipleValues = false;
        double value = ((StyleProperty*) props.first())->matrix(i);
        QDoubleSpinBox* spinBox = 0;
        switch (i) {
        case 0: spinBox = xOffsetSP; break;
        case 1: spinBox = yOffsetSP; break;
        case 2: spinBox = rotationSP; break;
        default: spinBox = scalationSP; break;
        }

        foreach (Property* p, props) {
            StyleProperty* sp = (StyleProperty*) p;
            if (sp->matrix(i) != value) {
                multipleValues = true;
            }
        }
        spinBox->setValue((i == 2 ? M_1_PI * 180 : 1) * value);
        spinBox->setStyleSheet(multipleValues ? QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR) : "");
    };

    auto updateGlobalCheckbox = [=]() {
        bool multipleValue = false;
        bool value = ((StyleProperty*) props.first())->isGlobal();

        foreach (Property* p, props) {
            StyleProperty* sp = (StyleProperty*) p;
            if (sp->isGlobal() != value) {
                multipleValue = true;
            }
        }
        useGlobalMatrixCheckBox->setChecked(value);
        useGlobalMatrixCheckBox->setStyleSheet(multipleValue ? QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR) : "");
    };

    void (QComboBox:: *indexChangedInt)(int) = &QComboBox::currentIndexChanged;
    void (QDoubleSpinBox:: *valueChangedDouble)(double) = &QDoubleSpinBox::valueChanged;

    foreach (Property* p, props) {
        StyleProperty* sp = (StyleProperty*) p;
        connect(brushStyleCB, indexChangedInt, [=](int index) {
            sp->setBrushStyle((Qt::BrushStyle) index);
        });
        connect(penStyleCB, indexChangedInt, [=](int index) {
            sp->setPenStyle((Qt::PenStyle) index);
        });
        connect(widthSB, valueChangedDouble, [=](double width) {
           sp->setWidth(width);
        });
        connect(xOffsetSP, valueChangedDouble, [=] (double x) {
            sp->setXOffset(x);
        });
        connect(yOffsetSP, valueChangedDouble, [=] (double y) {
            sp->setYOffset(y);
        });
        connect(rotationSP, valueChangedDouble, [=] (double r) {
            sp->setRotation(r * M_PI / 180.0);
        });
        connect(scalationSP, valueChangedDouble, [=] (double s) {
            sp->setScalation(s);
        });
        connect(useGlobalMatrixCheckBox, &QCheckBox::clicked, [=] () {
            sp->setIsGlobal(useGlobalMatrixCheckBox->isChecked());
        });

        connect(sp, &StyleProperty::penStyleChanged, [=](){updateCB(false);});
        connect(sp, &StyleProperty::brushStyleChanged, [=](){updateCB(true);});
        connect(sp, &StyleProperty::widthChanged, [=](){updateSpinBox();});
        connect(sp, &StyleProperty::drawColorChanged, [=](){updateColorButton(false);});
        connect(sp, &StyleProperty::fillColorChanged, [=](){updateColorButton(true);});
        connect(sp, &StyleProperty::xOffsetChanged, [=](){updateMatrix(0);});
        connect(sp, &StyleProperty::yOffsetChanged, [=](){updateMatrix(1);});
        connect(sp, &StyleProperty::rotationChanged, [=](){updateMatrix(2);});
        connect(sp, &StyleProperty::scalationChanged, [=](){updateMatrix(3);});
        connect(sp, &StyleProperty::isGlobaleChanged, [=]() {updateGlobalCheckbox();});
    }

    connect(brushColorButton, &QPushButton::clicked, [=](){
        QColor color = QColorDialog::getColor(((StyleProperty*) props.first())->fillColor(), container, "Fill Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            foreach (Property* p, props) {
                StyleProperty* sp = (StyleProperty*) p;
                sp->setFillColor(color);
            }
        }
    });

    connect(penColorButton, &QPushButton::clicked, [=](){
        QColor color = QColorDialog::getColor(((StyleProperty*) props.first())->drawColor(), container, "Fill Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            foreach (Property* p, props) {
                StyleProperty* sp = (StyleProperty*) p;
                sp->setDrawColor(color);
            }
        }
    });

    updateCB(true);
    updateCB(false);
    updateColorButton(true);
    updateColorButton(false);
    updateSpinBox();
    updateGlobalCheckbox();
    for (int i : {0,1,2,3,})
        updateMatrix(i);

    if (name.isEmpty()) {
        return container;
    } else {
        QWidget* w = new QWidget(parent);
        container->setParent(w);
        QVBoxLayout* layout = new QVBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(container);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}

qreal StyleProperty::matrix(int i)
{
    switch (i) {
    case 0: return _xOff;
    case 1: return _yOff;
    case 2: return _rotation;
    case 3: return _scale;
    }
    Q_ASSERT(false);
    return 0;
}

QMatrix StyleProperty::transform() const
{
    return QMatrix( scalation() * qCos(rotation()), scalation() * qSin(rotation()),
                    -scalation() * qCos(rotation()), scalation() * qCos(rotation()),
                    xOffset(), yOffset());
}
