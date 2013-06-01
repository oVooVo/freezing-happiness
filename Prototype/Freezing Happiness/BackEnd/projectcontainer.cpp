#include "projectcontainer.h"
#include <QDebug>
#include <QApplication>

//TODO
// nach ladevorgang ist kein undo/redo möglich!!

ProjectContainer::ProjectContainer(Project *project)
{
    _project = project;
    connect(_project, SIGNAL(newUndoRecordRequest()), this, SLOT(record()));
    connect(_project, SIGNAL(undoRequest()), this, SLOT(undo()));
    connect(_project, SIGNAL(redoRequest()), this, SLOT(redo()));
}

ProjectContainer::~ProjectContainer()
{
    _project->deleteLater();
}

void ProjectContainer::record()
{
    if (!_project) return;
    _future.clear();
    emit canRedoChanged(false);
    newHistoryRecord();
}

void ProjectContainer::newHistoryRecord()
{
    if (!_project) return;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << _project;
    _history.append(data);
    if (_history.size() == 1) {
        emit canUndoChanged(true);
    }
}

void ProjectContainer::newFutureRecord()
{
    if (!_project) return;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream << _project;
    _future.append(data);
    if (_future.size() == 1) {
        emit canRedoChanged(true);
    }
}

void ProjectContainer::undo()
{
    if (!_project) return;
    if (!_history.isEmpty()) {
        newFutureRecord();

        disconnect(_project, SIGNAL(newUndoRecordRequest()), this, SLOT(record()));
        disconnect(_project, SIGNAL(undoRequest()), this, SLOT(undo()));
        disconnect(_project, SIGNAL(redoRequest()), this, SLOT(redo()));

        _project->deleteLater();

        QByteArray record = _history.takeLast();
        if (_history.isEmpty()) {
            emit canUndoChanged(false);
        }
        QDataStream stream(&record, QIODevice::ReadOnly);
        stream >> _project;

        connect(_project, SIGNAL(newUndoRecordRequest()), this, SLOT(record()));
        connect(_project, SIGNAL(undoRequest()), this, SLOT(undo()));
        connect(_project, SIGNAL(redoRequest()), this, SLOT(redo()));

        emit projectChanged(_project);
        _project->emitStructureChanged();
        _project->setRecordHistory(true);

    } else {
        _project->addLogItem("Try to undo although there is nothing to undo.");
    }
}

void ProjectContainer::redo()
{
    if (!_project) return;
    if (!_future.isEmpty()) {
        newHistoryRecord();

        disconnect(_project, SIGNAL(newUndoRecordRequest()), this, SLOT(record()));
        disconnect(_project, SIGNAL(undoRequest()), this, SLOT(undo()));
        disconnect(_project, SIGNAL(redoRequest()), this, SLOT(redo()));

        _project->deleteLater();

        QByteArray record = _future.takeLast();
        if (_future.isEmpty()) {
            emit canRedoChanged(false);
        }
        QDataStream stream(&record, QIODevice::ReadOnly);
        stream >> _project;

        connect(_project, SIGNAL(newUndoRecordRequest()), this, SLOT(record()));
        connect(_project, SIGNAL(undoRequest()), this, SLOT(undo()));
        connect(_project, SIGNAL(redoRequest()), this, SLOT(redo()));

        emit projectChanged(_project);
        _project->emitStructureChanged();
        _project->setRecordHistory(true);
    } else {
        _project->addLogItem("Try to redo although there is nothing to redo.");
    }
}
