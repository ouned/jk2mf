#include <QResource>
#include <QFile>
#include <QDir>
#include <QMessageBox>
#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QStringList cmdline = QCoreApplication::arguments();

    if ( cmdline.length() == 3 )
    {
        // Delete DSound.dll
        QFile::remove(cmdline[2] + "\\DSound.dll");

        QFile plg(cmdline[2] + "\\winmm.dll");
        plg.open(QIODevice::WriteOnly);
        QResource winmm(":/files/incl/winmm.dll");
        plg.write((const char *)winmm.data(), winmm.size());
        plg.close();

        QString appdata(getenv("appdata"));

        QDir(appdata).mkdir("jk2mf");

        appdata += "\\jk2mf";

        QStringList list;
        list << "jk2mfcl21.dll" << "jk2mfcl23.dll" << "jk2mfcl30b1.dll" << "jk2mfcl211.dll";

        foreach ( QString name, list )
        {
            QFile cl(appdata + "\\" + name);
            cl.open(QIODevice::WriteOnly);
            QResource clres(":/files/incl/" + name);
            cl.write((const char *)clres.data(), clres.size());
            cl.close();
        }

        return 0;
    }

    MainWindow w;
    w.show();
    
    return a.exec();
}
