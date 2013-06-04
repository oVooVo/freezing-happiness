#include "constrainttag.h"
#include <QDebug>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QCheckBox>
#include "FrontEnd/objectedit.h"

REGISTER_DEFN_TAGTYPE(ConstraintTag);

ConstraintTag::ConstraintTag(QByteArray* data)
{
    if (data == 0) {
        _positionMode = Mode::ignore;
        _rotationMode = Mode::ignore;
        _scalationMode = Mode::ignore;
        _hasPosId = false;
        _hasRotId = false;
        _hasScaleId = false;
        _affectX = true;
        _affectY = true;
    } else {
        QDataStream stream(data, QIODevice::ReadOnly);
        QString className;
        quint8 posMode, affectX, affectY, hasPosId, rotMode, hasRotId, scaleMode, hasScaleId;
        quint64 posId, rotId, scaleId;
        stream >> className
               >> posMode >> affectX >> affectY >> hasPosId >> posId
               >> rotMode >> hasRotId >> rotId
               >> scaleMode >> hasScaleId >> scaleId;
        _positionMode = (Mode) posMode;
        _rotationMode = (Mode) rotMode;
        _scalationMode = (Mode) scaleMode;
        _hasPosId = (bool) hasPosId;
        _posId = posId;
        _hasRotId = (bool) hasRotId;
        _rotId = rotId;
        _hasScaleId = (bool) hasScaleId;
        _scaleId = scaleId;
        _affectX = (bool) affectX;
        _affectY = (bool) affectY;
    }
}

QByteArray ConstraintTag::toByteArray() const
{
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << type()
           << (quint8) _positionMode << (quint8) _affectX << (quint8) _affectY << (quint8) _hasPosId << (quint64) _posId
           << (quint8) _rotationMode << (quint8) _hasRotId << (quint64) _rotId
           << (quint8) _scalationMode << (quint8) _hasScaleId << (quint64) _scaleId;
    return data;
}

