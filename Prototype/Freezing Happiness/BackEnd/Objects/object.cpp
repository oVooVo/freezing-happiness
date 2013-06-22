#include "object.h"
#include "BackEnd/project.h"
#include <QDebug>
#include "BackEnd/Properties/propertystring.h"
#include "BackEnd/Tags/styletag.h"
#include <QString>
#include "BackEnd/mathutility.h"
#include <qmath.h>
#include "BackEnd/Tags/tag.h"
#include <QTimer>
#include <QMetaMethod>
#include "BackEnd/Properties/transformproperty.h"


QMap<QString, Object* (*)(Project*)> *Object::_creatorMap = 0;

Object::Object(Project* project, QString name, bool isRoot)
{
    _globaleTransformUpToDate = false;
    _globaleTransformInvertedUpToDate = false;
    _isSelected = false;
    _isExpanded = true;
    _project = project;
    if (!project) return;

    _id = _project->reserveId(this);
    addProperty("Name", new PropertyString("Object", "Name", name));
    addProperty("localeMatrix", new TransformProperty("Object", "Locale Matrix"));

    if (isRoot) {
        _parent = 0;
    } else {
        project->root()->_children.append(this);
        _parent = project->root();
    }
}

Property* Object::addProperty(QString key, Property *property)
{
    property->setProject(project());
    _properties.insert(key, property);
    connect(property, SIGNAL(valueChanged()), this, SLOT(emitObjectChanged()));
    return property;
}

void Object::emitObjectChanged()
{
    adjustProperties();
    dumpGlobaleTransformationCache();
    if (_parent) _parent->childrenHasChanged();
    _project->emitObjectChanged(this);
    emit iChanged();
}

Object::~Object()
{
    //on destroying project, root becomes deleted!
    //Q_ASSERT_X(this != _project->root(), "Object::~Object", "Trying to delete root");

    setSelected(false);
    if (_parent) {
        _parent->_children.removeOne(this);
    }
    foreach (Property* p, _properties) {
        p->deleteLater();
    }

    foreach (Object* child, _children) {
        delete child;
    }
}

bool Object::setTreeParent(Object *parent, bool invariantGlobalTransform)
{
    if (parent->isDescedantOf(this)) return false;  //prevent cyclic parentship
    if (parent && !parent->canHaveChildren()) return false;

    QTransform gTransform = QTransform();
    if (invariantGlobalTransform)
        gTransform = globaleTransform();   //should be invariant

    if (_parent) {
        _parent->_children.removeOne(this);
    }
    if (parent) {
        parent->_children.append(this);
    }
    _parent = parent;

    if (invariantGlobalTransform)
        setGlobaleTransform(gTransform);

    setParent(_parent);
    if (_parent) _parent->childrenHasChanged();
    _project->emitStructureChanged();
    return true;
}

bool Object::moveBelow(Object *above)
{
    Object* parent = above->_parent;
    if (!parent || !setTreeParent(parent)) {
        return false;
    }

    Q_ASSERT(parent->_children.last() == this);
    parent->_children.takeLast();

    QList<Object*> removedChildren;
    while (parent->_children.last() != above) {
        removedChildren.append(parent->_children.takeLast());
    }
    parent->_children.append(this);
    while (!removedChildren.isEmpty()) {
        parent->_children.append(removedChildren.takeLast());
    }

    _project->emitStructureChanged();
    return true;
}

bool Object::moveAbove(Object *below)
{
    Object* parent = below->_parent;
    if (!parent || !setTreeParent(parent)) {
        return false;
    }


    Q_ASSERT(parent->_children.last() == this);
    parent->_children.takeLast();

    QList<Object*> removedChildren;
    while (parent->_children.last() != below) {
        removedChildren.append(parent->_children.takeLast());
    }
    removedChildren.append(parent->_children.takeLast());
    parent->_children.append(this);
    while (!removedChildren.isEmpty()) {
        parent->_children.append(removedChildren.takeLast());
    }

    _project->emitStructureChanged();
    return true;
}

bool Object::isDescedantOf(Object *o) const
{
    if (o == 0) {
        return false;
    } else if (this == o) {
        return true;
    } else if (_parent) {
        return _parent->isDescedantOf(o);
    } else {
        return false;
    }
}

bool Object::isAncestorOf(Object* o) const
{
    if (o == 0) {
        return false;
    } else if (o == this) {
        return true;
    } else if (o->treeParent()) {
        return isAncestorOf(o->treeParent());
    } else {
        return false;
    }
}


QString Object::toString() const
{
    return name();
}

