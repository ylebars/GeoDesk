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
 * \date 2013/06/27
 * \date 2013/06/28
 */

#include <QFileDialog>
#include <QString>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>
#include <cstddef>
#include <algorithm>
#include <QInputDialog>
#include <QMessageBox>
#include <sstream>

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

    numberReferencePoints = 0;
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
  const Point2D pos = getMousePosition(event->pos());

  if (numberReferencePoints < r1.size()) {
    r1[numberReferencePoints] = pos;
    /* Did the user push "OK" button? */
    bool ok;
    r2[numberReferencePoints].x() =
      QInputDialog::getDouble(this, tr("Longitude"),
                              tr("Longitude in decimal degrees east"),
                              0., -180., 180., 2, &ok);
    r2[numberReferencePoints++].y() =
      QInputDialog::getDouble(this, tr("Latitude"),
                              tr("Latitude in decimal degrees north"),
                              0., -90., 90., 2, &ok);

    if (numberReferencePoints == 3) {
      change = computeCoefficients(r1, r2).transpose();
    }
  }
  else {
    const Eigen::Vector3d x (pos.x(), pos.y(), 1.);
    const Eigen::Vector2d b = change * x;
    /* Message to be outputted. */
    const QString message = QString::number(b(0)) + tr("° E, ")
      + QString::number(b(1)) + tr("° N");
    QMessageBox::information(this, tr("Coordinates"), message);
  }
}
