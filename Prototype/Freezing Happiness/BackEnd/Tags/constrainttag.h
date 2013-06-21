#ifndef CONSTRAINTTAG_H
#define CONSTRAINTTAG_H

#include <QDebug>
#include "tag.h"

class ConstraintTag : public Tag
{
    Q_OBJECT
public:
    enum Mode { ignore, local, global };
    ConstraintTag(Object* owner, QByteArray* data);
    QByteArray toByteArray() const;
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);
    void exec();

public slots:
    void setPositionMode(Mode mode);
    void setRotationMode(Mode mode);
    void setScalationMode(Mode mode);
    void setAffectX(bool on);
    void setAffectY(bool on);
    void setPositionObject(quint64 id, bool isEmpty);
    void setRotationObject(quint64 id, bool isEmpty);
    void setScalationObject(quint64 id, bool isEmpty);

signals:
    void positionModeChanged(Mode);
    void affectXChanged(bool);
    void affectYChanged(bool);
    void positionIdChanged(quint64, bool);
    void rotationModeChanged(Mode);
    void rotationIdChanged(quint64, bool);
    void scalationModeChanged(Mode);
    void scalationIdChanged(quint64, bool);

private:
    REGISTER_DECL_TAGTYPE(ConstraintTag);

    Mode _positionMode;
    bool _affectX;
    bool _affectY;
    quint64 _posId;
    bool _hasPosId;

    Mode _rotationMode;
    quint64 _rotId;
    bool _hasRotId;

    Mode _scalationMode;
    quint64 _scaleId;
    bool _hasScaleId;

    static int modeToIndex(Mode m) { return (int) m; }
    static Mode intToMode(int i) { return (Mode) i; }

    void affectPosition();
    void affectRotation();
    void affectScalation();

};

#endif // CONSTRAINTTAG_H
