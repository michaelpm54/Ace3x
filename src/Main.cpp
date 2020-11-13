/* SPDX-License-Identifier: GPLv3-or-later */

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QMessageBox>
#include <iostream>

#include "qt_sink.hpp"
#include "widgets/main-window.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("Ace3X");
    QCoreApplication::setApplicationVersion("0.1");
    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::applicationName());
    parser.addHelpOption();
    QCommandLineOption fileOption(
        QStringList() << "f"
                      << "file",
        "File to open",
        "filename");
    parser.addOption(fileOption);
    parser.process(app);

    MainWindow mainWindow;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<qt_sink_mt>(mainWindow.getLog()));

    auto logger = std::make_shared<spdlog::logger>("qt_stdout_chain_logger", std::begin(sinks), std::end(sinks));
    logger->set_pattern("(%l) [%H:%M:%S] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::cfg::load_env_levels();

    try {
        if (parser.isSet(fileOption))
            mainWindow.load(parser.value(fileOption));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    mainWindow.show();

    try {
        app.exec();
    }
    catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(e.what()), QMessageBox::Ok);
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
