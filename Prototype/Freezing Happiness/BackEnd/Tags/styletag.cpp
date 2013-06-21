#include "styletag.h"
#include <QPushButton>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include <QColorDialog>
#include <qmath.h>
#include <QDebug>

REGISTER_DEFN_TAGTYPE(StyleTag);

#define MAX_RANGE -std::numeric_limits<qreal>::max(), std::numeric_limits<qreal>::max()
#define POS_RANGE 0.001, std::numeric_limits<qreal>::max()

const QStringList StyleTag::PEN_STYLES = QStringList() << tr("Nothing") <<  tr("solid") << tr("Dashed") << tr("Dotted") << tr("Dash Dotte") << tr("Dash dot dotted");
const QStringList StyleTag::BRUSH_STYLES = QStringList() << tr("Nothing") << tr("solid") << tr("Extremely dense") << tr("Very dense") << tr("Somewhat dense")
                                                              << tr("Half dense") << tr("Somewhat sparse") << tr ("Very sparse") << tr("Extremely sparse") << tr("Horizontal") << tr("Vertical")
                                                              << tr("Cross") << tr("Backward diagonal") << tr("Forward diagonal") << tr ("Cross diagonal");

StyleTag::StyleTag(Object *owner, QByteArray *data)
{
    setOwner(owner);
    if (data) {
        QDataStream stream(data, QIODevice::ReadOnly);
        quint8 brush, pen;
        QString className;
        qreal x, y, r, s;
        quint8 g;

        stream >> className >> brush >> pen >> _width >> _fillColor >> _drawColor
               >> x >> y >> r >> s >> g;
        setXOffset(x);
        setYOffset(y);
        setRotation(r);
        setScalation(s);
        setIsGlobal((bool) g);
        Q_ASSERT(type() == className);

        _brushStyle = (Qt::BrushStyle) brush;
        _penStyle = (Qt::PenStyle) pen;
    }
}

QByteArray StyleTag::toByteArray() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);

    stream << type() << (quint8) _brushStyle << (quint8) _penStyle << _width << _fillColor << _drawColor
           << xOffset() << yOffset() << rotation() << scalation() << (quint8) isGlobal();

    return data;
}

QString StyleTag::toString() const
{
    return  QString("%1(%2)").arg(this->type()).arg((quint64) this, 0, 16);
}

void StyleTag::setPenStyle(Qt::PenStyle penStyle)
{
    if (_penStyle == penStyle) return;

    _penStyle = penStyle;
    emit valueChanged();
    emit penStyleChanged(_penStyle);
}

void StyleTag::setBrushStyle(Qt::BrushStyle brushStyle)
{
    if (_brushStyle == brushStyle) return;

    _brushStyle = brushStyle;
    emit valueChanged();
    emit brushStyleChanged(_brushStyle);
}

void StyleTag::setWidth(qreal width)
{
    if (_width == width) return;

    _width = width;
    emit valueChanged();
    emit widthChanged(_width);
}

void StyleTag::setFillColor(QColor c)
{
    if (_fillColor == c) return;
    _fillColor = c;
    emit valueChanged();
    emit fillColorChanged(_fillColor);
}

void StyleTag::setDrawColor(QColor c)
{
    if (_drawColor == c) return;
    _drawColor = c;
    emit valueChanged();
    emit drawColorChanged(_drawColor);
}

void StyleTag::setXOffset(qreal x)
{
    if (_xOff == x) return;

    _xOff = x;
    emit xOffsetChanged(x);
}

void StyleTag::setYOffset(qreal y)
{
    if (_yOff == y) return;

    _yOff = y;
    emit yOffsetChanged(y);
}

void StyleTag::setRotation(qreal r)
{
    if (_rotation == r) return;

    _rotation = r;
    emit rotationChanged(r);
}

void StyleTag::setScalation(qreal s)
{
    if (_scale == s) return;

    _scale = s;
    emit scalationChanged(s);
}

void StyleTag::setIsGlobal(bool s)
{
    if (_globale == s) return;

    _globale = s;
    emit isGlobaleChanged(s);
}

