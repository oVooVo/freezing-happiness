#include "integeredit.h"
#include <QDebug>
#include <QLineEdit>
#include <QApplication>

IntegerEdit::IntegerEdit(QWidget *parent) :
    QSpinBox(parent)
{
}

void IntegerEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_pos.y() < 0) return;
    int diff = _pos.y() - event->pos().y();

    QCursor::setPos(mapToGlobal(_pos));
    setValue(value() + diff * singleStep());
    emit editingFinished();
}

void IntegerEdit::mouseReleaseEvent(QMouseEvent *)
{
    _pos.setY(-1);
    QApplication::restoreOverrideCursor();
}

void IntegerEdit::mousePressEvent(QMouseEvent *event)
{
    _pos = event->pos();
    QApplication::setOverrideCursor(Qt::BlankCursor);
}
