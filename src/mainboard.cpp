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
 * \date 2013/07/03
 * \date 2013/07/04
 * \date 2013/07/05
 * \date 2013/07/09
 * \date 2013/07/12
 * \date 2013/07/16
 * \date 2013/07/29
 * \date 2013/07/30
 * \date 2013/09/24
 * \date 2013/10/15
 * \date 2013/10/18
 * \date 2013/10/21
 * \date 2013/10/31
 * \date 2013/11/05
 * \date 2013/11/06
 * \date 2013/11/12
 */

#include <QFileDialog>
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
#include <boost/units/systems/si/io.hpp>
// #include <gdal_priv.h>

#include "mainboard.hpp"

/* -- Open an image. ------------------------------------------------------ */
void GUI::MainBoard::on_actionOpen_triggered () {
  ui.statusbar->showMessage(tr("Opening image."));

  /* Filter for standard images. */
  const QString standard =
    tr("Standard images (*.bmp *.gif *.jpg *.jpeg *.png *.pbm *.pgm *.ppm "
       "*.tiff *.tif *.xbm *.xpm)");
  /* Filter for geo-referenced images. */
  const QString georeferenced =
    tr("Geo-referenced images (*.tiff *.tif *.jp2 *.j2k *.jpf *.jpx *.jpm "
       "*.mj2)");
  /* Filter for all files. */
  const QString all = tr("All files (*)");
  /* List of filters. */
  const QString filtersList = standard + ";;" + georeferenced + ";;" + all;
  /* Filter selected by user. */
  QString selectedFilter;
  /* Name of the file to be opened. */
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                        QDir::currentPath(),
                                                        filtersList,
                                                        &selectedFilter);

  if (!fileName.isEmpty()) {
    if (selectedFilter == georeferenced) {
//       GDALAllRegister();
// 
//       /* Pointer to the image */
//       GDALDataset* poDataset =
//         static_cast<GDALDataset*>(GDALOpen(fileName.toStdString().c_str(),
//                                   GA_ReadOnly));
//       if (poDataset == nullptr) {
//         QMessageBox::information(this, tr("GeoDesk"),
//                                  tr("Cannot load %1.").arg(fileName));
//       }
    }
    else {
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
      ui.actionSetData->setEnabled(false);
      ui.actionSampleIsobath->setEnabled(false);
      ui.actionSetData->setChecked(false);
      ui.actionSampleIsobath->setChecked(false);

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
      ui.statusbar->showMessage(geoNotOk);
      if (QFile::exists(worldFileName)) {
        loadWorldFile(worldFileName);
      }

      ui.actionSaveWorldFile->setEnabled(false);
      ui.actionSaveReferencePoints->setEnabled(false);
      ui.actionSaveReferencePointsAs->setEnabled(false);
      data.clear();
      dataFileName.clear();
      referencePointFileName.clear();
      referencing = false;
      setting = false;
      sampling = false;
      ui.actionGeoreferenceImage->setEnabled(true);
      ui.actionSaveDataFile->setEnabled(false);
      ui.actionSaveDataFileAs->setEnabled(false);
    }
  }
  else {
    ui.statusbar->showMessage(noFile);
  }
}

/* -- Load a world file. -------------------------------------------------- */
void GUI::MainBoard::on_actionLoadWorldFile_triggered () {
  ui.statusbar->showMessage(tr("Loading world file."));

  /* Name of the file to be opened. */
  const QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                        QDir::currentPath(),
                                                tr("World files (*.bpw *.gfw "
                                                   "*.jgw *.pgw *.pmw "
                                                   "*.tfw *.twfx *.xmw);;"
                                                   "All files (*)"));

  if (!fileName.isEmpty()) {
    loadWorldFile(fileName);
    setting = false;
    ui.statusbar->showMessage(done);
  }
  else {
    ui.statusbar->showMessage(noFile);
  }
}

