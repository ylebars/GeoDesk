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

#include "ui_mainboard.h"

/// \brief Namespace for GUI definition.
namespace GUI {
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

    private:
      /// \brief Image scale factor.
      double scaleFactor;

      /// \brief Name of the file to be opened.
      QString fileName;

      /// \brief Label for image manipulation.
      QLabel* imageLabel;

      /// \brief Allows to scroll in the image.
      QScrollArea* scrollArea;

      /// \brief Get the GUI description.
      Ui::MainBoard ui;

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
  };
}

#endif  // #ifndef MAINBOARD_HPP
