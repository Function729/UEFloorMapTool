#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QObject>
#include <QStyledItemDelegate>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class ESelectMode {
    None,
    ESM_FloorMode,
    ESM_BuildingMode

};
Q_NAMESPACE
Q_ENUM_NS(ESelectMode)


class DelagateWithCreateEditor : public QStyledItemDelegate
{
Q_OBJECT
    public:
    using QStyledItemDelegate::QStyledItemDelegate;

    signals:
    void textEdited(const QString& text);

protected:
    QWidget * createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

};



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void OnOpenButtonClicked();
    void OnSaveButtonClicked();
    void OnShowTableMenuCalled(const QPoint& pos);
    void OnTextChanged(const QString& text,int row,int col);
private:
    Ui::MainWindow *ui;
    QStringList parseCSVLine(QString inLine);
    QString filePath;
    QMenu* oneBlockRightMenu;
    QVector<class QTableWidgetItem*> TableItem;
    ESelectMode SelectMode;
};
#endif // MAINWINDOW_H