/* -- Load a data file. --------------------------------------------------- */
void GUI::MainBoard::on_actionLoadDataFile_triggered () {
  ui.statusbar->showMessage(tr("Loading data file."));

  data.clear();
  /* Name of the file to  be opened. */
  dataFileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                              QDir::currentPath(),
                                              tr("Text files (*.txt);;"
                                                 "All files (*)"));

  if (!dataFileName.isEmpty()) {
    /* The file itself. */
    QFile file (dataFileName);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
      data = file.readAll();
      file.close();
    }
    else {
      QMessageBox::critical(this, tr("Error"),
                            tr("File named \"%1\" cannot "
                               "be opened.").arg(dataFileName));
    }
    ui.statusbar->showMessage(done);
  }
  else {
    ui.statusbar->showMessage(noFile);
  }
}

/* -- Zoom into image. ---------------------------------------------------- */
void GUI::MainBoard::on_actionZoomIn_triggered () {
  ui.statusbar->showMessage(tr("Zooming in."));
  scaleImage(1.25);
  ui.statusbar->showMessage(done);
}

/* -- Zoom out an image. -------------------------------------------------- */
void GUI::MainBoard::on_actionZoomOut_triggered () {
  ui.statusbar->showMessage(tr("Zooming out."));
  scaleImage(0.8);
  ui.statusbar->showMessage(done);
}

/* -- Set image to its normal size. --------------------------------------- */
void GUI::MainBoard::on_actionNormalSize_triggered () {
  ui.statusbar->showMessage(tr("Back to normal size."));
  imageLabel->adjustSize();
  scaleFactor = 1.0;
  ui.actionZoomIn->setEnabled(true);
  ui.actionZoomOut->setEnabled(true);
  ui.statusbar->showMessage(done);
}

/* -- Save reference points used for image geo-referencing. --------------- */
void GUI::MainBoard::on_actionSaveReferencePoints_triggered () {
  ui.statusbar->showMessage(tr("Saving reference points."));

  if (referencePointFileName.isEmpty()) {
    on_actionSaveReferencePointsAs_triggered();;
  }
  else {
    saveReferencePointFile();
  }
}

/* -- Save reference points in a new file. -------------------------------- */
void GUI::MainBoard::on_actionSaveReferencePointsAs_triggered () {
  ui.statusbar->showMessage(tr("Saving reference points in a new file."));

  referencePointFileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save reference "
                                                           "points in a new "
                                                           "file"),
                                                        QDir::currentPath(),
                                                        tr("Text files "
                                                           "(*.txt);;"
                                                           "All files (*)"));
  saveReferencePointFile();
}

