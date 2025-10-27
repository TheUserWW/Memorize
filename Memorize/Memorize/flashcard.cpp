#include "flashcard.h"
#include "ui_flashcardsetup.h"
#include "ui_flashcarddisplay.h"
#include <QMessageBox>
#include <QRandomGenerator>
#include <QFont>
#include <QDebug>
#include <QTextToSpeech>
#include <QMouseEvent>

// FlashcardSetupWidget 实现
FlashcardSetupWidget::FlashcardSetupWidget(QStandardItemModel *model, QWidget *parent)
    : QWidget(parent), ui(new Ui::FlashcardSetupWidget), wordModel(model)
{
    ui->setupUi(this);
    if (!wordModel) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid word model!"));
        close();
        return;
    }
    if (wordModel) {
        // 将索引改为1-based
        int maxRow = wordModel->rowCount();
        ui->lowerBoundSpin->setRange(1, maxRow);
        ui->upperBoundSpin->setRange(1, maxRow);
        ui->upperBoundSpin->setValue(qMin(10, maxRow));
    }
}
FlashcardSetupWidget::~FlashcardSetupWidget()
{
    delete ui;
}
void FlashcardSetupWidget::on_generateButton_clicked()
{
    // 转换为0-based索引
    int startIdx = ui->lowerBoundSpin->value() - 1;
    int endIdx = ui->upperBoundSpin->value() - 1;
    if (startIdx > endIdx) {
        QMessageBox::warning(this, tr("invalid input"), tr("The starting index cannot be greater than the ending index."));
        return;
    }
    // 添加索引边界检查
    int maxValidIndex = wordModel->rowCount() - 1;
    if (endIdx > maxValidIndex) {
        endIdx = maxValidIndex;
        ui->upperBoundSpin->setValue(endIdx + 1); // 加1恢复为1-based显示
    }
    emit startPractice(startIdx, endIdx);
}
void FlashcardSetupWidget::on_backButton_clicked()
{
    emit backToHome();
}

