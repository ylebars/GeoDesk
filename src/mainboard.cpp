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
 * \date 2013/07/02
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
                                          QDir::currentPath(),
tr("Images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm *.tiff *.xbm *.xpm);;All files (*)"));

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
    worldExists = false;
    if (QFile::exists(worldFileName)) {
      /* The world file itself. */
      QFile worldFile (worldFileName);
      if (worldFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        /* Stream on the file. */
        QTextStream worldFileStream (&worldFile);
        worldFileStream >> change(0, 0) >> change(1, 0) >> change(0, 1)
                        >> change(1, 1) >> change(0, 2) >> change(1, 2);
        worldExists = true;
      }
      else {
        QMessageBox::critical(this, "Error", "World file cannot be opened.");
      }
    }

    ui.actionSaveWorldFile->setEnabled(false);
    data = "";
    referencing = false;
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
  if (QFile::exists(worldFileName)) {
    /* User's decision whether or not overwrite world file. */
    const int choice =
      QMessageBox::question(this, tr("Overwrite"),
                            tr("World file already exists, overwrite it?"),
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

  ui.actionSaveWorldFile->setEnabled(false);
}

/* -- Save data which have been set by the user. -------------------------- */
void GUI::MainBoard::on_actionSaveDataFile_triggered () {
  /* Name of the file to contain data. */
  const QString dataFileName =
    QFileDialog::getSaveFileName(this, tr("Save data file"),
                                 QDir::currentPath(),
                                 tr("Text files (*.txt);;All files (*)"));
  if (!dataFileName.isEmpty()) {
    /* Data file itself. */
    QFile dataFile (dataFileName);
    dataFile.open(QIODevice::WriteOnly | QIODevice::Text
                  | QIODevice::Truncate);
    /* Stream on the data file. */
    QTextStream dataFileStream (&dataFile);
    dataFileStream << data;
    dataFile.close();
  }
}

/* -- Give reference point for image geo-reference. ----------------------- */
void GUI::MainBoard::on_actionGeoreferenceImage_triggered () {
  worldExists = false;
  referencing = true;
  numberReferencePoints = 0;

  status->setText(tr("Referencing: point 1 / %1").arg(requiredReference));
}

/* -- When mouse is left-clicked. ----------------------------------------- */
void GUI::MainBoard::mousePressEvent (QMouseEvent* event) {
  QWidget::mousePressEvent(event);
  if (!imageLabel->pixmap()) return;
  if (event->button() != Qt::LeftButton) return;

  /* Coordinate of the point being clicked. */
  const Point2D pos = getMousePosition(event->pos());

  if (referencing) {
    /* Reference points in image coordinates. */
    static std::vector<Point2D> r1 (requiredReference);

    /* \brief Reference points in geographical coordinates. */
    static std::vector<Point2D> r2 (requiredReference);

    r1[numberReferencePoints] = pos;
    /* Did the user push "OK" button? */
    bool ok;
    r2[numberReferencePoints].x() =
      QInputDialog::getDouble(this, tr("Longitude"),
                              tr("Longitude in decimal degrees east"),
                              0., -180., 180., 2, &ok);
    if (!ok) return;
    r2[numberReferencePoints].y() =
      QInputDialog::getDouble(this, tr("Latitude"),
                              tr("Latitude in decimal degrees north"),
                              0., -90., 90., 2, &ok);
    if (ok) ++numberReferencePoints;
    status->setText(tr("Referencing: point %1 / %2").arg(numberReferencePoints
                                                 + 1).arg(requiredReference));

    if (numberReferencePoints == requiredReference) {
      change = computeCoefficients(r1, r2).transpose();
      ui.actionSaveWorldFile->setEnabled(true);
      worldExists = true;
      referencing = false;
      status->setText("");
    }
  }
  else if (worldExists) {
    /* Degree character. */
    const QChar degree = 0x00B0;
    /* Vector for referential change. */
    const Eigen::Vector3d x (pos.x(), pos.y(), 1.);
    /* Coordinates in geographical referential. */
    const Eigen::Vector2d b = change * x;
    /* Message to be outputted. */
    const QString message = QString::number(b(0)) + degree + tr(" E, ")
      + QString::number(b(1)) + degree + tr(" N");
    /* Did the user push "OK" button? */
    bool ok;
    /* Value associated to the clicked localisation. */
    const double value = QInputDialog::getDouble(this, tr("Enter value"),
                                                 message, 0., 0., 15000., 2,
                                                 &ok);
    /* Stream on the QString which contains data. */
    QTextStream dataStream (&data);
    dataStream << pos.x() << ' ' << pos.y() << ' ' << b(0) << ' ' << b(1)
               << ' ' << value << '\n';
  }
}
