#include "text.h"
#include "BackEnd/Properties/propertystring.h"
#include "BackEnd/Properties/fontproperty.h"

REGISTER_DEFN_OBJECTTYPE(Text);

Text::Text(Project *project, QString name) : PathObject(project, name)
{
    addProperty("text", new PropertyString(tr("Text"), tr("Text"), "", false));
    addProperty("font", new FontProperty(tr("Text"), tr("Font")));
    polish();
}

void Text::connectPropertyTriggers()
{
    connect(properties()["font"], &Property::valueChanged, [=]() {
        ((PropertyString*) properties()["text"])->setFont(((FontProperty*) properties()["font"])->font());
    });
}

void Text::updatePath()
{
    _path = QPainterPath();
    _path.addText(0,0, ((FontProperty*) property("font"))->font(), ((PropertyString*) property("text"))->string());
}

