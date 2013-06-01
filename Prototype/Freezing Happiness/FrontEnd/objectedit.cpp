#include "objectedit.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>


#include "objectedit.h"
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>

ObjectEdit::ObjectEdit(QWidget *parent) : QLineEdit(parent)
{
    _empty = true;
    _objectValid = false;
    _project = 0;
    connect(this, SIGNAL(editingFinished()), this, SLOT(onTextEditFinished()));
    installEventFilter(this);
}

void ObjectEdit::setId(quint64 id)
{
    if (id == _id && !_empty) return;

    _empty = false;
    blockSignals(true);
    setText(QString("%1").arg(id));
    blockSignals(false);
    onTextEditFinished();
    emit statusChanged();
}

void ObjectEdit::clear()
{
    _empty = true;
    emit statusChanged();
    QLineEdit::clear();
}

Object* ObjectEdit::object()
{
    if (_empty) return 0;

    if (!_objectValid) {
        _object = _project->getObject(_id);
    }
    return _object;
}

void ObjectEdit::onTextEditFinished()
{
    if (!_project) {
        clear();
        return;
    }

    bool ok;
    quint64 id = text().toULongLong(&ok);
    Object* object;
    if (ok) {
        object = _project->getObject(id);
    } else {
        object = _project->find(text());
    }
    if (object) {
        _id = id;
        setText(object->name());
    } else {
        clear();
    }
}

void ObjectEdit::mousePressEvent(QMouseEvent *e)
{
    clear();
    QLineEdit::mousePressEvent(e);
}

void ObjectEdit::dropEvent(QDropEvent* event)
{
    if (!_project) return;

    QString text = event->mimeData()->text();
    QStringList ids = text.split(',', QString::SkipEmptyParts);
    if (!ids.isEmpty()) {
        bool ok;
        quint64 id = ids[0].toULongLong(&ok);
        if (!ok) {
            qDebug() << "ObjectEdit::dropEvent: illegal id";
            return;
        } else {
            setId(id);
        }
    }
}

void ObjectEdit::objectMightBeDeleted()
{
    if (isEmpty()) return;

    if (!_project->getObject(_id)) {
        clear();
    }
}

void ObjectEdit::setProject(Project *project)
{
    _project = project;
    if (project) {
        connect(_project, SIGNAL(structureChanged()), this, SLOT(objectMightBeDeleted()));
    }
    clear();
}
