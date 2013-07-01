/**
 * \file main.cpp
 * \brief Tool to get geographical data from digitalised maps.
 * \author Le Bars, Yoann
 * \version 1.0
 * \date 2013/06/19
 * \date 2013/07/01
 */

/**
 * \mainpage
 * GeoDesk is a tool to get geographical data from digitalised maps.
 *
 * Command : 
 *
 *      geodesk [Options]
 *
 * Supported options:
 *
 *      -h [ --help ]         Display help message.
 *      -v [ --version ]      Display program version.
 */

#include <config.hpp>

#include <cstddef>
#include <boost/test/included/prg_exec_monitor.hpp>
#include <boost/program_options.hpp>
#include <iostream>
#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QString>

#include "mainboard.hpp"

/**
 * \brief Main function of the program.
 * \param argc Count of arguments transmitted to the program.
 * \param argv Values of arguments transmitted to the program.
 * \return Qt return value.
 */
int cpp_main (int argc, char** argv) {
  namespace po = boost::program_options;

  /* Initialisation of Qt. */
  const QApplication app (argc, argv);

  /* System locale. */
  const QString locale = QLocale::system().name().section('_', 0, 0);
  QTranslator translator;
  translator.load(QString("qt_") + locale,
                  QLibraryInfo::location(QLibraryInfo::TranslationsPath));
  app.installTranslator(&translator);

  /* -- Reading the command line. -- */

  /* Declaring supported options. */
  po::options_description desc("Supported options");
  desc.add_options()
    ("help,h", "Display this help message.")
    ("version,v", "Display program version.");
  /* Command line. */
  po::options_description cmd;
  cmd.add(desc);

  /* Options map. */
  po::variables_map vm;
  po::store(po::command_line_parser(argc, argv).
  options(cmd).run(), vm);
  po::notify(vm);

  /* Indicates whether or not the program should be stopped. */
  bool stop = false;

  /* Check if help message should be displayed. */
  if (vm.count("help")) {
    std::cout << "GeoDesk is a tool to get geographical data from "
              << "digitalised maps.\n\n"
              << "Command : \n\n"
              << '\t' << argv[0] << " [Options]\n\n"
              << desc << '\n';
    stop = true;
  }

  /* Check if program version should be displayed. */
  if (vm.count("version")) {
    std::cout << argv[0] << " version "
              << Configuration::versionMajor
              << '.' << Configuration::versionMinor << '.'
              << Configuration::patchVersion << '\n';
    stop = true;
  }

  if (stop) return 0;

  /* Main board of the program. */
  GUI::MainBoard mainBoard;
  mainBoard.show();

  return app.exec();
}
