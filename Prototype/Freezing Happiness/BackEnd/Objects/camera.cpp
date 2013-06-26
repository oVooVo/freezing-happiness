#include "camera.h"
#include "BackEnd/project.h"

REGISTER_DEFN_OBJECTTYPE(Camera);

Camera::Camera(Project* project, QString name) : Object(project, name, false)
{
    BoolProperty* bp = new BoolProperty("Camera", "Is Active", false);
    addProperty("active", bp);
    polish();
}

void Camera::deactivateOtherCams()
{
    if (!project()) return;
    for (Object* o : project()->objects()) {
        if (o->type() == type() && o != this)  {
            ((Camera*) o)->setActive(false);
        }
    }
}

void Camera::connectPropertyTriggers()
{
    connect(((BoolProperty*) properties()["active"]), &BoolProperty::valueChanged, this, &Camera::deactivateOtherCams);
}

void Camera::setActive(bool active)
{
    project()->blockSignals(true);
    project()->setRecordHistory(false);
    properties()["active"]->blockSignals(true);
    ((BoolProperty*) properties()["active"])->setValue(active);
    properties()["active"]->blockSignals(false);
    project()->blockSignals(false);
    project()->setRecordHistory(true);
}
