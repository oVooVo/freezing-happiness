#include "combobox.h"
#include <QFocusEvent>
#include <QEvent>

ComboBox::ComboBox(QWidget *parent) :
    QComboBox(parent)
{
    installEventFilter(this);
}


void ComboBox::focusOutEvent(QFocusEvent *e)
{
    qDebug() << "focus Out event";
    //TODO emit lostFocus();    //is called to often, e.g. on click on ComboBox
    QComboBox::focusOutEvent(e);
}