QString Object::visualize(int level) const
{
    QString pre;
    for (int i = 0; i <= level; i++) {
        pre = pre.append(" ");
    }
    QString s = toString().append("[%1][%2]{%3}\n").arg(isSelected() ? "x": "o").arg(level).arg(_id);
    foreach (Object* child, _children) {
        s = s.append(pre).append(child->visualize(level + 1));
    }
    return s;
}

void Object::addChild(Object *o)
{
    _children.append(o);
    o->_parent = this;
}

Object* Object::createInstance(QString className, Project *project)
{
    if (!_creatorMap)
    {
        Q_ASSERT(false);
        _creatorMap = new QMap<QString, Object* (*)(Project*)>();
    }

    QMap<QString, Object* (*)(Project*)>::iterator it = _creatorMap->find(className);
    if (it == _creatorMap->end()) {
        qDebug() << "unable to find" << className;
        Q_ASSERT(false);
        return 0;
    }

    return (it.value())(project);
}

QString Object::type() const
{
    return metaObject()->className();
}

void Object::setId(quint64 id)
{
    _id = id;
}

QString Object::name() const
{
    return ((PropertyString*) _properties["Name"])->string();
}

void Object::setName(QString newName)
{
    if (name() == newName) return;

    ((PropertyString*) _properties["Name"])->setString(newName);
}

QTransform Object::localeTransform() const
{
    return ((TransformProperty*) _properties["localeMatrix"])->transform();
}

void Object::dumpGlobaleTransformationCache()
{
    _globaleTransformUpToDate = false;
    _globaleTransformInvertedUpToDate = false;
    foreach (Object* child, _children) {
        child->dumpGlobaleTransformationCache();
    }
}

void Object::setLocaleTransform(QTransform t)
{
    dumpGlobaleTransformationCache();
    ((TransformProperty*) _properties["localeMatrix"])->setTransform(t);
}

void Object::setLocalePosition(QPointF pos)
{
    dumpGlobaleTransformationCache();
    ((TransformProperty*) _properties["localeMatrix"])->setPosition(pos);
}

void Object::setLocaleScalation(qreal scale)
{
    dumpGlobaleTransformationCache();
    ((TransformProperty*) _properties["localeMatrix"])->setScalation(scale);
}

void Object::setLocaleRotation(qreal r)
{
    dumpGlobaleTransformationCache();
    ((TransformProperty*) _properties["localeMatrix"])->setRotation(r);
}

QPointF Object::localePosition() const
{
    return MathUtility::translation(localeTransform());
}

qreal Object::localeScalation() const
{
    return MathUtility::scalation(localeTransform());
}

qreal Object::localeRotation() const
{
    return MathUtility::rotation(localeTransform());
}

QTransform Object::globaleTransform()
{
    if (!_globaleTransformUpToDate) {
        QTransform glob = localeTransform();
        if (_parent) {
            glob = glob * _parent->globaleTransform();
        }
        _globaleTransformCache = glob;
        _globaleTransformUpToDate = true;
        return glob;
    }
    return _globaleTransformCache;
}

QTransform Object::globaleTransformWithoutRoot() const
{
    QTransform glob;
    const Object* o = this;
    while (o && !o->isRoot()) {
        glob = glob * o->localeTransform();
        o = o->treeParent();
    }
    return glob;
}

QTransform Object::globaleTransformInverted()
{
    if (!_globaleTransformInvertedUpToDate) {
        _globaleTransformInvertedCache = globaleTransform().inverted();
        _globaleTransformInvertedUpToDate = true;
    }
    return _globaleTransformInvertedCache;
}

void Object::setGlobaleTransform(QTransform t)
{
    if (_parent) {
        setLocaleTransform(MathUtility::mult(t, _parent->globaleTransformInverted()));
    } else {
        setLocaleTransform(t);
    }
    _globaleTransformCache = t;
    _globaleTransformUpToDate = true;
}

void Object::setGlobalePosition(QPointF pos, Mode mode)
{
    QList<QTransform> transforms;
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            transforms.append(child->globaleTransform());
        }
    }
    QTransform t = globaleTransform();
    t.setMatrix(t.m11(), t.m12(), t.m13(), t.m21(), t.m22(), t.m23(), pos.x(), pos.y(), t.m33());
    setGlobaleTransform(t);
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            child->setGlobaleTransform(transforms.takeFirst());
        }
    }
    _globaleTransformCache = t;
    _globaleTransformUpToDate = true;
}

QPointF Object::globalePosition()
{
    return MathUtility::translation(globaleTransform());
}