QWidget* ConstraintTag::createWidget(QList<Tag *> tags, QWidget *parent)
{
    // === Create Widget ===
    QWidget* base = new QWidget(parent);
    QHBoxLayout* horLayout = new QHBoxLayout();
    QStringList comboboxItems = QStringList() << tr("ignore") << tr("local") << tr("global");

    QVBoxLayout* posLayout = new QVBoxLayout();
    posLayout->setSpacing(0);
    posLayout->setContentsMargins(0,0,0,0);
    QComboBox* posComboBox = new QComboBox(base);
    posComboBox->addItems(comboboxItems);
    posLayout->addWidget(posComboBox);
    QCheckBox* affectXCB = new QCheckBox("x", base);
    QCheckBox* affectYCB = new QCheckBox("y", base);
    QHBoxLayout* l = new QHBoxLayout();
    l->setContentsMargins(0,0,0,0);
    l->setSpacing(0);
    l->addWidget(affectXCB);
    l->addWidget(affectYCB);
    posLayout->addLayout(l);
    posLayout->addStretch();
    ObjectEdit* posObject = new ObjectEdit(base);
    posObject->setProject(tags.first()->owner()->project());
    posLayout->addWidget(posObject);

    QVBoxLayout* rotLayout = new QVBoxLayout();
    rotLayout->setSpacing(0);
    rotLayout->setContentsMargins(0,0,0,0);
    QComboBox* rotComboBox = new QComboBox(base);
    rotComboBox->addItems(comboboxItems);
    rotLayout->addWidget(rotComboBox);
    rotLayout->addStretch();
    ObjectEdit* rotObject = new ObjectEdit(base);
    rotObject->setProject(tags.first()->owner()->project());
    rotLayout->addWidget(rotObject);

    QVBoxLayout* scaleLayout = new QVBoxLayout();
    scaleLayout->setSpacing(0);
    scaleLayout->setContentsMargins(0,0,0,0);
    QComboBox* scaleComboBox = new QComboBox(base);
    scaleComboBox->addItems(comboboxItems);
    scaleLayout->addWidget(scaleComboBox);
    scaleLayout->addStretch();
    ObjectEdit* scaleObject = new ObjectEdit(base);
    scaleObject->setProject(tags.first()->owner()->project());
    scaleLayout->addWidget(scaleObject);

    horLayout->addLayout(posLayout);
    horLayout->addLayout(rotLayout);
    horLayout->addLayout(scaleLayout);
    base->setLayout(horLayout);
    // === end create Widget ===

    // === define update function ===
    auto update = [=]() {

        posComboBox->blockSignals(true);
        posObject->blockSignals(true);
        affectXCB->blockSignals(true);
        affectYCB->blockSignals(true);
        rotComboBox->blockSignals(true);
        rotObject->blockSignals(true);
        scaleComboBox->blockSignals(true);
        scaleObject->blockSignals(true);

        bool pModeDifference = false;
        bool affectXDifference = false;
        bool affectYDifference = false;
        bool pIdDifference = false;
        bool rModeDifference = false;
        bool rIdDifference = false;
        bool sModeDifference = false;
        bool sIdDifference = false;

        Mode pMode = ((ConstraintTag*) tags.first())->_positionMode;
        bool affectX = ((ConstraintTag*) tags.first())->_affectX;
        bool affectY = ((ConstraintTag*) tags.first())->_affectY;
        quint64 pId = ((ConstraintTag*) tags.first())->_posId;
        bool hasPId = ((ConstraintTag*) tags.first())->_hasPosId;
        Mode rMode = ((ConstraintTag*) tags.first())->_rotationMode;
        quint64 rId = ((ConstraintTag*) tags.first())->_rotId;
        bool hasRId = ((ConstraintTag*) tags.first())->_hasRotId;
        Mode sMode = ((ConstraintTag*) tags.first())->_scalationMode;
        quint64 sId = ((ConstraintTag*) tags.first())->_scaleId;
        bool hasSId = ((ConstraintTag*) tags.first())->_hasScaleId;


        foreach (Tag* t, tags) {
            ConstraintTag* ct = (ConstraintTag*) t;
            if (ct->_positionMode != pMode) pModeDifference = true;
            if (ct->_affectX != affectX) affectXDifference = true;
            if (ct->_affectY != affectY) affectYDifference = true;
            if ((ct->_hasPosId != hasPId)
                    || (ct->_hasPosId && hasPId && ct->_posId != pId)) pIdDifference = true;
            if (ct->_rotationMode != rMode) rModeDifference = true;
            if ((ct->_hasRotId != hasRId)
                    || (ct->_hasRotId && hasRId && ct->_rotId != rId)) rIdDifference = true;
            if (ct->_scalationMode != sMode) sModeDifference = true;
            if ((ct->_hasScaleId != hasSId)
                    || (ct->_hasScaleId && hasSId && ct->_scaleId != sId)) sIdDifference = true;
        }

        if (pModeDifference) posComboBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else posComboBox->setStyleSheet("");
        if (affectXDifference) affectXCB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else affectXCB->setStyleSheet("");
        if (affectYDifference) affectYCB->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else affectYCB->setStyleSheet("");
        if (pIdDifference) posObject->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else posObject->setStyleSheet("");
        posComboBox->setCurrentIndex(modeToIndex(pMode));
        affectXCB->setChecked(affectX);
        affectXCB->setEnabled(pMode != Mode::ignore);
        affectYCB->setChecked(affectY);
        affectYCB->setEnabled(pMode != Mode::ignore);
        posObject->setEnabled(pMode != Mode::ignore);
        if (hasPId)
            posObject->setId(pId);
        else
            posObject->clear();

        if (rModeDifference) rotComboBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else rotComboBox->setStyleSheet("");
        if (rIdDifference) rotObject->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else rotObject->setStyleSheet("");
        rotComboBox->setCurrentIndex(modeToIndex(rMode));
        rotObject->setEnabled(rMode != Mode::ignore);
        if (hasRId)
            rotObject->setId(rId);
        else
            rotObject->clear();
        if (sModeDifference) scaleComboBox->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else scaleComboBox->setStyleSheet("");
        if (sIdDifference) scaleObject->setStyleSheet(QString("background-color:%1").arg(Property::MULTIPLE_VALUES_COLOR));
        else scaleObject->setStyleSheet("");

        scaleComboBox->setCurrentIndex(modeToIndex(sMode));
        scaleObject->setEnabled(sMode != Mode::ignore);
        if (hasSId)
            scaleObject->setId(sId);
        else
            scaleObject->clear();

        posComboBox->blockSignals(false);
        posObject->blockSignals(false);
        affectXCB->blockSignals(false);
        affectYCB->blockSignals(false);
        rotComboBox->blockSignals(false);
        rotObject->blockSignals(false);
        scaleComboBox->blockSignals(false);
        scaleObject->blockSignals(false);
    };
    // === end define update function ===

    void (QComboBox:: *currentIndexChangedInt)(int) = &QComboBox::currentIndexChanged;

    foreach (Tag* tag, tags) {
        ConstraintTag* ct = ((ConstraintTag*) tag);
        connect(tag, &Tag::valueChanged, update);

        connect(posComboBox, currentIndexChangedInt, [=]() {
            ct->setPositionMode(intToMode(posComboBox->currentIndex()));
        });
        connect(affectXCB, &QCheckBox::clicked, [=]() {
            ct->setAffectX(affectXCB->isChecked());
        });
        connect(affectYCB, &QCheckBox::clicked, [=]() {
            ct->setAffectY(affectYCB->isChecked());
        });
        connect(posObject, &ObjectEdit::statusChanged, [=]() {
            ct->setPositionObject(posObject->id(), posObject->isEmpty());
        });
        connect(rotComboBox, currentIndexChangedInt, [=]() {
            ct->setRotationMode(intToMode(rotComboBox->currentIndex()));
        });
        connect(rotObject, &ObjectEdit::statusChanged, [=]() {
            ct->setRotationObject(rotObject->id(), rotObject->isEmpty());
        });
        connect(scaleComboBox, currentIndexChangedInt, [=]() {
            ct->setScalationMode(intToMode(scaleComboBox->currentIndex()));
        });
        connect(scaleObject, &ObjectEdit::statusChanged, [=]() {
            ct->setScalationObject(scaleObject->id(), scaleObject->isEmpty());
        });
    }
    update();
    return base;
}


