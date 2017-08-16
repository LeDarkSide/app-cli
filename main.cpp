#include <QCoreApplication>
#include <QCommandLineParser>

#include <repositorytask.h>
#include <packagetask.h>
#include <settings.h>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("LeDarkSide");
    QCoreApplication::setApplicationName("app-cli");
    QCoreApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(a.tr("Command line interface for LeDarkSide"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
            {{"l", "list"},
                QCoreApplication::translate("main", "List packages in repositories.")},
            {{"ur", "update-repo"},
                QCoreApplication::translate("main", "Update the official repository.")},
            {"info",
                QCoreApplication::translate("main", "Displays information about a package."),
                QCoreApplication::translate("main", "package id")},
            {{"i", "install"},
                QCoreApplication::translate("main", "Installs the package component."),
                QCoreApplication::translate("main", "package id")},
            {{"u", "update"},
                QCoreApplication::translate("main", "Updates package component."),
                QCoreApplication::translate("main", "package id")},
    });

    parser.process(a);

    Repository repo("LeDarkSide", "The official repo of LeDarkSide", "https://github.com/LeDarkSide/modules.git", Repository::Git);

    ReadRepository taskRRepo;
    QObject::connect(&taskRRepo, &ReadRepository::result, [](Package package) {
        qDebug() << "read package finished:" << package.name() << package.description() << package.path();
    });
    if (parser.isSet("list")) {
        QObject::connect(&taskRRepo, &ReadRepository::finished, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
        taskRRepo.start(repo);
    }

    InstallRepository taskRepo;
    if (parser.isSet("update-repo")) {
        QObject::connect(&taskRepo, &InstallRepository::finished, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
        taskRepo.start(repo);
    }


    ReadPackage taskPack;
    QObject::connect(&taskPack, &ReadPackage::result, [](Package package) {
        qDebug() << "read package finished:" << package.name() << package.description() << package.path();
    });

    if (parser.isSet("info")) {
        QObject::connect(&taskPack, &ReadPackage::finished, qApp, &QCoreApplication::quit, Qt::QueuedConnection);
        taskPack.start(Settings::repositories() + '/' + repo.name() + '/' + parser.value("info"));
    }

    return a.exec();
}
