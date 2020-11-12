/* SPDX-License-Identifier: GPLv3-or-later */

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QMessageBox>

#include "FrameDecoder.hpp"
#include "Util.hpp"
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

    try {
        if (parser.isSet(fileOption))
            mainWindow.loadVpp(parser.value(fileOption));
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    mainWindow.loadVpp("E:/Games/Emulation/Roms/PS2/S2ASSETS/CHARS.VPP");

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