void ConstraintTag::setPositionMode(Mode mode)
{
    if (_positionMode == mode) return;

    createUndoRecord();
    _positionMode = mode;
    emit valueChanged();
}

void ConstraintTag::setRotationMode(Mode mode)
{
    if (_rotationMode == mode) return;

    createUndoRecord();
    _rotationMode = mode;
    emit valueChanged();
}

void ConstraintTag::setScalationMode(Mode mode)
{
    if (_scalationMode == mode) return;

    createUndoRecord();
    _scalationMode = mode;
    emit valueChanged();
}

void ConstraintTag::setAffectX(bool on)
{
    if (_affectX == on) return;

    createUndoRecord();
    _affectX = on;
    emit valueChanged();
}

void ConstraintTag::setAffectY(bool on)
{
    if (_affectY == on) return;

    createUndoRecord();
    _affectY = on;
    emit valueChanged();
}

void ConstraintTag::setPositionObject(quint64 id, bool isEmpty)
{
    if (_posId == id && _hasPosId != isEmpty) return;

    createUndoRecord();
    _posId = id;
    _hasPosId = !isEmpty;

    emit valueChanged();
}

void ConstraintTag::setRotationObject(quint64 id, bool isEmpty)
{
    if (_rotId == id && _hasRotId != isEmpty) return;

    createUndoRecord();
    _rotId = id;
    _hasRotId = !isEmpty;
    emit valueChanged();
}

void ConstraintTag::setScalationObject(quint64 id, bool isEmpty)
{
    if (_scaleId == id && _hasScaleId != isEmpty) return;

    createUndoRecord();
    _scaleId = id;
    _hasScaleId = !isEmpty;
    emit valueChanged();
}

void ConstraintTag::exec(QPainter &p)
{
    Q_UNUSED(p)

    if (!owner()) return;

    if (_hasPosId && owner()->isAncestorOf(owner()->project()->getObject(_posId))) {
        qDebug() << "Cyclic dependency.";
    } else {
        affectPosition();
    }

    if (_hasRotId && owner()->isAncestorOf(owner()->project()->getObject(_rotId))) {
        qDebug() << "Cyclic dependency.";
    } else {
        affectRotation();
    }

    if (_hasScaleId && owner()->isAncestorOf(owner()->project()->getObject(_scaleId))) {
        qDebug() << "Cyclic dependency.";
    } else {
        affectScalation();
    }
}

void ConstraintTag::affectPosition()
{
    if (!_hasPosId || _positionMode == ignore || !(_affectX || _affectY) || !owner() || !owner()->project()) return;
    Object* sub = owner()->project()->getObject(_posId); //null if _posId is invalid.
    if (!sub) return;

    bool g = (_positionMode == global);
    QPointF newPos = g ? sub->globalePosition() : sub->localePosition();
    QPointF oldPos;
    if (!_affectX || !_affectY) {
        oldPos = g ? owner()->globalePosition() : owner()->localePosition();
    }
    if (!_affectX) {
        newPos.setX(oldPos.x());
    }
    if (!_affectY) {
        newPos.setY(oldPos.y());
    }
    if (g) {
        owner()->setGlobalePosition(newPos);
    } else {
        owner()->setLocalePosition(newPos);
    }
}

void ConstraintTag::affectRotation()
{
    if (!_hasRotId || _rotationMode == ignore || !owner() || !owner()->project()) return;
    Object* sub = owner()->project()->getObject(_rotId);
    if (!sub) return;

    bool g = (_positionMode == global);
    qreal rotation = g ? sub->globaleRotation() : sub->localeRotation();
    if (g) {
        owner()->setGlobaleRotation(rotation);
    } else {
        owner()->setLocaleRotation(rotation);
    }
}

void ConstraintTag::affectScalation()
{
    if (!_hasScaleId || _scalationMode == ignore || !owner() || !owner()->project()) return;
    Object* sub = owner()->project()->getObject(_rotId);
    if (!sub) return;

    bool g = (_scalationMode == global);
    qreal scalation = g ? sub->globaleScalation() : sub->localeScalation();
    if (g) {
        owner()->setGlobaleScalation(scalation);
    } else {
        owner()->setLocaleScalation(scalation);
    }
}




