// FlashcardDisplayWidget 实现
FlashcardDisplayWidget::FlashcardDisplayWidget(QStandardItemModel *model, int startIdx, int endIdx, QWidget *parent)
    : QWidget(parent), ui(new Ui::FlashcardDisplayWidget), wordModel(model),
    currentIndex(0), isFlipped(false), isAnimating(false)
{
    ui->setupUi(this);
    // 设置按钮图标
    ui->prevButton->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    ui->prevButton->setText("");
    ui->nextButton->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    ui->nextButton->setText("");
    // 设置卡片框架可点击
    ui->cardFrame->setAttribute(Qt::WA_Hover);
    ui->cardFrame->installEventFilter(this);
    // 创建卡片容器
    QHBoxLayout *cardLayout = new QHBoxLayout(ui->cardFrame);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(0);
    // 创建正面卡片
    frontCard = new QWidget(ui->cardFrame);
    QVBoxLayout *frontLayout = new QVBoxLayout(frontCard);
    frontLayout->setSpacing(20);
    frontLayout->setContentsMargins(30, 30, 30, 30);
    wordLabelFront = new QLabel(frontCard);
    wordLabelFront->setAlignment(Qt::AlignCenter);
    wordLabelFront->setFont(QFont("Arial", 24, QFont::Bold));
    playButton = new QPushButton(frontCard);
    playButton->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    playButton->setIconSize(QSize(16, 16));
    playButton->setFixedSize(120, 40);
    frontLayout->addWidget(wordLabelFront);
    frontLayout->addWidget(playButton, 0, Qt::AlignCenter);
    // 创建反面卡片
    backCard = new QWidget(ui->cardFrame);
    QVBoxLayout *backLayout = new QVBoxLayout(backCard);
    backLayout->setSpacing(15);
    backLayout->setContentsMargins(30, 30, 30, 30);
    wordLabelBack = new QLabel(backCard);
    wordLabelBack->setAlignment(Qt::AlignCenter);
    wordLabelBack->setFont(QFont("Arial", 20, QFont::Bold));
    phoneticLabel = new QLabel(backCard);
    phoneticLabel->setAlignment(Qt::AlignCenter);
    phoneticLabel->setFont(QFont("Arial", 16));
    posLabel = new QLabel(backCard);
    posLabel->setAlignment(Qt::AlignCenter);
    posLabel->setFont(QFont("Arial", 16));
    posLabel->setStyleSheet("color: #555;");
    translationLabel = new QLabel(backCard);
    translationLabel->setAlignment(Qt::AlignCenter);
    translationLabel->setFont(QFont("Arial", 18));
    translationLabel->setWordWrap(true);
    backLayout->addWidget(wordLabelBack);
    backLayout->addWidget(phoneticLabel);
    backLayout->addWidget(posLabel);
    backLayout->addWidget(translationLabel);
    // 添加到卡片容器
    cardLayout->addWidget(frontCard);
    cardLayout->addWidget(backCard);
    backCard->hide();
    // 设置透明度效果
    frontEffect = new QGraphicsOpacityEffect(this);
    frontCard->setGraphicsEffect(frontEffect);
    frontEffect->setOpacity(1.0);
    backEffect = new QGraphicsOpacityEffect(this);
    backCard->setGraphicsEffect(backEffect);
    backEffect->setOpacity(0.0);
    // 创建翻转动画
    flipAnimation = new QPropertyAnimation(this, "flipProgress", this);
    flipAnimation->setDuration(500);
    flipAnimation->setEasingCurve(QEasingCurve::InOutQuad);
    // 连接信号
    connect(playButton, &QPushButton::clicked, this, &FlashcardDisplayWidget::playWord);
    connect(ui->backButton, &QPushButton::clicked, this, &FlashcardDisplayWidget::on_backButton_clicked);
    // 生成单词卡
    generateFlashcards(startIdx, endIdx);
}
FlashcardDisplayWidget::~FlashcardDisplayWidget()
{
    // 停止动画并断开所有信号连接
    if (flipAnimation) {
        flipAnimation->stop();
        flipAnimation->disconnect();
    }
    delete ui;
}
// 添加事件过滤器处理点击事件
bool FlashcardDisplayWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->cardFrame && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton) {
            flipCard();
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}
void FlashcardDisplayWidget::generateFlashcards(int startIdx, int endIdx)
{
    flashcards.clear();
    for (int i = startIdx; i <= endIdx; ++i) {
        Flashcard card;
        card.word = wordModel->item(i, 0)->text();
        card.phonetic = wordModel->item(i, 1)->text();
        card.pos = wordModel->item(i, 2)->text();
        card.translation = wordModel->item(i, 3)->text();
        flashcards.append(card);
    }
    displayCurrentFlashcard();
}
void FlashcardDisplayWidget::displayCurrentFlashcard()
{
    if (flashcards.isEmpty() || currentIndex < 0 || currentIndex >= flashcards.size())
        return;
    const Flashcard &card = flashcards[currentIndex];
    // 更新正面卡片
    wordLabelFront->setText(card.word);
    // 更新反面卡片
    wordLabelBack->setText(card.word);
    phoneticLabel->setText("[" + card.phonetic + "]");
    posLabel->setText(card.pos);
    translationLabel->setText(card.translation);
    // 确保卡片在正确状态
    if (isFlipped) {
        frontCard->hide();
        backCard->show();
    } else {
        frontCard->show();
        backCard->hide();
    }
    ui->statusLabel->setText(tr("Card %1 of %2").arg(currentIndex + 1).arg(flashcards.size()));
}
qreal FlashcardDisplayWidget::flipProgress() const
{
    return frontEffect->opacity();
}
void FlashcardDisplayWidget::setFlipProgress(qreal progress)
{
    frontEffect->setOpacity(1.0 - progress);
    backEffect->setOpacity(progress);
    if (progress >= 0.5 && !isFlipped) {
        frontCard->hide();
        backCard->show();
        isFlipped = true;
    } else if (progress < 0.5 && isFlipped) {
        backCard->hide();
        frontCard->show();
        isFlipped = false;
    }
}
void FlashcardDisplayWidget::flipCard()
{
    if (isAnimating) return;
    isAnimating = true;
    flipAnimation->stop();
    if (isFlipped) {
        flipAnimation->setStartValue(1.0);
        flipAnimation->setEndValue(0.0);
    } else {
        flipAnimation->setStartValue(0.0);
        flipAnimation->setEndValue(1.0);
    }
    flipAnimation->start();
    connect(flipAnimation, &QPropertyAnimation::finished, this, [this]() {
        isAnimating = false;
    });
}
void FlashcardDisplayWidget::playWord()
{
    if (flashcards.isEmpty() || currentIndex < 0 || currentIndex >= flashcards.size())
        return;
    static QTextToSpeech *speech = nullptr;
    if (!speech)
        speech = new QTextToSpeech(this);
    speech->say(flashcards[currentIndex].word);
}
void FlashcardDisplayWidget::on_nextButton_clicked()
{
    if (!flashcards.isEmpty() && currentIndex < flashcards.size() - 1) {
        currentIndex++;
        isFlipped = false;
        displayCurrentFlashcard();
    }
}

void FlashcardDisplayWidget::on_prevButton_clicked()
{
    if (!flashcards.isEmpty() && currentIndex > 0) {
        currentIndex--;
        isFlipped = false;
        displayCurrentFlashcard();
    }
}
void FlashcardDisplayWidget::on_backButton_clicked()
{
    emit backToHome();
}
