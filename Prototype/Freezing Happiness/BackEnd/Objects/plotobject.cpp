#include "plotobject.h"
#include "BackEnd/Properties/propertystring.h"
#include "BackEnd/Properties/realproperty.h"
#include "BackEnd/Properties/integerproperty.h"
#include "BackEnd/mathutility.h"
#include "BackEnd/Properties/selectproperty.h"
#include "BackEnd/project.h"

REGISTER_DEFN_OBJECTTYPE(PlotObject);

const QStringList PlotObject::CARTESIAN = QStringList() << "start" << "end";
const QStringList PlotObject::POLAR = QStringList() << "polar-start" << "polar-end";

PlotObject::PlotObject(Project *project, QString name) : PathObject(project, name)
{
    addProperty("formular", new PropertyString("PlotObject", tr("formular"), "", true, QFont("Courier")));
    addProperty("points", new IntegerProperty("PlotObject", tr("Points"), 0, std::numeric_limits<int>::max(), 100));
    addProperty("mode", new SelectProperty("PlotObject", tr("Mode"), 0, QStringList() << tr("Cartesian") << tr("Polar")));

    addProperty("start", new RealProperty("PlotObject", tr("Start"), 0));
    addProperty("end", new RealProperty("PlotObject", tr("End"), 100));

    addProperty("polar-start", new RealProperty("PlotObject", tr("Start"), 0));
    addProperty("polar-end", new RealProperty("PlotObject", tr("End"), 360));
    polish();
    updatePropertiesVisibility();
}

void PlotObject::updatePath()
{
    _path = QPainterPath();
    if (((SelectProperty*) properties()["mode"])->currentIndex() == 0) {
        QList<qreal> xValues;
        QList<qreal> yValues;
        qreal start = ((RealProperty*) properties()["start"])->value();
        qreal end = ((RealProperty*) properties()["end"])->value();
        qreal pointCount = ((IntegerProperty*) properties()["points"])->value();

        qreal step = (end - start) / pointCount;
        if (step > 0) {
            for (qreal x = start; x <= end; x += step) {
                xValues.append(x);
           }
        } else if (step < 0) {
            for (qreal x = start; x >= end; x += step) {
                xValues.append(x);
            }
        } else {
            return;
        }
        if (xValues.isEmpty()) return;

        yValues = MathUtility::parse(((PropertyString*) properties()["formular"])->string(), xValues);
        _path = QPainterPath(QPointF(xValues.first(), yValues.first()));
        for (int i = 1; i < xValues.size(); i++) {
            _path.lineTo(xValues[i], yValues[i]);
        }
    } else {
        QList<qreal> alphas;
        QList<qreal> radius;
        qreal start = ((RealProperty*) properties()["polar-start"])->value() * M_PI/180.0;
        qreal end = ((RealProperty*) properties()["polar-end"])->value() * M_PI/180.0;
        qreal pointCount = ((IntegerProperty*) properties()["points"])->value();
        qreal step = (end - start) / pointCount;
        if (step > 0) {
            for (qreal x = start; x <= end; x += step) {
                alphas.append(x);
           }
        } else if (step < 0) {
            for (qreal x = start; x >= end; x += step) {
                alphas.append(x);
            }
        } else {
            return;
        }
        radius = MathUtility::parse(((PropertyString*) properties()["formular"])->string(), alphas);

        auto polarToCart = [](qreal alpha, qreal radius) {
            return QPointF(qCos(alpha) * radius,
                           qSin(alpha) * radius);
        };

        _path = QPainterPath(polarToCart(alphas.first(), radius.first()));
        for (int i = 1; i < alphas.size(); i++) {
            QPointF p = polarToCart(alphas[i], radius[i]);
            _path.lineTo(p.x(), p.y());
        }

    }
}

void PlotObject::updatePropertiesVisibility()
{
    for (QString cart : CARTESIAN) {
        properties()[cart]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 0);
    }
    for (QString polar : POLAR) {
        properties()[polar]->isVisible = (((SelectProperty*) properties()["mode"])->currentIndex() == 1);
    }

    project()->emitSelectionChanged();
}

void PlotObject::connectPropertyTriggers()
{
    connect(properties()["mode"], &Property::valueChanged, [=]() {updatePropertiesVisibility();});
}


