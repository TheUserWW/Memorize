#include "testwidget.h"
#include "ui_testwidget.h"
#include <QMessageBox>
#include <QFont>
#include <QSpinBox>
#include <QApplication>
#include <QRandomGenerator>
#include <algorithm>

TestWidget::TestWidget(QStandardItemModel *model, QWidget *parent)
    : QWidget(parent),
    ui(new Ui::TestWidget),
    wordModel(model),
    speech(new QTextToSpeech(this)),
    timer(new QTimer(this)),
    optionsGroup(new QButtonGroup(this)),
    testMode(0),
    questionCount(10),
    timeLeft(60),
    currentQuestion(0),
    correctAnswers(0)
{
    if (!wordModel) {
        QMessageBox::critical(this, tr("Error"), tr("Invalid word model!"));
        close();
        return;
    }
    ui->setupUi(this);

    // 在构造函数中设置测试模式的用户数据
    ui->modeCombo->setItemData(0, 0);  // Word->Meaning
    ui->modeCombo->setItemData(1, 1);  // Meaning->Word
    ui->modeCombo->setItemData(2, 2);  // Meaning->Spelling
    ui->modeCombo->setItemData(3, 3);  // 新增：Typing测试

    // 一次性连接所有信号
    connect(ui->startButton, &QPushButton::clicked, this, &TestWidget::startTest);
    connect(ui->backButton, &QPushButton::clicked, this, &TestWidget::backToHome);
    connect(ui->homeButton, &QPushButton::clicked, this, &TestWidget::backToHome);
    connect(ui->submitButton, &QPushButton::clicked, this, &TestWidget::submitSpelling);
    connect(ui->restartButton, &QPushButton::clicked, this, &TestWidget::restartTest);
    connect(ui->resultsBackButton, &QPushButton::clicked, this, &TestWidget::backToHome);
    connect(timer, &QTimer::timeout, this, &TestWidget::updateTimer);

    // 初始化UI状态
    ui->stackedWidget->setCurrentWidget(ui->setupPage);
    ui->spellingInput->setVisible(false);
    ui->submitButton->setVisible(false);
}

TestWidget::~TestWidget()
{
    timer->stop();
    clearOptions();
    delete optionsGroup;
    delete speech;
    delete ui;
}