void Object::setGlobaleScalation(qreal scale, Mode mode)
{
    QList<QTransform> transforms;
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            transforms.append(child->globaleTransform());
        }
    }
    QTransform t = globaleTransform();
    qreal s = scale / MathUtility::scalation(t);
    t.setMatrix(t.m11()*s, t.m12()*s, t.m13(),
                t.m21()*s, t.m22()*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setGlobaleTransform(t);
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            child->setGlobaleTransform(transforms.takeFirst());
        }
    }
    _globaleTransformCache = t;
    _globaleTransformUpToDate = true;
}

void Object::setGlobaleScalation(qreal scale, Mode mode, QPointF center)
{
    //Calculate rotation and radius relative to center
    QPointF position_center = globalePosition() - center;
    qreal radius = MathUtility::dist(position_center);
    qreal rotation = qAtan2(position_center.y(), position_center.x());
    qreal newRadius = radius * scale / globaleScalation();
    setGlobaleScalation(scale, mode);
    setGlobalePosition(center + QPointF(qCos(rotation) * newRadius, qSin(rotation) * newRadius), ObjectMode);
}

qreal Object::globaleScalation()
{
    return MathUtility::scalation(globaleTransform());
}

void Object::setGlobaleRotation(qreal r, Mode mode, QPointF center)
{
    //Calculate rotation and radius relative to center
    QPointF position_center = globalePosition() - center;
    qreal radius = MathUtility::dist(position_center);
    qreal rotation = qAtan2(position_center.y(), position_center.x());

    qreal newRotation = rotation + (globaleRotation()-r);

    setGlobaleRotation(r, mode);
    setGlobalePosition(center + QPointF(qCos(newRotation) * radius, qSin(newRotation) * radius), ObjectMode);
}

void Object::setGlobaleRotation(qreal r, Mode mode)
{
    QList<QTransform> transforms;
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            transforms.append(child->globaleTransform());
        }
    }
    QTransform t = globaleTransform();
    qreal s = MathUtility::scalation(t);
    t.setMatrix(qCos(r)*s, -qSin(r)*s, t.m13(),
                qSin(r)*s,  qCos(r)*s, t.m23(),
                t.m31(), t.m32(), t.m33());
    setGlobaleTransform(t);
    if (mode == AxesMode) {
        foreach (Object* child, _children) {
            child->setGlobaleTransform(transforms.takeFirst());
        }
    }
    _globaleTransformCache = t;
    _globaleTransformUpToDate = true;
}

qreal Object::globaleRotation()
{
    return MathUtility::rotation(globaleTransform());
}

void Object::setSelected(bool selected)
{
    if (_isSelected == selected) return;

    _isSelected = selected;
    if (_project) {
        _project->emitSelectionChanged();
    }
}

void Object::setExpanded(bool expanded)
{
    _isExpanded = expanded;
}

QString Object::toTikz() const
{
    return QString();
}

void Object::paint(QPainter &p, bool applyStyle, bool render)
{
    for (Tag* tag : _tags) {
        if (tag->type() != "StyleTag" || applyStyle)
            tag->exec();
    }


    if (!render && (isSelected() || !treeParent() || treeParent()->isSelected() || treeParent()->selectedDirectChildren().size() > 0)) {
        QPen oldPen = p.pen();
        p.save();
        QPen cosmeticPen;
        cosmeticPen.setCosmetic(true);
        cosmeticPen.setWidth(4);
        p.setPen(cosmeticPen);
        p.drawPoint(0,0);
        p.restore();
        p.setPen(oldPen);
    }

    if (valid()) {
        customDraw(p);
    }
    if (drawChildren())  {
        foreach (Object* o, _children) {
            p.save();
            p.setTransform(o->localeTransform(), true);
            o->paint(p, render);
            p.restore();
        }
    }
}

QList<Object*> Object::selectedObjects()     //Returns all selected Objects.
{
    QList<Object*> children;
    if (isSelected()) children.append(this);
    foreach (Object* child, _children) {
        children.append(child->selectedObjects());
    }
    return children;
}

QList<Object*> Object::subjects()                     //Returns all objects.
{
    QList<Object*> children;
    children.append(this);
    foreach (Object* child, _children) {
        children.append(child->subjects());
    }
    return children;
}

QList<Object*> Object::selectedParents()     //Returns selected Objects, but never a child of a selected Object.
{
    QList<Object*> children;
    if (isSelected()) {
        children.append(this);
    } else {
        foreach (Object* child, _children) {
            children.append(child->selectedParents());
        }
    }
    return children;
}

QList<Object*> Object::selectedChildren()    //Returns selected Objects and every children of a selected Object.
{
    QList<Object*> children;
    if (isSelected()) {
        children.append(this);
        foreach (Object* child, _children) {
            children.append(child->subjects());
        }
    } else {
        foreach (Object* child, _children) {
            children.append(child->selectedChildren());
        }
    }
    return children;
}

