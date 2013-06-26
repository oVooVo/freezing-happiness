#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) :
    QPlainTextEdit(parent)
{
    installEventFilter(this);
}

bool TextEdit::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && (
                e->type() == QEvent::FocusOut))
        emit editingFinished();
    return QPlainTextEdit::eventFilter(o, e);
}
