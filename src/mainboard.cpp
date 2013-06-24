/**
 * \file mainboard.cpp
 * \brief Implementation of application main board.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/20
 * \date 2013/06/21
 * \date 2013/06/24
 */

#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>

#include "mainboard.hpp"

/* -- Open an image. ------------------------------------------------------ */
void GUI::MainBoard::on_actionOpen_triggered () {
  fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                          QDir::currentPath());

  if (!fileName.isEmpty()) {
    const QImage image (fileName);
    if (image.isNull()) {
      QMessageBox::information(this, tr("GeoDesk"),
                               tr("Cannot load %1.").arg(fileName));
      return;
    }

    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;

    imageLabel->adjustSize();
  }
}

/* -- Zoom into image. ---------------------------------------------------- */
void GUI::MainBoard::on_actionZoomIn_triggered () {
  scaleImage(1.25);
}

/* -- Zoom out an image. -------------------------------------------------- */
void GUI::MainBoard::on_actionZoomOut_triggered () {
  scaleImage(0.8);
}

/* -- Set image to its normal size. --------------------------------------- */
void GUI::MainBoard::on_actionNormalSize_triggered () {
  imageLabel->adjustSize();
  scaleFactor = 1.0;
  ui.actionZoomIn->setEnabled(true);
  ui.actionZoomOut->setEnabled(true);
}
