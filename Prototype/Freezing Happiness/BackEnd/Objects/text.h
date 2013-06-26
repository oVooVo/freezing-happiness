#ifndef TEXT_H
#define TEXT_H

#include "object.h"

class Text : public Object
{
    Q_OBJECT
public:
    Text(Project* project, QString name = tr("Text"));

private:
    REGISTER_DECL_OBJECTTYPE(Text);

};

#endif // TEXT_H
