#ifndef POINTTAG_H
#define POINTTAG_H

#include "tag.h"

class PointTag : public Tag
{
    Q_OBJECT
public:
    PointTag(Object* owner, QByteArray* data = 0);
    QByteArray toByteArray() const;
    quint64 index() const { return _index; }
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);

public slots:
    void setIndex(quint64 index);

protected:
    void addProperties();

private:
    REGISTER_DECL_TAGTYPE(PointTag);
    quint64 _index;
};

#endif // POINTTAG_H
