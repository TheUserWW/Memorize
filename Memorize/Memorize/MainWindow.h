#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "statistics.h"
#include <QMainWindow>
#include <QStandardItemModel>
#include <QSettings>
#include <QTextToSpeech>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
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
#include <QTranslator>
#include <QMap>
#include <QActionGroup>

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
	void on_actionMore_Resources_triggered();
    void onWordListSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void on_actionDelete_triggered();
    void on_actionFlashcard_triggered();
    void changeLanguage(QAction *action);

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
    QActionGroup *languageActionGroup;

    void loadWordsFromCSV(const QString &filename);
    void saveWordsToCSV(const QString &filename);
    void setupLanguageActions();
    void updateMenuStates();
};
#endif // MAINWINDOW_H
