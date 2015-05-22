#include <QSettings>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>
#include <Windows.h>
#include <TlHelp32.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"

#pragma comment(lib, "Shell32.lib")

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QSettings sets("HKEY_LOCAL_MACHINE\\Software\\LucasArts Entertainment Company LLC\\Star Wars JK II Jedi Outcast\\1.0", QSettings::NativeFormat);
    QString path = sets.value("Install Path", "").toString();

    if ( path != "" )
    {
        ui->txtPath->setText(path + "\\GameData");
    }

    path = sets.value("InstallPath", "").toString();

    if ( path != "" )
    {
        ui->txtPath->setText(path + "\\GameData");
    }
}

HANDLE GetProcessByName(PCWSTR name)
{
    DWORD pid = 0;

    // Create toolhelp snapshot.
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 process;
    ZeroMemory(&process, sizeof(process));
    process.dwSize = sizeof(process);

    // Walkthrough all processes.
    if (Process32First(snapshot, &process))
    {
        do
        {
            // Compare process.szExeFile based on format of name, i.e., trim file path
            // trim .exe if necessary, etc.
            if (!wcsicmp(process.szExeFile, name))
            {
               pid = process.th32ProcessID;
               break;
            }
        } while (Process32Next(snapshot, &process));
    }

    CloseHandle(snapshot);

    if (pid != 0)
    {
         return OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    }

    // Not found


       return NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnQuit_clicked()
{
    this->close();
}

bool installUAC = false;

void MainWindow::on_txtPath_textChanged(const QString &arg1)
{
    QFileInfo fi(arg1 + "\\jk2mp.exe");

    if ( fi.size() != 0 )
    {
        if ( QFileInfo(arg1 + "\\winmm.dll").size() == 267264 )
        {
            ui->btnInstall->setEnabled(false);
            ui->btnInstall->setIcon(QIcon());
            ui->btnInstall->setText("already installed");
            return;
        }

        ui->btnInstall->setEnabled(true);
        ui->btnInstall->setText("Install");

        QFile tf(arg1 + "\\test.tmp");
        if ( tf.open(QIODevice::WriteOnly) )
        {
            tf.close();
            tf.remove();
            ui->btnInstall->setIcon(QIcon());
            installUAC = false;
        }
        else
        {
            QIcon uacIco = QApplication::style()->standardIcon(QStyle::SP_VistaShield);
            ui->btnInstall->setIcon(uacIco);
            installUAC = true;
        }
    }
    else
    {
        ui->btnInstall->setEnabled(false);
        ui->btnInstall->setIcon(QIcon());
        ui->btnInstall->setText("Install");
    }
}

void MainWindow::on_btnPath_clicked()
{
    QMessageBox::information(this, "Information", "Please select the \"GameData\" directory of your Jedi Knight II installation.");

    QString path = QFileDialog::getExistingDirectory(this, tr("Directory"));

    if ( path != "" )
        ui->txtPath->setText(path);
}

void MainWindow::on_btnInstall_clicked()
{
    HANDLE h = GetProcessByName(L"jk2mp.exe");
    if ( h )
    {
        CloseHandle(h);

        QMessageBox::critical(this, "Error", "JK2 is running.\nPlease end JK2 before installing JK2MF.");
        return;
    }

    QFileInfo fi(ui->txtPath->text() + "\\jk2mp.exe");

    if ( fi.size() != 1155072 && fi.size() != 1507328 && fi.size() != 1126400 )
    {
        QMessageBox::critical(this, "Error", "JK2 has been found at<br>\"" + ui->txtPath->text() + "\"\n but is not version <font color=\"red\">1.04 or 1.02</font>");
        return;
    }

    wchar_t pathtmp[2048];
    memset(pathtmp, 0, sizeof(pathtmp));
    QCoreApplication::applicationFilePath().toWCharArray(pathtmp);

    wchar_t param[2048];
    memset(param, 0, sizeof(param));
    ("install \"" + ui->txtPath->text() + "\"").toWCharArray(param);

    SHELLEXECUTEINFO shExInfo = {0};
    shExInfo.cbSize = sizeof(shExInfo);
    shExInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
    shExInfo.hwnd = 0;

    if ( installUAC )
        shExInfo.lpVerb = L"runas";

    shExInfo.lpFile = pathtmp;
    shExInfo.lpParameters = param;
    shExInfo.lpDirectory = 0;
    shExInfo.nShow = SW_SHOW;
    shExInfo.hInstApp = 0;

    if (ShellExecuteEx(&shExInfo))
    {
        WaitForSingleObject(shExInfo.hProcess, INFINITE);
    }

    DWORD exitCode;
    GetExitCodeProcess(shExInfo.hProcess, &exitCode);
    CloseHandle(shExInfo.hProcess);

    if ( exitCode == 0 )
    {
        QMessageBox::information(this, "Success", "JK2MF Clientside is now installed!\nHave fun playing the game :)");
        QApplication::exit(0);
    }
    else
    {
        QMessageBox::critical(this, "Error", "JK2MF could not be installed! Please try the manual installation!");
    }
}
