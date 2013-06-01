#ifndef OBJECTEDIT_H
#define OBJECTEDIT_H

#include <QLineEdit>
#include "BackEnd/Objects/object.h"
#include <BackEnd/project.h>

class ObjectEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit ObjectEdit(QWidget* parent = 0);
    void setProject(Project* project);
    quint64 id() const { return _id; }
    bool isEmpty() { return _empty; }
    Object* object();

public slots:
    void setId(quint64 id);
    void clear();

protected:
    void dropEvent(QDropEvent *);
    void mousePressEvent(QMouseEvent *e);

signals:
    void statusChanged();

private slots:
    void onTextEditFinished();
    void objectMightBeDeleted();

private:
    Project* _project;
    quint64 _id;
    bool _empty;

    Object* _object;
    bool _objectValid;

    bool _wasEdited;
};

#endif // OBJECTEDIT_H
