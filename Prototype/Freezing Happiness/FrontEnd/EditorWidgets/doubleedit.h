#ifndef DOUBLEEDIT_H
#define DOUBLEEDIT_H

#include <QDoubleSpinBox>
#include <QMouseEvent>

class DoubleEdit : public QDoubleSpinBox
{
    Q_OBJECT
public:
    explicit DoubleEdit(QWidget *parent = 0);

protected:
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *);

private:
    QPoint _pos;
};

#endif // DOUBLEEDIT_H
