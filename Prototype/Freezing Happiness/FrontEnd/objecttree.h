#ifndef OBJECTTREE_H
#define OBJECTTREE_H

#include <QWidget>
#include <BackEnd/project.h>
#include <QTreeWidgetItem>
#include <QDrag>

class ObjectTree : public QTreeWidget
{
    Q_OBJECT

public:
    explicit ObjectTree(QWidget *parent = 0);
    Project* project() const { return _project; }
    void setProject(Project* project);

private slots:
    void updateStructure();
    void updateObject(Object* o);
    void updateSelection();
    void selectionChanged();

protected:
    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    bool eventFilter(QObject *o, QEvent *e);
    QMimeData* mimeData(const QList<QTreeWidgetItem *> items) const;

private:
    Project* _project;
    QTreeWidgetItem* _doubleClickedObject;
    QTreeWidgetItem* _rightClickedObject;
    QPoint _leftMouseDownPosition;
    QTreeWidgetItem* treeWidgetItemFromObject(Object* o);
    QMap<QTreeWidgetItem*, Object*> _objectsMap;
    QMap<Object*, QTreeWidgetItem*> _itemsMap;
    void setUpContextMenu();
    void clearSelection();



};

#endif // OBJECTTREE_H
