/* SPDX-License-Identifier: GPLv3-or-later */

#include <spdlog/cfg/env.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QMessageBox>

#include "qt-sink.hpp"
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

    QApplication::setWindowIcon(QPixmap(":/images/ace3x_icon.png"));

    /*
        MainWindow uses spdlog before the logging is fully initialised because
        initialisation needs QTextEdit *from* the QMainWindow. This can be avoided
        by passing a QTextEdit into the QMainWindow, but this way isn't too
        bad either.
    */
    spdlog::set_pattern("[%^%L%$] [%H:%M:%S] %v");

    MainWindow main_window;

    std::vector<spdlog::sink_ptr> sinks;
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<qt_sink_mt>(main_window.get_log()));

    auto logger = std::make_shared<spdlog::logger>("qt_stdout_chain_logger", std::begin(sinks), std::end(sinks));
    logger->set_pattern("[%^%L%$] [%H:%M:%S] %v");
    spdlog::register_logger(logger);
    spdlog::set_default_logger(logger);

    spdlog::cfg::load_env_levels();

    try {
        if (parser.isSet(fileOption))
            main_window.load(parser.value(fileOption));
        main_window.show();
    }
    catch (const std::exception &e) {
        fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    try {
        app.exec();
    }
    catch (const std::exception &e) {
        QMessageBox::critical(nullptr, "Error", QString::fromStdString(e.what()), QMessageBox::Ok);
        fprintf(stderr, "%s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
