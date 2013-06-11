#include "styleproperty.h"
#include <QPainter>
#include <QLineEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QColorDialog>
#include <QLabel>
#include <QHBoxLayout>

REGISTER_DEFN_PROPERTYTYPE(StyleProperty);

const QStringList StyleProperty::PEN_STYLES = QStringList() << tr("Nothing") <<  tr("solid") << tr("Dashed") << tr("Dotted") << tr("Dash Dotte") << tr("Dash dot dotted");
const QStringList StyleProperty::BRUSH_STYLES = QStringList() << tr("Nothing") << tr("solid") << tr("Extremely dense") << tr("Very dense") << tr("Somewhat dense")
                                                              << tr("Half dense") << tr("Somewhat sparse") << tr("Extremely sparse") << tr("Horizontal") << tr("Vertical")
                                                              << tr("Cross") << tr("Backward diagonal") << tr("Forward diagonal") << tr ("Cross diagonal");

StyleProperty::StyleProperty(QString category, QString name)
{
    _brushStyle = Qt::NoBrush;
    _penStyle = Qt::SolidLine;
    _fillColor = Qt::white;
    _drawColor = Qt::black;
    _width = 1;
    setCategory(category);
    setName(name);
}

StyleProperty::StyleProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    quint8 brush, pen;
    QString className, category, name;

    stream >> className >> category >> name >> brush >> pen >> _width >> _fillColor >> _drawColor;
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

    stream << type() << category() << name() << (quint8) _brushStyle << (quint8) _penStyle << _width
              << _fillColor << _drawColor;

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

    drawLayout->addWidget(penColorButton);
    drawLayout->addWidget(penStyleCB);

    fillLayout->addWidget(brushColorButton);
    fillLayout->addWidget(brushStyleCB);

    QVBoxLayout* layout = new QVBoxLayout();
    layout->addLayout(drawLayout);
    layout->addLayout(fillLayout);
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

        connect(sp, &StyleProperty::penStyleChanged, [=](){updateCB(false);});
        connect(sp, &StyleProperty::brushStyleChanged, [=](){updateCB(true);});
        connect(sp, &StyleProperty::widthChanged, [=](){updateSpinBox();});
        connect(sp, &StyleProperty::drawColorChanged, [=](){updateColorButton(false);});
        connect(sp, &StyleProperty::fillColorChanged, [=](){updateColorButton(true);});
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

    if (name.isEmpty()) {
        return container;
    } else {
        QWidget* w = new QWidget(parent);
        container->setParent(w);
        QHBoxLayout* layout = new QHBoxLayout();
        layout->addWidget(new QLabel(QString("%1:").arg(name), parent));
        layout->addWidget(container);
        layout->setContentsMargins(0,0,0,0);
        layout->setSpacing(0);
        w->setLayout(layout);
        return w;
    }
}
