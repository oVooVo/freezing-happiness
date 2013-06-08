#ifndef PROJECTCONTAINER_H
#define PROJECTCONTAINER_H

#include <QList>
#include <QByteArray>
#include <QDataStream>
#include "project.h"

class ProjectContainer : public QObject
{
    Q_OBJECT
public:
    ProjectContainer(Project* project);
    ~ProjectContainer();
    Project* project() const { return _project; }
    void setProject(Project *project);
    bool canUndo() { return true; }// _history.size() > 1; }
    bool canRedo() { return false; }// !_future.isEmpty(); }

public slots:
    void undo();
    void redo();

signals:
    void projectChanged(Project*);
    void canUndoChanged(bool);
    void canRedoChanged(bool);

private slots:
    void record();
private:
    QList<QByteArray> _history;
    QByteArray _buffer;

    Project* _project;
    void newHistoryRecord();
    //void newFutureRecord();

};

#endif // PROJECTCONTAINER_H
