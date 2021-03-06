#include <QApplication>
#include <QAction>
#include <QList>
#include <QMainWindow>
#include <QMessageBox>
#include <QtDebug>

#include "Scene_points_with_normal_item.h"
#include "Scene_polyhedron_item.h"
#include "Scene_polygon_soup_item.h"

#include <CGAL/Three/Polyhedron_demo_plugin_interface.h>
#include "Messages_interface.h"

using namespace CGAL::Three;
class Polyhedron_demo_point_set_from_vertices_plugin : 
  public QObject,
  public Polyhedron_demo_plugin_interface
{
  Q_OBJECT
  Q_INTERFACES(CGAL::Three::Polyhedron_demo_plugin_interface)
  Q_PLUGIN_METADATA(IID "com.geometryfactory.PolyhedronDemo.PluginInterface/1.0")

public:
  void init(QMainWindow* mainWindow,
            CGAL::Three::Scene_interface* scene_interface, Messages_interface*);

  bool applicable(QAction*) const {
    const CGAL::Three::Scene_interface::Item_id index = scene->mainSelectionIndex();

    return qobject_cast<Scene_polyhedron_item*>(scene->item(index))
      || qobject_cast<Scene_polygon_soup_item*>(scene->item(index));
  }

  QList<QAction*> actions() const;

public Q_SLOTS:
  void createPointSet();

private:
  CGAL::Three::Scene_interface* scene;
  QAction* actionPointSetFromPolyhedronVertices;

}; // end Polyhedron_demo_point_set_from_vertices_plugin

void Polyhedron_demo_point_set_from_vertices_plugin::init(QMainWindow* mainWindow,
                                                          CGAL::Three::Scene_interface* scene_interface,
                                                          Messages_interface*)
{
  scene = scene_interface;
  actionPointSetFromPolyhedronVertices = new QAction(tr("&Create Point Set from Vertices"), mainWindow);
  actionPointSetFromPolyhedronVertices->setObjectName("actionPointSetFromPolyhedronVertices");
  connect(actionPointSetFromPolyhedronVertices, SIGNAL(triggered()),
          this, SLOT(createPointSet()));
}

QList<QAction*> Polyhedron_demo_point_set_from_vertices_plugin::actions() const {
  return QList<QAction*>() << actionPointSetFromPolyhedronVertices;
}

void Polyhedron_demo_point_set_from_vertices_plugin::createPointSet()
{
  QApplication::setOverrideCursor(Qt::WaitCursor);
  const CGAL::Three::Scene_interface::Item_id index = scene->mainSelectionIndex();

  Scene_polyhedron_item* poly_item =
    qobject_cast<Scene_polyhedron_item*>(scene->item(index));

  if (poly_item)
    {
      Scene_points_with_normal_item* points
        = new Scene_points_with_normal_item(*(poly_item->polyhedron()));

      if (points)
        {
          points->setColor(Qt::blue);
          points->setName(QString("%1 (vertices)").arg(poly_item->name()));
          scene->addItem (points);
        }
    }

  Scene_polygon_soup_item* soup_item =
    qobject_cast<Scene_polygon_soup_item*>(scene->item(index));

  if (soup_item)
    {
      Scene_points_with_normal_item* points
        = new Scene_points_with_normal_item();
      points->setColor(Qt::blue);
      points->setName(QString("%1 (vertices)").arg(soup_item->name()));

      // TODO get points from polygon soup
      const std::vector<Kernel::Point_3>& pts
        = soup_item->points();

      for (std::size_t i = 0; i < pts.size(); ++ i)
        points->point_set()->insert (pts[i]);
      
      scene->addItem (points);
    }

  QApplication::restoreOverrideCursor();
}


#include "Point_set_from_vertices_plugin.moc"
