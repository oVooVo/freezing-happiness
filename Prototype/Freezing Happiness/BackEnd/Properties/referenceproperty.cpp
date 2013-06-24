#include "referenceproperty.h"
#include "BackEnd/project.h"
#include "FrontEnd/EditorWidgets/objectedit.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

REGISTER_DEFN_PROPERTYTYPE(ReferenceProperty);

ReferenceProperty::ReferenceProperty(QByteArray *data)
{
    QDataStream stream(data, QIODevice::ReadOnly);
    QString name, className, category;
    quint8 hasObject;
    stream >> className >> category >> name >> hasObject;
    if (hasObject == (quint64) 1) {
        stream >> _id;
        _empty = false;
    } else {
        _empty = true;
    }
    Q_ASSERT(className == type());
    setCategory(category);
    setName(name);
}

ReferenceProperty::ReferenceProperty(QString category, QString name)
{
    _empty = true;
    setCategory(category);
    setName(name);
    connect(this, &ReferenceProperty::valueChanged, [=]() { qDebug() << "value changed to " << _id; });
}

QByteArray ReferenceProperty::toByteArray()
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << type() << category() << name();
    if (!_empty) {
        stream << (quint8) 1 << _id;
    } else {
        stream << (quint8) 0;
    }
    return data;
}

QWidget* ReferenceProperty::createWidget(QList<Property *> props, QWidget *parent)
{
    QWidget* w = new QWidget(parent);
    QVBoxLayout* layout = new QVBoxLayout();
    QString name = props.first()->name();
    if (!name.isEmpty()) {
        QLabel* label = new QLabel(w);
        label->setText(props.first()->name());
        layout->addWidget(label);
    }

    ObjectEdit* edit = new ObjectEdit(w);
    edit->setProject(props.first()->project());
    layout->addWidget(edit);

    auto updateObjectEdit = [=]() {
        bool multipleValues = false;
        quint64 id = ((ReferenceProperty*) props.first())->id();
        bool empty = ((ReferenceProperty*) props.first())->isEmpty();

        foreach (Property* p, props) {
            ReferenceProperty* refProp = (ReferenceProperty*) p;
            if (refProp->id() != id || refProp->isEmpty() != empty) {
                multipleValues = true;
            }
        }

        edit->blockSignals(true);
        if (empty) {
            edit->clear();
        } else {
            edit->setId(id);
        }
        if (multipleValues) {
            edit->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        } else {
            edit->setStyleSheet("");
        }
        edit->blockSignals(false);

    };

    foreach (Property* p, props) {
        ReferenceProperty* refProp = (ReferenceProperty*) p;
        connect(refProp, &ReferenceProperty::valueChanged, updateObjectEdit);
        connect(edit, &ObjectEdit::statusChanged, [=]() { refProp->setId(edit->id(), edit->isEmpty()); } );
    }
    updateObjectEdit();

    w->setLayout(layout);
    return w;
}

void ReferenceProperty::setId(quint64 id, bool isEmpty)
{
    if (_id == id && _empty == isEmpty) return;

    if (!_empty) {
        Object* object = project()->getObject(id);
        if (object)
            disconnect(object, SIGNAL(iChanged()), this, SLOT(emitWatchedObjectChanged()));
    }

    _id = id;
    _empty = isEmpty;

    if (!_empty) {
        Object* object = project()->getObject(id);
        connect(object, SIGNAL(iChanged()), this, SLOT(emitWatchedObjectChanged()));
    }

    emit valueChanged();
}

void ReferenceProperty::emitWatchedObjectChanged()
{
    emit watchedObjectChanged();
}
