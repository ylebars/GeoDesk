#ifndef MAINBOARD_HPP
#define MAINBOARD_HPP

/**
 * \file mainboard.hpp
 * \brief Definitions to create program main board.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/19
 * \date 2013/06/20
 */

#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QGridLayout>
#include <QMenu>
#include <QAction>
#include <QApplication>

/// \brief Namespace for GUI definition.
namespace GUI {
  class MainBoard: public QMainWindow {
    public:
      /// \brief Default constructor.
      MainBoard () {
        /* Centre zone of the main window. */
        QWidget* mapWidget = new QWidget;

        /* Label to identify image. */
        QLabel* labelImg = new QLabel (this);
        /* Pixmap of the image. */
        QPixmap* pixmapImg =
          new QPixmap ("/home/ylebars/Images/carte_congo.jpg");
        labelImg->setPixmap(*pixmapImg);

        /* Layout in which place the image. */
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->addWidget(labelImg);
        mapWidget->setLayout(gridLayout);

        setCentralWidget(mapWidget);
        setWindowTitle("GeoDesk");

        /* File menu entry. */
        QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

        /* Leave the program. */
        QAction* quitAction = new QAction(tr("&Quit"), this);
        fileMenu->addAction(quitAction);
        quitAction->setShortcut(QKeySequence("Ctrl+Q"));
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
      }
  };
}

#endif  // #ifndef MAINBOARD_HPP
