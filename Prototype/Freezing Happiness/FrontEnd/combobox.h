#ifndef COMBOBOX_H
#define COMBOBOX_H

#include <QComboBox>
#include <QDebug>

class ComboBox : public QComboBox
{
    Q_OBJECT
public:
    ComboBox(QWidget* parent = 0);

protected:
    void focusOutEvent(QFocusEvent *e);
signals:
    void lostFocus();
};

#endif // COMBOBOX_H