QList<Object*> Object::directChildren() const
{
    return _children;
}

bool Object::isRoot() const
{
    return project()->root() == this;
}

void Object::serialize(QDataStream &out)
{
    out << type() << (quint64) _children.size() << id() << _properties << _tags << (quint8) isSelected() << (quint8) isExpanded();
    foreach (Object* child, _children) {
        child->serialize(out);
    }
}

Object* Object::deserialize(QDataStream &in, Project* project, bool assertId, bool recursive, quint64 *childrenCount_)
{
    Object* o;
    quint64 id, childrenCount;
    QString className;
    QMap<QString, Property*> props;
    QList<Tag*> tags;
    quint8 isSelected, isExpanded;

    in >> className;
    o = Object::createInstance(className, project);

    Tag::AUX_OWNER_BUFFER = o;
    in >> childrenCount >> id >> props >> tags >> isSelected >> isExpanded;
    Tag::AUX_OWNER_BUFFER = 0;

    if (o->id() != id) {
        project->freeId(o->id());
        project->reserveId(o, id);
        if (assertId)
            Q_ASSERT(o->id() == id);
    }

    o->setProperties(props);
    o->setTags(tags);
    o->setSelected((bool) isSelected);
    o->setExpanded((bool) isExpanded);

    if (recursive) {
        for (quint64 i = 0; i < childrenCount; i++) {
            Object* child = deserialize(in, project, assertId);
            child->setTreeParent(o, false);
        }
    }
    if (childrenCount_ != 0) {
        *childrenCount_ = childrenCount;
    }
    o->updatePropertiesVisibility();
    o->polish();
    return o;
}

void Object::polish()
{
    connectPropertyTriggers();
    _project->emitStructureChanged();
}

QPointF Object::mapToGlobal(QPointF point)
{
    return globaleTransform().map(point);
}

QPointF Object::mapToLocal(QPointF point)
{
    return globaleTransform().inverted().map(point);
}

bool Object::hasProperty(QString role) const
{
    return (property(role) != 0);
}

void Object::setProperties(QMap<QString, Property *> props)
{
    foreach (QString key, _properties.keys()) {
        delete _properties[key];
    }
    _properties.clear();

    foreach (QString key, props.keys()) {
        props[key]->setProject(project());
        addProperty(key, props[key]);
    }
}

void Object::setTags(QList<Tag *> tags)
{
    foreach (Tag* tag, _tags) {
        delete tag;
    }
    _tags.clear();

    for (Tag* tag : tags) {
        if (hasTag(tag)) continue;
        _tags.append(tag);
        //tag->setOwner(this); //TODO check
    }
    _project->emitStructureChanged();
}

bool Object::hasTag(QString className) const
{
    foreach (Tag* t, _tags) {
        if (t->type() == className) return true;
    }
    return false;
}

bool Object::hasTag(Tag* tag) const
{
    return hasTag(tag->type());
}

void Object::newTag(QString tagName)
{
    if (hasTag(tagName)) return;

    Tag* tag = Tag::createInstance(this, tagName);
    _tags.append(tag);
    _project->emitStructureChanged();
}

void Object::deleteTag(Tag* tag)
{
    if (_tags.removeOne(tag)) {
        tag->deleteLater();
        project()->emitStructureChanged();
    }
}

Tag* Object::tag(QString className)
{
    foreach (Tag* tag, _tags) {
        if (tag->type() == className) {
            return tag;
        }
    }
    return 0;
}

void Object::applyStyleOptions(QPainter &p)
{
    StyleTag* sp;
    StyleTag fallBack;

    Object* object = this;
    while (!object->hasTag("StyleTag") && object->treeParent()) {
        object = object->treeParent();
    }
    if (object->hasTag("StyleTag")) {
        sp = ((StyleTag*) object->tag("StyleTag"));
    } else {
        sp = &fallBack;
    }

    QPen pen;
    pen.setColor(sp->drawColor());
    pen.setWidth(sp->width());
    pen.setStyle(sp->penStyle());
    p.setPen(pen);

    QBrush brush;
    QMatrix m = sp->isGlobal() ? object->globaleTransformInverted().toAffine() : QMatrix();
    brush.setMatrix(m * sp->transform());

    brush.setColor(sp->fillColor());
    brush.setStyle(sp->brushStyle());
    p.setBrush(brush);
}

QList<Object*> Object::selectedDirectChildren() const
{
    QList<Object*> selection;
    for (Object* o : directChildren()) {
        if (o->isSelected()) {
            selection.append(o);
        }
    }
    return selection;
}
