#include "text.h"
#include "BackEnd/Properties/propertystring.h"

REGISTER_DEFN_OBJECTTYPE(Text);

Text::Text(Project *project, QString name) : Object(project, name, false)
{
    addProperty("text", new PropertyString("Text", "Text", "", false, false));
    polish();
}
