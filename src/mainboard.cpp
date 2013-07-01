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
 * \date 2013/07/01
 */

#include <QFileDialog>
#include <QLabel>
#include <QPixmap>
#include <QImage>
#include <QMessageBox>
#include <cstddef>
#include <algorithm>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QTextStream>
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

    ui.actionZoomIn->setEnabled(true);
    ui.actionZoomOut->setEnabled(true);
    ui.actionNormalSize->setEnabled(true);

    /* Information about the image file. */
    const QFileInfo imageFile (fileName);
    /* Extension of the image file. */
    const QString imageExtension = imageFile.suffix();
    /* Extension for the world file. */
    const QString worldExtension = '.' + imageExtension.at(0)
      + imageExtension.at(imageExtension.size() - 1) + 'w';
    /* Name of the world file. */
    worldFileName =
      imageFile.canonicalPath() + QDir::separator()
      + imageFile.completeBaseName() + worldExtension;
    if (QFile::exists(worldFileName)) {
      worldExists = true;
      /* The world file itself. */
      QFile worldFile (worldFileName);
      if (worldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        /* Stream on the file. */
        QTextStream worldFileStream (&worldFile);
        worldFileStream >> change(0, 0) >> change(1, 0) >> change(0, 1)
                        >> change(1, 1) >> change(0, 2) >> change(1, 2);
        numberReferencePoints = 3;
      }
      else {
        QMessageBox::critical(this, "Error", "World file cannot be opened.");
      }
    }
    else {
      worldExists = false;
      numberReferencePoints = 0;
    }

    ui.actionSaveWorldFile->setEnabled(false);
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

/* -- Save world file associated to opened image. ------------------------- */
void GUI::MainBoard::on_actionSaveWorldFile_triggered () {
  if (worldExists) {
    /* User's decision whether or not overwrite world file. */
    const int choice =
      QMessageBox::question(this, "Overwrite",
                            "World file already exists, overwrite it?",
                            QMessageBox::Yes | QMessageBox::No);
    if (choice == QMessageBox::No) return;
  }

  /* World file itself. */
  QFile worldFile (worldFileName);
  worldFile.open(QIODevice::WriteOnly | QIODevice::Text
                 | QIODevice::Truncate);
  /* Stream on the world file. */
  QTextStream worldFileStream (&worldFile);

  worldFileStream << change(0, 0) << '\n'
                  << change(1, 0) << '\n'
                  << change(0, 1) << '\n'
                  << change(1, 1) << "\n\n"
                  << change(0, 2) << '\n'
                  << change(1, 2) << '\n';

  worldFile.close();
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
      ui.actionSaveWorldFile->setEnabled(true);
    }
  }
  else {
    /* Degree character. */
    const QChar degree = 0x00B0;
    /* Vector for referential change. */
    const Eigen::Vector3d x (pos.x(), pos.y(), 1.);
    /* Coordinates in geographical referential. */
    const Eigen::Vector2d b = change * x;
    /* Message to be outputted. */
    const QString message = QString::number(b(0)) + degree + tr(" E, ")
      + QString::number(b(1)) + degree + tr(" N");
    QMessageBox::information(this, tr("Coordinates"), message);
  }
}
