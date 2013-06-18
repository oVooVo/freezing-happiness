#ifndef POINTTAG_H
#define POINTTAG_H

#include "tag.h"

class PointTag : public Tag
{
    Q_OBJECT
public:
    PointTag(Object* owner, QByteArray* data = 0);
    QByteArray toByteArray() const;
    qint64 index() const;
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);

public slots:
    void setIndex(qint64 ind);

private:
    REGISTER_DECL_TAGTYPE(PointTag);
};

#endif // POINTTAG_H
