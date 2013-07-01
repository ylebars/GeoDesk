#ifndef MAINBOARD_HPP
#define MAINBOARD_HPP

/**
 * \file mainboard.hpp
 * \brief Definitions to create program main board.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/19
 * \date 2013/06/20
 * \date 2013/16/21
 * \date 2013/06/24
 * \date 2013/06/25
 * \date 2013/06/26
 * \date 2013/06/27
 * \date 2013/06/28
 * \date 2013/07/01
 */

#include <boost/concept_check.hpp>
#include <QtGui>
#include <QWidget>
#include <QLabel>
#include <QMenu>
#include <QApplication>
#include <QString>
#include <QAction>
#include <QScrollBar>
#include <QPoint>
#include <utility>
#include <vector>

#include "ui_mainboard.h"
#include "projection.hpp"

/// \brief Namespace for GUI definition.
namespace GUI {
  using namespace Projection;

  class MainBoard: public QMainWindow {
      Q_OBJECT

    public:
      /// \brief Default constructor.
      explicit MainBoard (): QMainWindow (), r1 (3), r2 (3) {
        ui.setupUi(this);

        imageLabel = new QLabel;
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);

        scrollArea = new QScrollArea;
        scrollArea->setWidget(imageLabel);
        setCentralWidget(scrollArea);

        ui.actionZoomIn->setEnabled(false);
        ui.actionZoomOut->setEnabled(false);
        ui.actionNormalSize->setEnabled(false);
        ui.actionSaveWorldFile->setEnabled(false);
      }

      /// \brief Destructor.
      virtual ~MainBoard () {
        delete imageLabel;
        delete scrollArea;
      }

    protected slots:
      /// \brief Get the name of the file to be opened.
      void on_actionOpen_triggered ();

      /// \brief Zooms in an image.
      void on_actionZoomIn_triggered ();

      /// \brief Zooms out an image.
      void on_actionZoomOut_triggered ();

      /// \brief Gets the image back to its normal size.
      void on_actionNormalSize_triggered ();

      /**
       * \brief Save a world file associated to an image.
       *
       * Information about world file format can be found at:
       * <http://en.wikipedia.org/wiki/World_file>
       */
      void on_actionSaveWorldFile_triggered ();

    protected:
      /**
       * \brief What to do when mouse is clicked.
       * \param event The event that indicates mouse button is clicked.
       */
      virtual void mousePressEvent (QMouseEvent *event);

    private:
      /// \brief Matrix to compute referential change.
      Eigen::Matrix<double, 2, 3> change;

      /// \brief Image scale factor.
      double scaleFactor;

      /// \brief Reference points in image coordinates.
      std::vector<Point2D> r1;

      /// \brief Reference points in geographical coordinates.
      std::vector<Point2D> r2;

      /// \brief Name of the file to be opened.
      QString fileName;

      /// \brief Name of the world file associated to the image.
      QString worldFileName;

      /// \brief Label for image manipulation.
      QLabel* imageLabel;

      /// \brief Allows to scroll in the image.
      QScrollArea* scrollArea;

      /// \brief Get the GUI description.
      Ui::MainBoard ui;

      /// \brief Number of given reference point .
      size_t numberReferencePoints;

      /// \brief Whether or not the world file exists.
      bool worldExists;

      /**
       * \brief Set scroll bar if needed.
       * \param scrollBar Scroll bar to be adjusted.
       * \param factor Factor for adjusting.
       */
      void adjustScrollBar (QScrollBar* scrollBar, double factor) {
        scrollBar->setValue(static_cast<int>(factor * scrollBar->value()
                               + ((factor - 1) * scrollBar->pageStep() / 2)));
      }

      /// \brief Change scale of an image.
      void scaleImage (double factor) {
        Q_ASSERT(imageLabel->pixmap());
        scaleFactor *= factor;
        imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        adjustScrollBar(scrollArea->horizontalScrollBar(), factor);
        adjustScrollBar(scrollArea->verticalScrollBar(), factor);

        ui.actionZoomIn->setEnabled(scaleFactor < 3.0);
        ui.actionZoomOut->setEnabled(scaleFactor > 0.333);
      }

      /**
       * \brief Get mouse pointer position in the image referential.
       * \param pos Position given by the event.
       * \return A pair containing abscissa and ordinate of the point being
       * clicked.
       */
      Point2D getMousePosition (const QPoint &pos) {
        /* Minimum of horizontal scroll bar. */
        const double horizontalMinimum =
          static_cast<double>(scrollArea->horizontalScrollBar()->minimum());
        /* Maximum of horizontal scroll bar. */
        const double horizontalMaximum =
          static_cast<double>(scrollArea->horizontalScrollBar()->maximum());
        /* Minimum of vertical scroll bar. */
        const double verticalMinimum =
          static_cast<double>(scrollArea->verticalScrollBar()->minimum());
        /* Maximum of vertical scroll bar. */
        const double verticalMaximum =
          static_cast<double>(scrollArea->verticalScrollBar()->maximum());
        /* Quantity of pixel not shown according horizontal orientation. */
        const double horizontalRemaining =
          static_cast<double>(std::max(imageLabel->pixmap()->width()
                              - imageLabel->width(), 0));
        /* Quantity of pixel not shown according vertical orientation. */
        const double verticalRemaining =
          static_cast<double>(std::max(imageLabel->pixmap()->width()
                              - imageLabel->width(), 0));
        /* How much horizontally scrolled. */
        const double horizontalPercentage =
          (horizontalMaximum > horizontalMinimum)?
            (static_cast<double>(scrollArea->horizontalScrollBar()->value())
             - horizontalMinimum)
            / (horizontalMaximum - horizontalMinimum): 0. ;
        /* How much vertically scrolled. */
        const double verticalPercentage =
          (verticalMaximum > verticalMinimum)?
            (static_cast<double>(scrollArea->verticalScrollBar()->value())
             - verticalMinimum)
            / (verticalMaximum - verticalMinimum): 0. ;
        /* Localisation where clicked in the frame. */
        const QPoint localisation =
          pos - imageLabel->pos() - scrollArea->pos();
        /* Abscissa of the point clicked in the image. */
        const double x =
          (static_cast<double>(localisation.x())
            + (horizontalRemaining * horizontalPercentage))
          / scaleFactor;
        /* Ordinate of the point clicked in the image. */
        const double y =
          (static_cast<double>(localisation.y())
            + (verticalRemaining * verticalPercentage))
          / scaleFactor;

        return Point2D (x, y);
      }
  };
}

#endif  // #ifndef MAINBOARD_HPP
