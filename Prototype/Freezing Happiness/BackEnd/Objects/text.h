#ifndef TEXT_H
#define TEXT_H

#include "pathobject.h"

class Text : public PathObject
{
    Q_OBJECT
public:
    Text(Project* project, QString name = tr("Text"));

protected:
    void connectPropertyTriggers();
    void updatePath();

private:
    REGISTER_DECL_OBJECTTYPE(Text);

};

#endif // TEXT_H
