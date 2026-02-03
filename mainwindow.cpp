#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QSaveFile>
#include <QSettings>
#include <QTextStream>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->OpenButton,&QPushButton::clicked,this,&MainWindow::OnOpenButtonClicked);
    connect(ui->SaveButton,&QPushButton::clicked,this,&MainWindow::OnSaveButtonClicked);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnOpenButtonClicked()
{
    QSettings settings("ZhangYonghao", "UEFloorMapTool");
    filePath = settings.value("lastPath",QDir::homePath()).toString();
    QString filePathWithFile = QFileDialog::getOpenFileName(this,QString("选择CSV文件"),filePath,QString("CSV文件 (*.csv);;所有文件 (*)"));
    if(filePath.isEmpty())return;

    QFile file(filePathWithFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::warning(this, tr("错误"), tr("无法打开文件: %1").arg(file.errorString()));
        return;
    }
    QFileInfo info(filePathWithFile);
    settings.setValue("lastPath", info.absolutePath());


   // QString lastPath = settings.value("lastPath", filePath).toString();
    QTextStream in(&file);
    in.setCodec("UTF-8");
    ui->TableWidget->clear();
    ui->TableWidget->setRowCount(0);
    ui->TableWidget->setColumnCount(0);
    if (!in.atEnd())
    {
        QString headerLine = in.readLine();
        QStringList headers = parseCSVLine(headerLine);
        ui->TableWidget->setColumnCount(headers.size());
        ui->TableWidget->setHorizontalHeaderLabels(headers);
    }
    int row = 0;
    while (!in.atEnd())
    {
        QString line = in.readLine();
        QStringList fields = parseCSVLine(line);

        if (fields.isEmpty())
        {
            continue;
        }

        ui->TableWidget->insertRow(row);
        for (int col = 0; col < fields.size(); ++col) {
            QTableWidgetItem *item = new QTableWidgetItem(fields.at(col));
            ui->TableWidget->setItem(row, col, item);
        }
        ++row;
    }

    file.close();
}

void MainWindow::OnSaveButtonClicked()
{
    QSettings settings("ZhangYonghao", "UEFloorMapTool");
    QString lastPath = settings.value("lastPath", filePath).toString();

    if (filePath.length()==0){
        filePath = QFileDialog::getOpenFileName(this,QString("保存位置"),"",QString("CSV文件 (*.csv);;所有文件 (*)"));
    }
    if (ui->TableWidget->rowCount()==0)return;


    QFile file(filePath);
    file.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner);
    //DWORD attributes = GetFileAttributesW(filePath.toStdWString().c_str());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        statusBar()->showMessage("保存失败"+file.errorString(), 3000);
    }


    QTextStream stream(&file);
    stream.setCodec("UTF-8");  // 使用 UTF-8 编码
    int rowCount = ui->TableWidget->rowCount();
    int colCount = ui->TableWidget->columnCount();

    for (int row = 0; row < rowCount; ++row)
    {
        QStringList rowData;

        // 遍历所有列
        for (int col = 0; col < colCount; ++col)
        {
            QTableWidgetItem *item = ui->TableWidget->item(row, col);
            QString cellText = item ? item->text() : "";

            // 处理单元格文本中的双引号（转义为两个双引号）
            cellText.replace("\"", "\"\"");

            // 第一列不加引号，其他列加引号
            if (col == 0) {
                // 第一列：只有包含逗号、换行符或双引号时才加引号
                if (cellText.contains(',') || cellText.contains('\n') ||
                        cellText.contains('"') || cellText.contains('\r')) {
                    cellText = "\"" + cellText + "\"";
                }
            } else {
                // 其他列：始终加引号
                cellText = "\"" + cellText + "\"";
            }

            rowData.append(cellText);
        }

        // 写入一行数据
        stream << rowData.join(",");

        // 如果不是最后一行，添加换行符
        if (row < rowCount - 1) {
            stream << "\n";
        }
    }
    file.close();
    statusBar()->showMessage("保存成功", 3000);
}

QStringList MainWindow::parseCSVLine(QString inLine)
{
    QString currentField;
    QStringList fields;
    for (int i = 0; i<inLine.length();++i) {
        QChar current = inLine.at(i);
        if(current != ',')
        {
            if(current != '"')
            {
                currentField.append(current);
            }
        }
        else
        {
            fields.append(currentField);
            currentField.clear();
        }

    }
    fields.append(currentField);
    currentField.clear();
    if(!fields.empty())
    {
        return fields;
    }
    return QStringList();
}
