#ifndef TESTWIDGET_H
#define TESTWIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QTextToSpeech>
#include <QTimer>
#include <QStackedWidget>
#include <QButtonGroup>
#include <QTime>

namespace Ui {
class TestWidget;
}

class TestWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TestWidget(QStandardItemModel *model, QWidget *parent = nullptr);
    ~TestWidget();

signals:
    void backToHome();
    void testFinished(double accuracy, int wordCount);

private slots:
    void startTest();
    void updateQuestion();
    void checkAnswer(QAbstractButton *button);
    void submitSpelling();
    void updateTimer();
    void showResults();
    void restartTest();

private:
    Ui::TestWidget *ui;
    QStandardItemModel *wordModel;
    QTextToSpeech *speech;
    QTimer *timer;
    QButtonGroup *optionsGroup;
    int testMode;
    int questionCount;
    int timeLeft;
    int currentQuestion;
    int correctAnswers;
    QString currentWord;
    QString currentMeaning;
    QTime startTime;

    // 修改：使用结构体存储测试历史
    struct TestResult {
        QString word;
        QString meaning;
        QString userAnswer;
        bool isCorrect;
    };
    QList<TestResult> testHistory;

    void clearOptions();
};

#endif // TESTWIDGET_H