/* -- Save world file associated to opened image. ------------------------- */
void GUI::MainBoard::on_actionSaveWorldFile_triggered () {
  ui.statusbar->showMessage(tr("Saving world file."));

  if (QFile::exists(worldFileName)) {
    /* User's decision whether or not overwrite world file. */
    const int choice =
      QMessageBox::question(this, tr("Overwrite"),
                            tr("World file already exists, overwrite it?"),
                            QMessageBox::Yes | QMessageBox::No);
    if (choice == QMessageBox::No) {
      ui.statusbar->showMessage(aborted);
      return;
    }
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
                  << change(1, 1) << '\n'
                  << change(0, 2) << '\n'
                  << change(1, 2) << '\n';

  worldFile.close();

  ui.actionSaveWorldFile->setEnabled(false);

  ui.statusbar->showMessage(tr("Saved world file: %1").arg(worldFileName));
  setting = false;
}

/* -- Save data which have been set by the user. -------------------------- */
void GUI::MainBoard::on_actionSaveDataFile_triggered () {
  ui.statusbar->showMessage(tr("Saving data file."));

  if (dataFileName.isEmpty()) {
    on_actionSaveDataFileAs_triggered();
  }
  else {
    saveDataFile();
  }
}

/* -- Save data in a new file. -------------------------------------------- */
void GUI::MainBoard::on_actionSaveDataFileAs_triggered () {
  ui.statusbar->showMessage(tr("Saving data in a new file."));

  dataFileName = QFileDialog::getSaveFileName(this, tr("Save data file as"),
                                              QDir::currentPath(),
                                              tr("Text files (*.txt);;"
                                                 "All files (*)"));
  saveDataFile();
}

/* -- Give reference point for image geo-reference. ----------------------- */
void GUI::MainBoard::on_actionGeoreferenceImage_triggered () {
  ui.statusbar->showMessage(
    tr("Referencing: point 1 / %1").arg(requiredReference));

  worldExists = false;
  referencing = true;
  setting = false;
  sampling = false;
  numberReferencePoints = 0;
}

/* -- Enable setting geo-referenced data. --------------------------------- */
void GUI::MainBoard::on_actionSetData_triggered () {
  if (setting) {
    setting = false;
    ui.actionSampleIsobath->setEnabled(true);
    ui.statusbar->showMessage(tr("Stop setting geo-referenced data."));
  }
  else {
    setting = true;
    ui.actionSampleIsobath->setEnabled(false);
    ui.statusbar->showMessage(tr("Setting geo-referenced data."));
  }
}

/* -- Sampling an isobath. ------------------------------------------------ */
void GUI::MainBoard::on_actionSampleIsobath_triggered () {
  if (sampling) {
    sampling = false;
    ui.actionSetData->setEnabled(true);
    ui.statusbar->showMessage(tr("Stop sampling isobath."));
  }
  else {
    /* Did the user push the "OK" button? */
    bool ok;
    /* Isobath value. */
//     const quantity<length> isobath
//       (QInputDialog::getDouble(this, tr("Isobath value"),
//                                tr("Enter isobath value in meter"), 0., 0.,
//                                15000., 2, &ok) * meter);
      const double isobath =
        QInputDialog::getDouble(this, tr("Isobath value"),
                                tr("Enter isobath value in meter"), 0., 0.,
                                15000., 2, &ok);
    if (ok) {
      sampling = true;
      value = isobath;
      ui.actionSetData->setEnabled(false);
      ui.statusbar->showMessage(tr("Sampling isobath"));
    }
    else {
      ui.actionSampleIsobath->setChecked(false);
    }
  }
}

/* -- When mouse is left-clicked. ----------------------------------------- */
void GUI::MainBoard::mousePressEvent (QMouseEvent* event) {
  /* Degree character. */
  const QChar degree = 0x00B0;

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
                              0., -180., 180., 4, &ok);
    if (!ok) return;
    r2[numberReferencePoints].y() =
      QInputDialog::getDouble(this, tr("Latitude"),
                              tr("Latitude in decimal degrees north"),
                              0., -90., 90., 4, &ok);
    if (ok) {
      referencePointList.push_back(std::make_pair(pos,
                                                  r2[numberReferencePoints]));
      ui.actionSaveReferencePoints->setEnabled(true);
      ui.actionSaveReferencePointsAs->setEnabled(true);
      ++numberReferencePoints;
    }
    ui.statusbar->showMessage(
      tr("Referencing: point %1 / %2").arg(numberReferencePoints
                                           + 1).arg(requiredReference));

    if (numberReferencePoints == requiredReference) {
      change = computeCoefficients(r1, r2).transpose();
      ui.actionSaveWorldFile->setEnabled(true);
      ui.actionSetData->setEnabled(true);
      worldExists = true;
      referencing = false;
      ui.statusbar->showMessage(geoOk);
    }
  }
  else if (setting) {
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
    const quantity<length> value
      (QInputDialog::getDouble(this, tr("Enter value"),
                               message, 0., 0., 15000., 2, &ok) * meter);
    if (ok) {
      /* Stream on the QString which contains data. */
      QTextStream dataStream (&data);
      dataStream << pos.x() << ' ' << pos.y() << ' ' << b(0) << ' ' << b(1)
                 << ' ' << value.value() << '\n';
      ui.actionSaveDataFile->setEnabled(true);
      ui.actionSaveDataFileAs->setEnabled(true);
    }
  }
  else if (sampling) {
    /* Vector for referential change. */
    const Eigen::Vector3d x (pos.x(), pos.y(), 1.);
    /* Coordinates in geographical referential. */
    const Eigen::Vector2d b = change * x;
    /* Stream on the QString which contains data. */
    QTextStream dataStream (&data);
    dataStream << pos.x() << ' ' << pos.y() << ' ' << b(0) << ' ' << b(1)
               << ' ' << value << '\n';
    ui.statusbar->showMessage(tr("Isobath ") + value + tr(" m, ")
                              + QString::number(b(0)) + degree + tr(" E,")
                              + QString::number(b(1)) + degree + tr(" N"));
    ui.actionSaveDataFile->setEnabled(true);
    ui.actionSaveDataFileAs->setEnabled(true);
  }
}