void TestWidget::clearOptions()
{
    // 安全清除所有选项按钮
    QList<QAbstractButton*> buttons = optionsGroup->buttons();
    for (QAbstractButton* button : buttons) {
        optionsGroup->removeButton(button);
        button->deleteLater();
    }

    // 清除布局中的项目
    QLayoutItem* item;
    while ((item = ui->optionsLayout->takeAt(0))) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void TestWidget::startTest()
{
    if (wordModel->rowCount() < 4) {
        QMessageBox::warning(this, tr("Test"), tr("You need at least 4 words to start a test!"));
        return;
    }

    // 重置测试状态
    currentQuestion = 0;
    correctAnswers = 0;
    testHistory.clear();  // 清空测试历史
    startTime = QTime::currentTime();

    // 获取测试参数
    testMode = ui->modeCombo->currentData().toInt();
    questionCount = ui->countSpin->value();
    timeLeft = ui->timeSpin->value();

    // 设置定时器
    if (timeLeft > 0) {
        ui->timerLabel->setText(tr("Time left: %1 seconds").arg(timeLeft));
        timer->start(1000);
    } else {
        ui->timerLabel->setText(tr("No time limit"));
        timer->stop();
    }

    // 切换到测试页面
    ui->stackedWidget->setCurrentWidget(ui->testPage);
    updateQuestion();
}

void TestWidget::updateQuestion()
{
    if (currentQuestion >= questionCount) {
        showResults();
        return;
    }

    ui->progressLabel->setText(QString("Question %1/%2").arg(currentQuestion + 1).arg(questionCount));

    // 随机选择单词
    int wordIndex = QRandomGenerator::global()->bounded(wordModel->rowCount());
    currentWord = wordModel->item(wordIndex, 0)->text(); // 第0列: 单词
    currentMeaning = wordModel->item(wordIndex, 3)->text(); // 第3列: 词义

    // 清除之前的选项
    clearOptions();
    disconnect(optionsGroup, nullptr, this, nullptr);

    // 根据测试模式设置UI
    switch (testMode) {
    case 0: { // Word -> Meaning
        ui->questionLabel->setText(currentWord);
        ui->spellingInput->setVisible(false);
        ui->submitButton->setVisible(false);

        // 创建选项按钮
        QVector<QString> options;
        options.append(currentMeaning);

        // 添加3个错误选项
        for (int i = 0; i < 3 && wordModel->rowCount() > 1; i++) {
            int wrongIndex;
            do {
                wrongIndex = QRandomGenerator::global()->bounded(wordModel->rowCount());
            } while (wrongIndex == wordIndex);
            options.append(wordModel->item(wrongIndex, 3)->text()); // 第3列: 词义
        }

        // 打乱选项顺序
        std::shuffle(options.begin(), options.end(), *QRandomGenerator::global());

        // 添加按钮到布局
        for (const QString &option : options) {
            QPushButton *button = new QPushButton(option);
            ui->optionsLayout->addWidget(button);
            optionsGroup->addButton(button);
        }
        break;
    }

    case 1: { // Meaning -> Word
        ui->questionLabel->setText(currentMeaning);
        ui->spellingInput->setVisible(false);
        ui->submitButton->setVisible(false);

        QVector<QString> wordOptions;
        wordOptions.append(currentWord);

        for (int i = 0; i < 3 && wordModel->rowCount() > 1; i++) {
            int wrongIndex;
            do {
                wrongIndex = QRandomGenerator::global()->bounded(wordModel->rowCount());
            } while (wrongIndex == wordIndex);
            wordOptions.append(wordModel->item(wrongIndex, 0)->text()); // 第0列: 单词
        }

        std::shuffle(wordOptions.begin(), wordOptions.end(), *QRandomGenerator::global());

        for (const QString &option : wordOptions) {
            QPushButton *button = new QPushButton(option);
            ui->optionsLayout->addWidget(button);
            optionsGroup->addButton(button);
        }
        break;
    }

    case 2: { // Spelling Test
        ui->questionLabel->setText(currentMeaning);
        ui->spellingInput->setVisible(true);
        ui->submitButton->setVisible(true);
        ui->spellingInput->clear();
        ui->spellingInput->setFocus();
        break;
    }

    case 3: { // 新增：Typing测试
        ui->questionLabel->setText("Listen and type the word:");
        // 播放单词发音
        speech->say(currentWord);
        ui->spellingInput->setVisible(true);
        ui->submitButton->setVisible(true);
        ui->spellingInput->clear();
        ui->spellingInput->setFocus();
        break;
    }
    }

    // 连接信号（拼写测试不需要选项按钮的信号）
    if (testMode != 2 && testMode != 3) {
        connect(optionsGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                this, &TestWidget::checkAnswer);
    }
}

void TestWidget::checkAnswer(QAbstractButton *button)
{
    if (!button) return;

    bool isCorrect = false;
    if (testMode == 0) {
        isCorrect = (button->text() == currentMeaning);
    } else if (testMode == 1) {
        isCorrect = (button->text() == currentWord);
    }

    // 记录测试结果
    testHistory.append({currentWord, currentMeaning, button->text(), isCorrect});

    if (isCorrect) {
        correctAnswers++;
    }

    currentQuestion++;
    updateQuestion();
}

void TestWidget::submitSpelling()
{
    QString userAnswer = ui->spellingInput->text().trimmed();
    bool isCorrect = false;

    if (testMode == 2 || testMode == 3) {
        // 移除用户答案中的双引号进行比较
        QString cleanAnswer = userAnswer;
        cleanAnswer.remove("\"");
        isCorrect = (cleanAnswer.compare(currentWord, Qt::CaseInsensitive) == 0);
    }

    // 记录测试结果
    testHistory.append({currentWord, currentMeaning, userAnswer, isCorrect});

    if (isCorrect) {
        correctAnswers++;
    }

    currentQuestion++;
    updateQuestion();
}

void TestWidget::updateTimer()
{
    timeLeft--;
    ui->timerLabel->setText(tr("Time left: %1 seconds").arg(timeLeft));

    if (timeLeft <= 0) {
        timer->stop();
        showResults();
    }
}

void TestWidget::showResults()
{
    // 创建数据模型
    QStandardItemModel *resultModel = new QStandardItemModel(this);
    resultModel->setColumnCount(4);  // 增加列数
    resultModel->setHorizontalHeaderLabels({tr("Word"), tr("Meaning"), tr("Your Answer"), tr("Result")});

    // 填充测试数据
    for (const TestResult &result : testHistory) {
        QList<QStandardItem*> row;
        row << new QStandardItem(result.word);
        row << new QStandardItem(result.meaning);
        row << new QStandardItem(result.userAnswer);

        // 添加结果列（✓ 或 ✗）
        QStandardItem *resultItem = new QStandardItem(result.isCorrect ? "✓" : "✗");
        resultItem->setTextAlignment(Qt::AlignCenter);
        if (result.isCorrect) {
            resultItem->setForeground(QBrush(Qt::darkGreen));
        } else {
            resultItem->setForeground(QBrush(Qt::red));
        }
        row << resultItem;

        resultModel->appendRow(row);
    }

    // 配置表格视图
    ui->resultTableView->setModel(resultModel);
    // 添加这行代码禁用编辑功能
    ui->resultTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->resultTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // 设置列宽策略
    ui->resultTableView->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->resultTableView->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->resultTableView->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->resultTableView->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    timer->stop();
    int timeUsed = startTime.secsTo(QTime::currentTime());

    // 更新结果页面
    ui->scoreLabel->setText(QString("Score: %1/%2").arg(correctAnswers).arg(questionCount));

    double percentage = questionCount > 0 ? (100.0 * correctAnswers / questionCount) : 0;
    ui->percentLabel->setText(QString("Percentage: %1%").arg(percentage, 0, 'f', 1));

    ui->timeUsedLabel->setText(QString("Time used: %1 seconds").arg(timeUsed));

    // 切换到结果页面
    ui->stackedWidget->setCurrentWidget(ui->resultsPage);

    // 发射测试完成信号
    emit testFinished(percentage, questionCount);
}

void TestWidget::restartTest()
{
    timer->stop();
    ui->stackedWidget->setCurrentWidget(ui->setupPage);
}
