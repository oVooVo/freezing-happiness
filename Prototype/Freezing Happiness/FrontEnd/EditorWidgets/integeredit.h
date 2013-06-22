#ifndef INTEGEREDIT_H
#define INTEGEREDIT_H

#include <QSpinBox>
#include <QMouseEvent>

class IntegerEdit : public QSpinBox
{
    Q_OBJECT
public:
    explicit IntegerEdit(QWidget *parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    QPoint _pos;
};

#endif // INTEGEREDIT_H
