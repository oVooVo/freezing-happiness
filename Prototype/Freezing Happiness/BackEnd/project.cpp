#include <QFileDialog>
#include <QMenu>
#include <QMenuBar>
#include <QScrollArea>
#include "project.h"
#include <QDebug>
#include "Tags/tag.h"
#include <QDateTime>
#include "Objects/point.h"
#include "BackEnd/renderoptions.h"
#include <QImageReader>
#include <QLabel>
#include <qmath.h>
#include "Objects/camera.h"
#include <BackEnd/mathutility.h>


Project::Project(Object *root)
{
    _recordHistory = false;
    _root = root;
    _maxId = 0;
    if (_root) {
        Q_ASSERT_X(_root->_project == this, "Project::Project(Object*)", "root.project.root =! root");
    }
    _selectedObjectsCacheUptodate = false;
    _objectsCacheUptodate = false;
    _selectedParentsCacheUptodate = false;
    _selectedChildrenCacheUptodate = false;
    _selectionsParentCacheUptodate = false;
}

Project::~Project()
{
    delete _root;
}

bool Project::setRoot(Object *root)
{
    if (_root || !root) return false;   //if _root is already set or trying to set _root 0
    _root = root;

    _objects.insert(root->id(), root);

    Q_ASSERT_X(_root->_project == this, "Project::setRoot(Object*)", "root.project.root =! root");

    emit structureChanged();
    return true;
}

QString Project::visualize() const
{
    return QString("\n").append(_root->visualize());
}

QDataStream& operator<<(QDataStream &out, Project* p)
{
    out << p->log();
    p->root()->serialize(out);
    return out;
}

QDataStream& operator>>(QDataStream &in, Project*& p)
{
    p = new Project();
    p->blockSignals(true);

    QString log;

    in >> log;
    p->setLog(log);

    quint64 childrenCount;
    Object* root = Object::deserialize(in, p, true, false, &childrenCount);
    p->setRoot(root);
    for (quint64 i = 0; i < childrenCount; i++) {
        Object* o = Object::deserialize(in, p);
        o->setTreeParent(root);
    }

    p->blockSignals(false);
    p->emitStructureChanged();
    return in;
}

quint64 Project::reserveId(Object* object, quint64 id)
{
    if (_freeIds.contains(id)) {
        _freeIds.removeOne(id);
    } else if (_maxId <= id) {
        for (quint64 i = _maxId; i < id; i++) {
            _freeIds.append(i);
        }
        _maxId = id + 1;
    } else {
        if (_freeIds.isEmpty()) {
            id = _maxId;
            _maxId += 1;
        } else {
            id = _freeIds.takeFirst();
        }
    }
    _objects.insert(id, object);
    object->setId(id);
    return id;
}

void Project::freeId(quint64 id)
{
    _objects.remove(id);
    _freeIds.append(id);
}

Object* Project::getObject(quint64 id)
{
    return _objects[id];
}

void Project::emitObjectChanged(Object *object)
{
    emit objectChanged(object);
    createNewUndoRecord();
}

void Project::emitStructureChanged()
{
    emit structureChanged();
    createNewUndoRecord();
    _selectedObjectsCacheUptodate = false;
    _objectsCacheUptodate = false;
    _selectedParentsCacheUptodate = false;
    _selectedChildrenCacheUptodate = false;
    _selectionsParentCacheUptodate = false;
}

void Project::emitSelectionChanged()
{
    emit selectionChanged();
    _selectedObjectsCacheUptodate = false;
    _selectedParentsCacheUptodate = false;
    _selectedChildrenCacheUptodate = false;
    _selectionsParentCacheUptodate = false;
}

QList<Object*> Project::selectedObjects() //Returns all selected Objects.
{
    if (!_selectedChildrenCacheUptodate) {
        _selectedChildrenCache = _root->selectedObjects();
        _selectedChildrenCacheUptodate = true;
    }
    return _selectedChildrenCache;
}

QList<Object*> Project::objects()       //Returns all objects.
{
    if (!_objectsCacheUptodate) {
        _objectsCache = _root->subjects();
        _objectsCacheUptodate = true;
    }
    return _objectsCache;
}

QList<Object*> Project::selectedParents()     //Returns selected Objects, but never a child of a selected Object.
{
    if (!_selectedParentsCacheUptodate) {
        _selectedParentsCache = _root->selectedParents();
        _selectedParentsCacheUptodate = true;
    }
    return _selectedParentsCache;
}

QList<Object*> Project::selectedChildren()    //Returns selected Objects and every children of a selected Object.
{
    if (!_selectedChildrenCacheUptodate) {
        _selectedChildrenCache = _root->selectedChildren();
        _selectedChildrenCacheUptodate = true;
    }
    return _selectedChildrenCache;
}

Object* Project::selectionsParent()
{

    if (!_selectionsParentCacheUptodate) {

        if (selectedParents().isEmpty()) return 0;

        Object* candidate = selectedParents().first();
        bool ok = true;

        do {
            ok = true;
            foreach (Object* o, selectedParents()) {
                if (!candidate->isAncestorOf(o)) {
                    ok = false;
                    Q_ASSERT_X(candidate->treeParent(), "Project::selectionsParent()", "no common parent!");
                    candidate = candidate->treeParent();
                    break;
                }
            }
        } while (!ok);

        // make sure to return a parent. (x.isAncestor(x) && x.isDecedantOf(x) == true)!
        if (selectedParents().contains(candidate) && candidate->treeParent()) {
            candidate = candidate->treeParent();
        }
        _selectionsParentCache = candidate;
        _selectionsParentCacheUptodate = true;

    }

    return _selectionsParentCache;
}

