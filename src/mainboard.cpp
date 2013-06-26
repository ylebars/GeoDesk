/**
 * \file mainboard.cpp
 * \brief Implementation of application main board.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/20
 * \date 2013/06/21
 * \date 2013/06/24
 * \date 2013/06/25
 * \date 2013/06/26
 */

#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>
#include <cstddef>
#include <algorithm>
#include <iostream>

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

    ui.actionZoomIn->setEnabled(true);
    ui.actionZoomOut->setEnabled(true);
    ui.actionNormalSize->setEnabled(true);
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

/* -- When mouse is left-clicked. ----------------------------------------- */
void GUI::MainBoard::mousePressEvent (QMouseEvent* event) {
  QWidget::mousePressEvent(event);
  if (!imageLabel->pixmap()) return;
  if (event->button() != Qt::LeftButton) return;

  /* Coordinate of the point being clicked. */
  const auto pair = getMousePosition(event->pos());

  std::cout << "Image width: " << imageLabel->pixmap()->width() << '\n'
            << "Image height: " << imageLabel->pixmap()->height() << '\n'
            << "Position in image: (" << pair.first << ", " << pair.second
            << ")\n" ;
}