QWidget* StyleTag::createWidget(QList<Tag *> tags, QWidget *parent)
{

    QWidget* container = new QWidget(parent);
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
    layout->addWidget(Tag::closeButton(tags, container));
    layout->addLayout(drawLayout);
    layout->addLayout(fillLayout);
    layout->addWidget(useGlobalMatrixCheckBox);
    layout->addLayout(matrixLayout);
    container->setLayout(layout);


    auto updateCB = [=](bool i) {
        bool multipleValues = false;
        quint8 index =  (i? (quint8)((StyleTag*) tags.first())->brushStyle() : (quint8)((StyleTag*) tags.first())->penStyle());

        foreach (Tag* p, tags) {
            StyleTag* sp = (StyleTag*) p;
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
        QColor color = (i? ((StyleTag*) tags.first())->fillColor() : ((StyleTag*) tags.first())->drawColor());

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
        double value = ((StyleTag*) tags.first())->width();

        foreach (Tag* p, tags) {
            StyleTag* sp = (StyleTag*) p;
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
        double value = ((StyleTag*) tags.first())->matrix(i);
        QDoubleSpinBox* spinBox = 0;
        switch (i) {
        case 0: spinBox = xOffsetSP; break;
        case 1: spinBox = yOffsetSP; break;
        case 2: spinBox = rotationSP; break;
        default: spinBox = scalationSP; break;
        }

        foreach (Tag* p, tags) {
            StyleTag* sp = (StyleTag*) p;
            if (sp->matrix(i) != value) {
                multipleValues = true;
            }
        }
        spinBox->setValue((i == 2 ? M_1_PI * 180 : 1) * value);
        spinBox->setStyleSheet(multipleValues ? QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR) : "");
    };

    auto updateGlobalCheckbox = [=]() {
        bool multipleValue = false;
        bool value = ((StyleTag*) tags.first())->isGlobal();

        foreach (Tag* p, tags) {
            StyleTag* sp = (StyleTag*) p;
            if (sp->isGlobal() != value) {
                multipleValue = true;
            }
        }
        useGlobalMatrixCheckBox->setChecked(value);
        useGlobalMatrixCheckBox->setStyleSheet(multipleValue ? QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR) : "");
    };

    void (QComboBox:: *indexChangedInt)(int) = &QComboBox::currentIndexChanged;
    void (QDoubleSpinBox:: *valueChangedDouble)(double) = &QDoubleSpinBox::valueChanged;

    foreach (Tag* p, tags) {
        StyleTag* sp = (StyleTag*) p;
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

        connect(sp, &StyleTag::penStyleChanged, [=](){updateCB(false);});
        connect(sp, &StyleTag::brushStyleChanged, [=](){updateCB(true);});
        connect(sp, &StyleTag::widthChanged, [=](){updateSpinBox();});
        connect(sp, &StyleTag::drawColorChanged, [=](){updateColorButton(false);});
        connect(sp, &StyleTag::fillColorChanged, [=](){updateColorButton(true);});
        connect(sp, &StyleTag::xOffsetChanged, [=](){updateMatrix(0);});
        connect(sp, &StyleTag::yOffsetChanged, [=](){updateMatrix(1);});
        connect(sp, &StyleTag::rotationChanged, [=](){updateMatrix(2);});
        connect(sp, &StyleTag::scalationChanged, [=](){updateMatrix(3);});
        connect(sp, &StyleTag::isGlobaleChanged, [=]() {updateGlobalCheckbox();});
    }

    connect(brushColorButton, &QPushButton::clicked, [=](){
        QColor color = QColorDialog::getColor(((StyleTag*) tags.first())->fillColor(), container, "Fill Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            foreach (Tag* p, tags) {
                StyleTag* sp = (StyleTag*) p;
                sp->setFillColor(color);
            }
        }
    });

    connect(penColorButton, &QPushButton::clicked, [=](){
        QColor color = QColorDialog::getColor(((StyleTag*) tags.first())->drawColor(), container, "Fill Color", QColorDialog::ShowAlphaChannel);
        if (color.isValid()) {
            foreach (Tag* p, tags) {
                StyleTag* sp = (StyleTag*) p;
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

    return container;
}

qreal StyleTag::matrix(int i)
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

QMatrix StyleTag::transform() const
{
    return QMatrix( scalation() * qCos(rotation()), scalation() * qSin(rotation()),
                    -scalation() * qCos(rotation()), scalation() * qCos(rotation()),
                    xOffset(), yOffset());
}

