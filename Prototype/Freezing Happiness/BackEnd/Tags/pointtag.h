#ifndef POINTTAG_H
#define POINTTAG_H

#include "tag.h"

class PointTag : public Tag
{
    Q_OBJECT
public:
    static const QStringList MODES;
    PointTag(Object* owner, QByteArray* data = 0);
    qint64 index() const;
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);


    QPointF ctrlA() const;
    QPointF ctrlB() const;
    QPointF globaleCtrlA();
    QPointF globaleCtrlB();
    void setGlobaleCtrlA(QPointF p);
    void setGlobaleCtrlB(QPointF p);
    QPointF ctrlAPosition() const;
    QPointF ctrlBPosition() const;

public slots:
    void setIndex(qint64 ind);

private:
    REGISTER_DECL_TAGTYPE(PointTag);
};

#endif // POINTTAG_H
