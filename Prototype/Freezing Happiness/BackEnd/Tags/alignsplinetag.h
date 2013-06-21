#ifndef ALIGNSPLINETAG_H
#define ALIGNSPLINETAG_H

#include "tag.h"

class AlignSplineTag : public Tag
{
    Q_OBJECT
public:
    AlignSplineTag(Object* owner, QByteArray *data = 0);
    void exec();
    static QWidget* createWidget(QList<Tag *> tags, QWidget *parent);

private:
    REGISTER_DECL_TAGTYPE(AlignSplineTag);
};

#endif // ALIGNSPLINETAG_H
