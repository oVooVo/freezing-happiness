#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QPlainTextEdit>

class TextEdit : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = 0);

protected:
    bool eventFilter(QObject *o, QEvent *e);
    
signals:
    void editingFinished();
    
};

#endif // TEXTEDIT_H