void Project::createNewUndoRecord(bool force)
{
    if (isHistoryRecorded() || force) {
        emit newUndoRecordRequest();
    }
}

void Project::addLogItem(QString item)
{
    _log.append(QString("(%2) -- %1")).arg(item).arg(QDateTime::currentDateTime().toString("YYYY.MM.dd hh:mm:ss.zzz"));
}


Object* Project::find(QString name)
{
    foreach (Object* o, objects()) {
        if (o->name() == name) {
            return o;
        }
    }
    return 0;
}

void Project::duplicateSelected()
{
    blockSignals(true);
    setRecordHistory(false);
    bool _rootWasSelected = root()->isSelected();
    root()->setSelected(false);
    QList<Object*> selection = selectedParents();
    foreach (Object* object, selection) {
        QByteArray data;
        QDataStream out(&data, QIODevice::WriteOnly);
        object->serialize(out);

        QDataStream in(&data, QIODevice::ReadOnly);
        Object* copy = Object::deserialize(in, this, false);
        copy->setName(object->name().append(tr(" (Copy)")));
        copy->setSelected(false);
        copy->setTreeParent(object->treeParent());
    }
    root()->setSelected(_rootWasSelected);
    blockSignals(false);
    setRecordHistory(true);
    emitStructureChanged();
}


QMap<QString, QList<Property*> > Project::activeProperties()
{

    if (selectedObjects().isEmpty()) {
        return QMap<QString, QList<Property*> >();
    }

    QList<QString> candidates = selectedObjects().first()->properties().keys();
    QList<QString> candidates2;

    if (candidates.isEmpty()) {
        return QMap<QString, QList<Property*> >();
    }

    foreach (Object* object, selectedObjects()) {
        foreach (QString s, candidates) {
            if (object->hasProperty(s)) {
                candidates2.append(s);
            }
        }
        candidates = candidates2;
        candidates2.clear();
    }

    QMap<QString, QList<Property*> > props;
    foreach (QString role, candidates) {
        QList<Property*> properties;
        foreach (Object* object, selectedObjects()) {
            properties.append(object->property(role));
        }
        props.insert(role, properties);
    }

    return props;
}


QMap<QString, QList<Tag *> > Project::activeTags()
{
    if (selectedObjects().isEmpty()) {
        return QMap<QString, QList<Tag*> >();
    }

    QList<QString> candidates;
    foreach (Tag* t, selectedObjects().first()->tags()) {
        candidates.append(t->type());
    }

    QList<QString> candidates2;

    if (candidates.isEmpty()) {
        return QMap<QString, QList<Tag*> >();
    }

    foreach (Object* object, selectedObjects()) {
        foreach (QString s, candidates) {
            if (object->hasTag(s)) {
                candidates2.append(s);
            }
        }
        candidates = candidates2;
        candidates2.clear();
    }

    QMap<QString, QList<Tag*> > tagsMap;
    foreach (QString className, candidates) {
        QList<Tag*> tags;
        foreach (Object* object, selectedObjects()) {
            tags.append(object->tag(className));
        }
        tagsMap.insert(className, tags);
    }
    return tagsMap;
}

bool Project::addPoint(QPointF globalePosition)
{
    Object* parent = 0;
    QListIterator<Object*> it(selectedObjects());
    while (it.hasNext()) {
        Object* tester = it.next();
        if (tester->isPointObject()) {
            parent = tester;
            break;
        }
    }

    if (!parent) return false;

    Object* point = new Point(this, "Point");
    point->setTreeParent(parent, false); //non invariant global transform, becaus glob trans becomes overridden soon
    point->setGlobalePosition(globalePosition, Object::ObjectMode);
    return true;

}

void Project::clearSelection()
{
    if (selectedObjects().isEmpty()) return;
    foreach (Object* o, selectedObjects()) {
        o->blockSignals(true);
        o->setSelected(false);
        o->blockSignals(false);
    }
    emitSelectionChanged();
}

void Project::paint(QPainter &p)
{
    p.save();
    p.setTransform(_root->localeTransform());
    _root->paint(p);
    p.restore();
    if (showRenderFrame) {
        p.save();
        QPen pen;
        pen.setCosmetic(true);
        p.setPen(pen);
        p.setTransform(cameraObject()->globaleTransform());
        p.drawRect(-_renderOptions.resolution().width()/2, -_renderOptions.resolution().height()/2,
                    _renderOptions.resolution().width(),    _renderOptions.resolution().height());
        p.restore();
    }
}

QImage Project::render()
{
    QImage image = QImage(_renderOptions.resolution(), QImage::Format_ARGB32);
    image.fill(Qt::transparent);
    QPainter p(&image);
    p.setRenderHint(QPainter::Antialiasing);

    for (Object* o : objects()) {
        p.save();

        p.translate(_renderOptions.resolution().width()/2, _renderOptions.resolution().height()/2);
        p.setTransform(cameraObject()->globaleTransformWithoutRoot().inverted(), true);
        p.setTransform(o->globaleTransformWithoutRoot(), true);

        o->customDraw(p);
        p.restore();
    }
    return image;
}

void Project::showRenderOptions()
{
    _renderOptions.showConfigurator();
}

Object* Project::cameraObject()
{
    for (Object* o : objects()) {
        if (o->type() == "Camera" && ((Camera*) o)->isActive()) {
            return o;
        }
    }
    return root();
}

void Project::convertSelected()
{

    blockSignals(true);
    for (Object* o : selectedParents()) {
        if (!o->convert()) return;
        if (o->deletable()) {
            freeId(o->id());
            delete o;
        }
    }
    blockSignals(false);
    emitStructureChanged();
}











