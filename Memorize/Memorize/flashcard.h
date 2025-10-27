#ifndef FLASHCARDWIDGET_H
#define FLASHCARDWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QStandardItemModel>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QStyle>

namespace Ui {
class FlashcardSetupWidget;
class FlashcardDisplayWidget;
}

class FlashcardSetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlashcardSetupWidget(QStandardItemModel *model, QWidget *parent = nullptr);
    ~FlashcardSetupWidget();

signals:
    void startPractice(int startIndex, int endIndex);
    void backToHome();

private slots:
    void on_generateButton_clicked();
    void on_backButton_clicked();

private:
    Ui::FlashcardSetupWidget *ui;
    QStandardItemModel *wordModel;
};

struct Flashcard {
    QString word;
    QString phonetic;
    QString pos;
    QString translation;
};

class FlashcardDisplayWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(qreal flipProgress READ flipProgress WRITE setFlipProgress)

public:
    explicit FlashcardDisplayWidget(QStandardItemModel *model, int startIdx, int endIdx, QWidget *parent = nullptr);
    ~FlashcardDisplayWidget();

    qreal flipProgress() const;
    void setFlipProgress(qreal progress);
    bool eventFilter(QObject *obj, QEvent *event); // Add this declaration

signals:
    void backToHome();

private slots:
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_backButton_clicked();
    void flipCard();  // 添加翻转方法
    void playWord();  // 添加播放方法

private:
    Ui::FlashcardDisplayWidget *ui;
    QStandardItemModel *wordModel;
    QList<Flashcard> flashcards;
    int currentIndex;
    bool isFlipped;
    bool isAnimating;

    // Animation
    QPropertyAnimation *flipAnimation;
    QGraphicsOpacityEffect *frontEffect;
    QGraphicsOpacityEffect *backEffect;
    QWidget *frontCard;
    QWidget *backCard;

    // Card widgets
    QLabel *wordLabelFront;
    QPushButton *playButton;
    QLabel *wordLabelBack;
    QLabel *phoneticLabel;
    QLabel *posLabel;
    QLabel *translationLabel;

    void generateFlashcards(int startIdx, int endIdx);
    void displayCurrentFlashcard();
};

#endif // FLASHCARDWIDGET_H
