#ifndef PROJECT_H
#define PROJECT_H

#include "BackEnd/Objects/object.h"
#include "BackEnd/renderoptions.h"

class Object;
class Project : public QObject
{
    Q_OBJECT
public:
    Project(Object* root = 0);
    ~Project();
    void init();
    Object* root() const { return _root; }
    QString visualize() const;
    bool setRoot(Object* root);     //ensure root has its final id!

    quint64 reserveId(Object* object, quint64 id = 0);
    void freeId(quint64 id);

    Object* getObject(quint64 id);
    Object* find(QString name);
    void emitObjectChanged(Object* object);
    void emitStructureChanged();
    void emitSelectionChanged();

    void clearSelection();
    QList<Object*> selectedObjects(); //Returns all selected Objects.
    QList<Object*> objects();         //Returns all objects.
    QList<Object*> selectedParents(); //Returns selected Objects, but never a child of a selected Object.
    QList<Object*> selectedChildren();//Returns selected Objects and every children of a selected Object.
    Object* selectionsParent();       //Returns the first Object, that is (indirect) parent of all selected.
                                            //Note that the returned objects may be not selected!
    void setRecordHistory(bool on) { _recordHistory = on; }
    bool isHistoryRecorded() const { return _recordHistory; }

    void addLogItem(QString item);
    QString log() const { return _log; }

    QMap<QString, QList<Property*> > activeProperties();
    QMap<QString, QList<Tag*> > activeTags();

    void paint(QPainter &p);
    bool addPoint(QPointF globalePosition);
    bool showRenderFrame;
    RenderOptions renderOptions() const { return _renderOptions; }

public slots:
    void createNewUndoRecord(bool force = false);
    void undo() { emit undoRequest(); }
    void redo() { emit redoRequest(); }
    void duplicateSelected();
    QImage render();
    void showRenderOptions();

signals:
    void selectionChanged();
    void structureChanged();
    void objectChanged(Object* object);
    void newUndoRecordRequest();
    void undoRequest();
    void redoRequest();

private:
    Object* _root;
    quint64 _maxId;
    QList<quint64> _freeIds;
    QMap<quint64, Object*> _objects;

    bool _selectedObjectsCacheUptodate;
    bool _objectsCacheUptodate;
    bool _selectedParentsCacheUptodate;
    bool _selectedChildrenCacheUptodate;
    bool _selectionsParentCacheUptodate;
    QList<Object*> _selectedObjectsCache;
    QList<Object*> _objectsCache;
    QList<Object*> _selectedParentsCache;
    QList<Object*> _selectedChildrenCache;
    Object* _selectionsParentCache;

    bool _recordHistory;
    QString _log;

    RenderOptions _renderOptions;
    Object* cameraObject(); //cache?

    //Should be used only by operator>> and operator<< implementation!
private:
    void setLog(QString log) { _log = log; }

    friend QDataStream& operator<<(QDataStream & out, Project* p);
    friend QDataStream& operator>>(QDataStream & in, Project* &p);
};

QDataStream& operator<<(QDataStream & out, Project* p);
QDataStream& operator>>(QDataStream & in, Project* &p);

#endif // PROJECT_H
