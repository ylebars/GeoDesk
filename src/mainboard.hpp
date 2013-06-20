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

#include <boost/concept_check.hpp>
#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QString>

/// \brief Namespace for GUI definition.
namespace GUI {
  class MainBoard: public QMainWindow {
      Q_OBJECT

    public:
      /// \brief Default constructor.
      MainBoard () {
        imageLabel = new QLabel;
        imageLabel->setBackgroundRole(QPalette::Base);
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);

        scrollArea = new QScrollArea;
        scrollArea->setBackgroundRole(QPalette::Dark);
        scrollArea->setWidget(imageLabel);
        setCentralWidget(scrollArea);

        /* File menu entry. */
        QMenu* fileMenu = menuBar()->addMenu(tr("&File"));

        /* Open an image. */
        QAction* openAction = new QAction (tr("&Open"), this);
        fileMenu->addAction(openAction);
        openAction->setShortcut(QKeySequence("Ctrl+O"));
        connect(openAction, SIGNAL(triggered()), this, SLOT(openFile()));

        /* Leave the program. */
        QAction* quitAction = new QAction (tr("&Quit"), this);
        fileMenu->addAction(quitAction);
        quitAction->setShortcut(QKeySequence("Ctrl+Q"));
        connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

        setWindowTitle("GeoDesk");
        resize(500, 400);
      }

      /// \brief Destructor.
      virtual ~MainBoard () {
        delete imageLabel;
        delete scrollArea;
      }

    protected slots:
      /// \brief Get the name of the file to be opened.
      void openFile ();

    private:
      /// \brief Image scale factor.
      double scaleFactor;

      /// \brief Name of the file to be opened.
      QString fileName;

      /// \brief Label for image manipulation.
      QLabel* imageLabel;

      /// \brief Allows to scroll in the image.
      QScrollArea* scrollArea;
  };
}

#endif  // #ifndef MAINBOARD_HPP
