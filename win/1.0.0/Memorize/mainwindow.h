#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statistics.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QSettings>
#include <QTextToSpeech>
#include <QMediaPlayer>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QInputDialog>
#include <QTimer>
#include <QTime>
#include <QButtonGroup>
#include <QRandomGenerator>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QComboBox>
#include <QStackedWidget>
#include <QHeaderView>
#include <QCloseEvent>
#include <QSpinBox>
#include <QFormLayout>
#include <QTextStream>
#include <QFileInfo>
#include <QPainter>
#include <QItemDelegate>
#include <QTranslator>  // 保留这行
#include <QMap>  // 添加这行


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class PlayButtonDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit PlayButtonDelegate(QObject *parent = nullptr);
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index) override;

signals:
    void playClicked(const QString &word);

private:
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionExit_triggered();
    void on_actionAdd_Word_triggered();
    void on_playButton_clicked();
    void on_actionTest_triggered();
    void on_actionHome_triggered();
    void on_actionStatistics_triggered();
    void saveTestResult(double accuracy, int wordCount);
    void showAboutDialog();
    void on_actionTheUserWW_triggered();
    void onWordListSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void on_actionDelete_triggered();
    void on_actionFlashcard_triggered();
    void on_actionSChinese_triggered();  // 添加中文语言切换槽函数
    void on_actionEnglish_triggered();  // 添加英文语言切换槽函数
    void on_actionTChinese_triggered();  // 添加繁体中文语言切换槽函数
    void on_actionArabic_triggered();  // 添加阿拉伯语语言切换槽函数
    void on_actionFrench_triggered();
    void on_actionTibetan_triggered();  // 添加藏文语言切换槽函数
    void on_actionUyghur_triggered();  // 添加维吾尔语语言切换槽函数
    void on_actionKazakh_triggered();  // 添加哈萨克语语言切换槽函数
    void on_actionSyriac_triggered();
    void on_actionSpanish_triggered();
    void on_actionRussian_triggered();

private:
    Ui::MainWindow *ui;
    QStandardItemModel *wordModel;
    QString currentFile;
    QSettings *settings;
    QTextToSpeech *speech;
    QStackedWidget *stackedWidget;
    PlayButtonDelegate *playDelegate;
    StatisticsWidget *statisticsWidget = nullptr;
    QMap<QString, QTranslator*> translators;
    void loadWordsFromCSV(const QString &filename);
    void saveWordsToCSV(const QString &filename);
    void changeLanguage(const QString &languageCode);  // 添加语言切换方法
};
#endif // MAINWINDOW_H
