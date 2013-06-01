#include "objecttree.h"
#include <QPaintEvent>
#include <QPainter>
#include <QDebug>
#include <QDropEvent>
#include <QMimeData>
#include <QByteArray>
#include <QTimer>

ObjectTree::ObjectTree(QWidget *parent) : QTreeWidget(parent)
{
    _project = 0;
    _doubleClickedObject = 0;
    setDefaultDropAction(Qt::MoveAction);
    setDragDropMode(QAbstractItemView::DragDrop);
    setEditTriggers(QAbstractItemView::DoubleClicked);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    installEventFilter(this);
    setHeaderHidden(true);
    connect(this, SIGNAL(clicked(QModelIndex)), this, SLOT(selectionChanged()));
}

void ObjectTree::setProject(Project *project)
{
    _project = project;
    if (_project) {
        connect(_project, SIGNAL(structureChanged()), this, SLOT(updateStructure()));
        connect(_project, SIGNAL(objectChanged(Object*)), this, SLOT(updateObject(Object*)));
        connect(_project, SIGNAL(selectionChanged()), this, SLOT(updateSelection()));
        updateStructure();
        updateSelection();
    }
}

void ObjectTree::updateStructure()
{
    clear();

    _objectsMap.clear();
    _itemsMap.clear();
    if (_project)
        addTopLevelItem(treeWidgetItemFromObject(_project->root()));

    foreach (QTreeWidgetItem* item, _objectsMap.keys()) {
        item->setExpanded(_expanded.value(_objectsMap[item], true));
    }
}

void ObjectTree::updateObject(Object *o)
{
    QTreeWidgetItem* item = _itemsMap[o];
    if (!item) return;

    item->setText(0, o->name());
}

QTreeWidgetItem* ObjectTree::treeWidgetItemFromObject(Object *o)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << o->name());
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
            this, SLOT(insertExpandInformation(QTreeWidgetItem*)));
    connect(this, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
            this, SLOT(insertExpandInformation(QTreeWidgetItem*)));

    _objectsMap.insert(item, o);
    _itemsMap.insert(o, item);

    foreach (Object* child, o->directChildren()) {
        item->addChild(treeWidgetItemFromObject(child));
    }

    return item;
}

void ObjectTree::dropEvent(QDropEvent *event)
{
    _project->createNewUndoRecord();
    _project->blockSignals(true);
    QTreeWidgetItem* parent = itemAt(event->pos());
    if (event->dropAction() == Qt::MoveAction) {
        if (!selectedItems().isEmpty() && parent) {
            int height = visualItemRect(itemAt(event->pos())).height();
            int pos = event->pos().y() % height;
            if (pos <= 2) {
                foreach (QTreeWidgetItem* item, selectedItems())
                    _objectsMap[item]->moveAbove(_objectsMap[parent]);
            } else if (pos >= height - 2) {
                foreach (QTreeWidgetItem* item, selectedItems())
                    _objectsMap[item]->moveBelow(_objectsMap[parent]);
            } else {
                foreach (QTreeWidgetItem* item, selectedItems())
                    _objectsMap[item]->setTreeParent(_objectsMap[parent]);
            }
        }
    }
    _project->blockSignals(false);
    _project->emitStructureChanged();
    //QTreeWidget::dropEvent(event);
}

void ObjectTree::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        _leftMouseDownPosition = event->pos();
    }
    QTreeWidget::mousePressEvent(event);
}

void ObjectTree::insertExpandInformation(QTreeWidgetItem *item)
{
    _expanded.insert(_objectsMap[item], item->isExpanded());
}

void ObjectTree::mouseDoubleClickEvent(QMouseEvent *event)
{
    _doubleClickedObject = itemAt(event->pos());
    QTreeWidget::mouseDoubleClickEvent(event);
}

bool ObjectTree::eventFilter(QObject *o, QEvent *e)
{
    if (o == this && e->type() == 207) {
        if (_objectsMap[_doubleClickedObject])
            _objectsMap[_doubleClickedObject]->setName(_doubleClickedObject->text(0));
    }
    return QTreeWidget::eventFilter(o, e);
}

void ObjectTree::selectionChanged()
{
    if (project()) {
        project()->blockSignals(true);
        foreach (QTreeWidgetItem* item, _objectsMap.keys()) {
            if (_objectsMap[item])
                _objectsMap[item]->setSelected(item->isSelected());
        }
        project()->blockSignals(false);
        project()->emitSelectionChanged();
    }
}

void ObjectTree::updateSelection()
{
    blockSignals(true);
    foreach (QTreeWidgetItem* item, _objectsMap.keys()) {
        if (_objectsMap[item])
            item->setSelected(_objectsMap[item]->isSelected());
    }
    blockSignals(false);
}

void ObjectTree::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == 16777223 && _project) { //entf
        _project->createNewUndoRecord();
        foreach (Object* o, _project->selectedParents()) {
            if (o->deletable()) {
                _project->blockSignals(true);
                _project->freeId(o->id());
                delete o;
                _project->blockSignals(false);
            }
        }
        _project->emitStructureChanged();
    }
}

QMimeData* ObjectTree::mimeData(const QList<QTreeWidgetItem *> items) const
{
    QString text;
    foreach (QTreeWidgetItem* item, items) {
        Object* o = _objectsMap[item];
        if (o) {
            QString id = QString("%1,").arg(o->id());
            text.append(id);
        }
    }
    if (text.endsWith(',')) {
        text.truncate(text.size() - 1);
    }
    QMimeData* m = QTreeWidget::mimeData(items);
    m->setText(text);
    return m;
}
