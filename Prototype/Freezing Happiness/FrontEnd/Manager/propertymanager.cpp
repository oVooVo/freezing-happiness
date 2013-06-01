#include "propertymanager.h"
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

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

    QMap<QString, QVBoxLayout*> propWidgets;



    foreach (QString name, properties.keys()) {
        if (properties[name].isEmpty()) continue;
        QString category = properties[name].first()->category();
        QVBoxLayout* layout;
        if (propWidgets.contains(category)) {
            layout = (QVBoxLayout*) propWidgets[category];
        } else {
            layout = new QVBoxLayout();
            layout->setContentsMargins(0,0,0,0);
            layout->setSpacing(0);
        }
        layout->addWidget(Property::createWidget(properties[name], _tabWidget));
        propWidgets.insert(category, layout);
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
        _tabWidget->insertTab(0, w, category);
    }
}
