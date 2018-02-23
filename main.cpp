#include <QCoreApplication>
#include <QCommandLineParser>

#include <ledarkside.h>
#include <repositorytask.h>
#include <packagetask.h>

#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QCoreApplication::setOrganizationName("LeDarkSide");
    QCoreApplication::setApplicationName("app-cli");
    QCoreApplication::setApplicationVersion(VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(QCoreApplication::tr("Command line interface for LeDarkSide"));
    parser.addHelpOption();
    parser.addVersionOption();

    parser.addOptions({
            {"list-repos",
                QCoreApplication::translate("main", "List repositories.")},
            {"info-repo",
                QCoreApplication::translate("main", "Displays information about a repository."),
                QCoreApplication::translate("main", "repository name")},
            {"update-repo",
                QCoreApplication::translate("main", "Update a repository."),
                QCoreApplication::translate("main", "repository name")},
            {"update-repos",
                QCoreApplication::translate("main", "Update repositories.")},

            {"list-pkgs",
                QCoreApplication::translate("main", "List packages in repositories.")},
            {"list-installed",
                QCoreApplication::translate("main", "List installed packages.")},
            {"info-pkg",
                QCoreApplication::translate("main", "Displays information about a package."),
                QCoreApplication::translate("main", "package id")},
            {"install-pkg",
                QCoreApplication::translate("main", "Installs the package component."),
                QCoreApplication::translate("main", "package id")},
            {"uninstall-pkg",
                QCoreApplication::translate("main", "Uninstalls the package component."),
                QCoreApplication::translate("main", "package id")},
            {"update-pkg",
                QCoreApplication::translate("main", "Updates package component."),
                QCoreApplication::translate("main", "package id")},
            {"update-pkgs",
                QCoreApplication::translate("main", "Update all installed packages.")},
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
        taskPack.start(LeDarkSide::repositories() + '/' + repo.name() + '/' + parser.value("info"));
    }

    return a.exec();
}
