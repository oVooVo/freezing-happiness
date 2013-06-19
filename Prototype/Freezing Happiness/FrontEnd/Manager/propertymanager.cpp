#include "propertymanager.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>
#include <QTabBar>

PropertyManager::PropertyManager(QWidget *parent) :
    Manager(parent)
{
    _tabWidget = new QTabWidget(this);
    _objectsLabel = new QLabel(this);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(_objectsLabel);
    layout->addWidget(_tabWidget);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    setLayout(layout);
}

void PropertyManager::updateProperties()
{
    _tabWidget->clear();

    QMap<QString, QList<Property*> > properties = project()->activeProperties();
    QMap<QString, QList<Tag*> > tags = project()->activeTags();

    QMap<QString, QVBoxLayout*> propWidgets;
    QMap<QString, QVBoxLayout*> tagWidgets;



    foreach (QString name, properties.keys()) {
        if (properties[name].isEmpty()) continue;
        if (!properties[name].first()->isVisible) continue;

        QString category = properties[name].first()->category();
        QVBoxLayout* layout;
        if (propWidgets.contains(category)) {
            layout = propWidgets[category];
        } else {
            layout = new QVBoxLayout();
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(0);
        }
        layout->addWidget(Property::createWidget(properties[name], _tabWidget));
        propWidgets.insert(category, layout);
    }

    foreach (QString className, tags.keys()) {
        if (tags[className].isEmpty()) return;
        QVBoxLayout* layout;
        if (tagWidgets.contains(className)) {
            layout = tagWidgets[className];
        } else {
            layout = new QVBoxLayout();
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(0);
        }
        layout->addWidget(Tag::createWidget(tags[className], _tabWidget));
        tagWidgets.insert(className, layout);
    }

    QString objects;
    QString sep = ", ";
    foreach (Object* o, project()->selectedObjects()) {
        objects = objects.append(o->name()).append(sep);
    }
    objects.truncate(objects.size() - sep.size());

    QFontMetrics fm(_objectsLabel->font());
    bool tooLarge = fm.width(objects) > width();

    if (objects.isEmpty() || tooLarge) {
        _objectsLabel->setText(QString("%1 Objects").arg(project()->selectedObjects().size()));
    } else {
        _objectsLabel->setText(objects);
    }

    foreach (QString category, propWidgets.keys()) {
        QWidget* w = new QWidget(_tabWidget);
        w->setLayout(propWidgets[category]);
        _tabWidget->addTab(w, category);
    }

    QList<QWidget*> tabs;
    _tagList.clear();
    for (QString className : tags.keys()) {
        QWidget* tab = Tag::createWidget(tags[className], _tabWidget);
        _tagList.append(tags[className]);
        QScrollArea* scroll = new QScrollArea(_tabWidget);
        scroll->setWidget(tab);
        tabs.append(scroll);
        _tabWidget->addTab(tab, className);
    }
}
