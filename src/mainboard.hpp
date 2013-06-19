#ifndef MAINBOARD_HPP
#define MAINBOARD_HPP

/**
 * \file mainboard.hpp
 * \brief Definitions to create program main board.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/19
 */

#include <QWidget>
#include <QLabel>
#include <QPixmap>
#include <QGridLayout>

/// \brief Namespace for GUI definition.
namespace GUI {
  class MainBoard: public QWidget {
      Q_OBJECT

    public:
      /**
       * \brief Default constructor.
       * \param parent Pointer to the parent widget.
       */
      MainBoard (QWidget* parent = nullptr): QWidget (parent) {
        /* Label to identify image. */
        QLabel* labelImg = new QLabel (this);
        /* Pixmap of the image. */
        QPixmap* pixmapImg =
          new QPixmap ("/home/ylebars/Images/carte_congo.jpg");
        labelImg->setPixmap(*pixmapImg);

        /* Layout in which place the image. */
        QGridLayout* gridLayout = new QGridLayout;
        gridLayout->addWidget(labelImg);
        setLayout(gridLayout);
      }
  };
}

#endif  // #ifndef MAINBOARD_HPP
