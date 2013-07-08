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
 * \date 2013/07/02
 * \date 2013/07/03
 * \date 2013/07/04
 * \date 2013/07/05
 * \date 2013/07/08
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
#include <boost/units/systems/si/length.hpp>

#include "ui_mainboard.h"
#include "projection.hpp"

/// \brief Namespace for GUI definition.
namespace GUI {
  using namespace Projection;
  using namespace boost::units;
  using namespace boost::units::si;

  /// \brief Class for program main window.
  class MainBoard: public QMainWindow {
      Q_OBJECT

    public:
      /// \brief Default constructor.
      explicit MainBoard (): QMainWindow () {
        ui.setupUi(this);

        imageLabel = new QLabel;
        imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
        imageLabel->setScaledContents(true);

        scrollArea = new QScrollArea;
        scrollArea->setWidget(imageLabel);
        setCentralWidget(scrollArea);

        ui.actionGeoreferenceImage->setEnabled(false);
        ui.actionSetData->setEnabled(false);
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

      /// \brief Load a world file.
      void on_actionLoadWorldFile_triggered ();

      /// \brief Load a data file.
      void on_actionLoadDataFile_triggered ();

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

      /// \brief Save data which have been set by user.
      void on_actionSaveDataFile_triggered ();

      /// \brief Save data in a new file.
      void on_actionSaveDataFileAs_triggered ();

      /// \brief Give reference points for image geo-reference.
      void on_actionGeoreferenceImage_triggered ();

      /// \brief Enable setting of geo-referenced data.
      void on_actionSetData_triggered ();

    protected:
      /**
       * \brief What to do when mouse is clicked.
       * \param event The event that indicates mouse button is clicked.
       */
      virtual void mousePressEvent (QMouseEvent *event);

    private:
      /// \brief String indicating operation complete.
      const QString done = tr("Done.");

      /// \brief String indicating geo-referencing is enable.
      const QString geoOk = tr("Image with geo-reference.");

      /// \brief String indicating geo-referencing is disable.
      const QString geoNotOk = tr("Image without geo-reference.");

      /// \brief String indicating there is no file to be opened.
      const QString noFile = tr("No file to be opened.") ;

      /// \brief Number of reference points required.
      const size_t requiredReference = 3;

      /// \brief Matrix to compute referential change.
      Eigen::Matrix<double, 2, 3> change;

      /// \brief Image scale factor.
      double scaleFactor;

      /// \brief Name of the world file associated to the image.
      QString worldFileName;

      /// \brief Name of the file containing geo-referenced data.
      QString dataFileName;

      /// \brief Data to be stored.
      QString data;

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

      /// \brief Whether or not being referencing image.
      bool referencing;

      /// \brief Whether or not being setting geo-referenced data.
      bool setting;

      /**
       * \brief Actually load world file.
       * \param fileName Name of the world file.
       */
      void loadWorldFile (const QString &fileName) {
        /* The world file itself. */
        QFile worldFile (fileName);
        if (worldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
          /* Stream on the file. */
          QTextStream worldFileStream (&worldFile);
          worldFileStream >> change(0, 0) >> change(1, 0) >> change(0, 1)
                          >> change(1, 1) >> change(0, 2) >> change(1, 2);
          worldFile.close();
          worldExists = true;
          ui.statusbar->showMessage(geoOk);
          ui.actionSaveWorldFile->setEnabled(false);
          ui.actionSetData->setEnabled(true);
        }
        else {
          QMessageBox::critical(this, tr("Error"),
                                tr("World file cannot be opened."));
        }
      }

      /// \brief Actually save data file.
      void saveDataFile () {
        if (!dataFileName.isEmpty()) {
          /* Data file itself. */
          QFile dataFile (dataFileName);
          if (dataFile.open(QIODevice::WriteOnly | QIODevice::Text
                        | QIODevice::Truncate)) {
              /* Stream on the data file. */
              QTextStream dataFileStream (&dataFile);
              dataFileStream << data;
              dataFile.close();
          }
          else {
            QMessageBox::critical(this, tr("Error"),
                            tr("File named \"%1\" cannot "
                               "be opened.").arg(dataFileName));
          }
          ui.statusbar->showMessage(done);
        }
        else {
          ui.statusbar->showMessage(tr("No file name specified."));
        }
      }

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
