#ifndef CAMERA_H
#define CAMERA_H

#include "BackEnd/Properties/boolproperty.h"
#include "object.h"

class Camera : public Object
{
    Q_OBJECT
public:
    Camera(Project *project, QString name = tr("Camera"));
    bool isActive() const { return ((BoolProperty*) properties()["active"])->value(); }

private slots:
    void deactivateOtherCams();

private:
    REGISTER_DECL_OBJECTTYPE(Camera);
    void setActive(bool active);
};

#endif // CAMERA_H
