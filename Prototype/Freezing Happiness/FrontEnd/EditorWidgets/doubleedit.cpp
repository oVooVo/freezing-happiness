#include "doubleedit.h"
#include <QDebug>
#include <QLineEdit>
#include <QApplication>

DoubleEdit::DoubleEdit(QWidget *parent) :
    QDoubleSpinBox(parent)
{
}

void DoubleEdit::mouseMoveEvent(QMouseEvent *event)
{
    if (_pos.y() < 0) return;
    int diff = _pos.y() - event->pos().y();

    QCursor::setPos(mapToGlobal(_pos));
    setValue(value() + diff * singleStep());
    emit editingFinished();
}

void DoubleEdit::mouseReleaseEvent(QMouseEvent *event)
{
    _pos.setY(-1);
    QApplication::restoreOverrideCursor();
}

void DoubleEdit::mousePressEvent(QMouseEvent *event)
{
    _pos = event->pos();
    QApplication::setOverrideCursor(Qt::BlankCursor);
}
