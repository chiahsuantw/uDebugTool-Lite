#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <io.h>
#include <cstring>
#include <windows.h>

#include <QFile>
#include <QMessageBox>
#include <QTextStream>
#include <QFileDialog>
#include <QProcess>
#include <QClipboard>
#include <QDesktopServices>

int hasSetPath = 0; //判斷是否已設定好路徑
QString dirPath;
QString location;
QString excutiveFile;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    char linkPath[] = "C:\\Users\\Public\\Documents\\uDebugTool_Data\\Link.txt";
        if (access(linkPath, 0) == 0)
        {
            QFile linkFile("C:\\Users\\Public\\Documents\\uDebugTool_Data\\Link.txt");

            if(linkFile.open(QFile::ReadOnly | QFile::Text))
            {
                hasSetPath = 1;

                QTextStream loadLink(&linkFile);
                location = loadLink.readLine();
                excutiveFile = loadLink.readLine();

                ui->FileName->setText(excutiveFile);
                ui->SourcePath->setText(location);
                dirPath = location + "/" + excutiveFile;
            }
        }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_Button_confirm_clicked() //使用者確認輸入測資
{
    QFile file("C:\\Users\\Public\\Documents\\uDebugTool_Data\\Input.txt"); //建立測資文件

    if(!file.open(QFile::WriteOnly | QFile::Text))
        QMessageBox::warning(this, "警告", "輸入測資檔案建立失敗!");
    else
    {
        QString dataText = ui->textEdit_input->toPlainText();
        QTextStream OutputText(&file);
        OutputText << dataText;
        file.flush();
        file.close();

        QString inputPath = "<C:\\Users\\Public\\Documents\\uDebugTool_Data\\Input.txt>";
        QString outputPath = "C:\\Users\\Public\\Documents\\uDebugTool_Data\\Output.txt";
        QString command = "\"" + dirPath + "\"" + inputPath + outputPath;
        system(qPrintable(command));

        //無法使用WinExec()操作CMD指令
        //QString winCommand = "cmd.exe /c " + command;
        //WinExec(qPrintable(winCommand),SW_HIDE);

        QFile file2("C:\\Users\\Public\\Documents\\uDebugTool_Data\\Output.txt");
        if(!file2.open(QFile::ReadOnly | QFile::Text))
            QMessageBox::warning(this, "警告", "輸出測資檔案建立失敗!");
        else if(hasSetPath==1)
        {
            QTextStream ShowText(&file2);
            QString Show = ShowText.readAll();
            if(Show.isEmpty())
            {
                ui->text_output->setPlainText("沒有輸出");
            }
            else
            {
                QMessageBox::information(this,"訊息","建立成功，已將輸出複製到剪貼簿!");
                ui->text_output->setPlainText(Show);

                QClipboard *board = QApplication::clipboard(); //建立剪貼簿物件
                board->setText(Show);
            }
            file.flush();
            file.close();
        }
        else
        {
            QMessageBox::warning(this,"設定錯誤","尚未設定路徑，請先選取執行檔!");
        }
    }
}

void MainWindow::on_Button_Select_clicked() //選擇檔案、建立臨時資料夾
{
    char filePath[] = "C:\\Users\\Public\\Documents\\uDebugTool_Data";
        if (access(filePath, 0) == -1)
            system("md C:\\Users\\Public\\Documents\\uDebugTool_Data");

    dirPath = QFileDialog::getOpenFileName(this,"選擇檔案","C://", tr("執行檔 (*.exe)"));
    if(!dirPath.isEmpty())
    {
        int lenghth = dirPath.size();
        int counter = 0;

        //取得執行檔名稱
        while(lenghth>0)
        {
            if(dirPath[lenghth-1] == '/')
                break;
            lenghth--;
            counter++;
        }

        int index = 0;
        for(int i=dirPath.size()-counter; i<dirPath.size(); i++)
        {
            excutiveFile[index]=dirPath[i];
            index++;
        }

        for(int i=0; i < dirPath.size()-counter-1; i++)
            location[i]=dirPath[i];

        ui->FileName->setText(excutiveFile);
        ui->SourcePath->setText(location);

        QFile saveLink("C:\\Users\\Public\\Documents\\uDebugTool_Data\\Link.txt");
        if(saveLink.open(QFile::WriteOnly | QFile::Text)){
            QTextStream OutputText(&saveLink);
            OutputText << location << "\n" << excutiveFile;
        }

        hasSetPath = 1; //路徑設定完成
    }
    else
    {
        hasSetPath = 0; //路徑設定失敗
        ui->SourcePath->setText("尚未設定");
        ui->FileName->setText("尚未設定");
    }
}

void MainWindow::on_label_6_linkActivated(const QString &link)
{
    QDesktopServices::openUrl (link);
}
