#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "addworddialog.h"
#include "aboutdialog.h"
#include "testwidget.h"
#include "flashcard.h"
#include <QDesktopServices>
#include <functional>
#include "tutorial.h"
#include <QTranslator>
#include <QApplication> 

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , settings(new QSettings("Memorize.ini", QSettings::IniFormat, this))
    , speech(new QTextToSpeech(this))
    , statisticsWidget(nullptr)
{
    ui->setupUi(this);

    // 初始化翻译器
    QTranslator *translatorZhCN = new QTranslator(this);
    QTranslator *translatorEn = new QTranslator(this);
    QTranslator *translatorZhTW = new QTranslator(this);
    QTranslator *translatorArSA = new QTranslator(this);
    QTranslator *translatorFrFR = new QTranslator(this);
    QTranslator *translatorBoCN = new QTranslator(this);
    QTranslator *translatorUgCN = new QTranslator(this);  // 添加维吾尔语翻译器
    QTranslator *translatorKkCN = new QTranslator(this);  // 添加哈萨克语翻译器
    QTranslator *translatorSyrSY = new QTranslator(this);  // 添加叙利亚语翻译器
    QTranslator *translatorEsES = new QTranslator(this);  // 添加西班牙语翻译器
    QTranslator *translatorRuRU = new QTranslator(this);  // 添加俄语翻译器

    translators["zh_CN"] = translatorZhCN;
    translators["en"] = translatorEn;
    translators["zh_TW"] = translatorZhTW;
    translators["ar_SA"] = translatorArSA;
    translators["fr_FR"] = translatorFrFR;
    translators["bo_CN"] = translatorBoCN;
    translators["ug_CN"] = translatorUgCN;  // 添加到翻译器映射
    translators["kk_CN"] = translatorKkCN;  // 添加哈萨克语到映射
    translators["syr_SY"] = translatorSyrSY;  // 添加叙利亚语到映射
    translators["es_ES"] = translatorEsES;  // 添加西班牙语到映射
    translators["ru_RU"] = translatorRuRU;  // 添加俄语到映射

    // 加载并应用保存的语言偏好
    QString lastLanguage = settings->value("LastLanguage", "en_US").toString();
    changeLanguage(lastLanguage);

    // 创建堆栈窗口
    stackedWidget = new QStackedWidget();
    stackedWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // 创建单词列表页
    QWidget *wordListPage = new QWidget();
    QVBoxLayout *wordListLayout = new QVBoxLayout(wordListPage);
    wordListLayout->setContentsMargins(0, 0, 0, 0);
    wordListLayout->addWidget(ui->WordList);
    stackedWidget->addWidget(wordListPage);

    // 设置中央部件为堆栈窗口
    setCentralWidget(stackedWidget);

    // 设置表格模型
    wordModel = new QStandardItemModel(this);
    wordModel->setColumnCount(5);
    wordModel->setHorizontalHeaderLabels({"Word", "Phonetic", "Part of Speech", "Meaning", "Audio"});
    ui->WordList->setModel(wordModel);
    ui->WordList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->WordList->verticalHeader()->setVisible(false);
    ui->WordList->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->WordList->setEditTriggers(QAbstractItemView::DoubleClicked);

    // 初始化删除按钮为禁用状态
    ui->actionDelete->setEnabled(false);

    // 连接表格选择变化信号（在设置模型后连接）
    connect(ui->WordList->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onWordListSelectionChanged);

    // 添加委托设置 - 修复：将委托设置到第5列（索引4）
    playDelegate = new PlayButtonDelegate(this);
    ui->WordList->setItemDelegateForColumn(4, playDelegate); // 索引4是第5列
    connect(playDelegate, &PlayButtonDelegate::playClicked, [this](const QString &word) {
        speech->say(word);
    });

    // 加载上次打开的文件
    QString lastFile = settings->value("LastFile").toString();
    if(!lastFile.isEmpty() && QFile::exists(lastFile)) {
        currentFile = lastFile;
        loadWordsFromCSV(currentFile);
        setWindowTitle("Memorize - " + QFileInfo(currentFile).fileName());
    }

    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::on_actionNew_triggered);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::on_actionExit_triggered);
    connect(ui->actionHome, &QAction::triggered, this, &MainWindow::on_actionHome_triggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::showAboutDialog);
    connect(ui->actionFlashcard, &QAction::triggered, this, &MainWindow::on_actionFlashcard_triggered); // 连接flashcard按钮信号与槽


    connect(ui->actiontutorial, &QAction::triggered, this, [this]() {
        Tutorial tutorial(this);
        tutorial.exec();
    });
}

MainWindow::~MainWindow()
{
    // 删除所有翻译器对象
    for (auto translator : translators.values()) {
        delete translator;
    }
    translators.clear();

    delete ui;
}

void MainWindow::on_actionNew_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Create New Word List"), "", "CSV Files (*.csv)");
    if(fileName.isEmpty()) return;

    if(!fileName.endsWith(".csv")) fileName += ".csv";

    QFile file(fileName);
    if(file.open(QIODevice::WriteOnly)) {
        file.close();
        currentFile = fileName;
        settings->setValue("LastFile", currentFile);
        wordModel->removeRows(0, wordModel->rowCount());
        setWindowTitle("Memorize - " + QFileInfo(currentFile).fileName());
    } else {
        QMessageBox::critical(this, tr("Error"), tr("Could not create file!"));
    }
}

void MainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Word List"), "", "CSV Files (*.csv)");
    if(fileName.isEmpty()) return;

    loadWordsFromCSV(fileName);
    currentFile = fileName;
    settings->setValue("LastFile", currentFile);
    setWindowTitle("Memorize - " + QFileInfo(currentFile).fileName());
}

void MainWindow::loadWordsFromCSV(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not open file!"));
        return;
    }

    wordModel->removeRows(0, wordModel->rowCount());

    QTextStream in(&file);
    while(!in.atEnd()) {
        QString line = in.readLine();
        QStringList fields;
        bool inQuotes = false;
        QString currentField;

        // 解析CSV行（处理逗号在引号内的情况）
        for (int i = 0; i < line.length(); ++i) {
            QChar c = line[i];

            if (c == '"') {
                // 处理双引号转义（两个双引号表示一个双引号）
                if (i + 1 < line.length() && line[i+1] == '"') {
                    currentField += '"';
                    i++; // 跳过下一个双引号
                } else {
                    inQuotes = !inQuotes;
                }
            } else if (c == ',' && !inQuotes) {
                fields.append(currentField.trimmed());
                currentField.clear();
            } else {
                currentField += c;
            }
        }
        fields.append(currentField.trimmed());

        // 确保至少有4个字段（单词、音标、词性、词义）
        if(fields.size() >= 4) {
            QList<QStandardItem*> items;
            items.append(new QStandardItem(fields[0]));
            items.append(new QStandardItem(fields[1]));
            items.append(new QStandardItem(fields[2]));
            items.append(new QStandardItem(fields[3]));

            // 播放按钮列（不可编辑）
            QStandardItem *playItem = new QStandardItem();
            playItem->setFlags(Qt::ItemIsEnabled);
            items.append(playItem);

            wordModel->appendRow(items);
        }
    }

    file.close();
}

void MainWindow::saveWordsToCSV(const QString &filename)
{
    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, tr("Error"), tr("Could not save file!"));
        return;
    }

    QTextStream out(&file);
    for(int row = 0; row < wordModel->rowCount(); ++row) {
        QStringList fields;

        // 处理4个文本字段
        for (int col = 0; col < 4; ++col) {
            QString text = wordModel->item(row, col)->text();

            // 转义双引号（" -> ""）
            text.replace("\"", "\"\"");

            // 包裹在双引号中
            fields.append("\"" + text + "\"");
        }

        out << fields.join(",") << "\n";
    }
    file.close();
}

void MainWindow::on_actionExit_triggered()
{
    if(!currentFile.isEmpty()) {
        saveWordsToCSV(currentFile);
    }
    close();
}

void MainWindow::on_actionAdd_Word_triggered()
{
    AddWordDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted)
    {
        QString word = dialog.word();
        QString phonetic = dialog.phonetic();
        QString partOfSpeech = dialog.partOfSpeech();
        QString translation = dialog.translation();

        if (word.isEmpty() || translation.isEmpty())
        {
            QMessageBox::warning(this, tr("Error"), tr("Word and Translation cannot be empty!"));
            return;
        }

        QList<QStandardItem*> items;
        items.append(new QStandardItem(word));
        items.append(new QStandardItem(phonetic));
        items.append(new QStandardItem(partOfSpeech));
        items.append(new QStandardItem(translation));

        // 播放按钮列
        QStandardItem *playItem = new QStandardItem();
        playItem->setFlags(Qt::ItemIsEnabled);
        items.append(playItem);

        wordModel->appendRow(items);

        if(!currentFile.isEmpty()) {
            saveWordsToCSV(currentFile);
        }
    }
}

void MainWindow::on_playButton_clicked()
{
    int row = ui->WordList->currentIndex().row();
    if(row >= 0) {
        QString word = wordModel->item(row, 0)->text();
        speech->say(word);
    }
}

void MainWindow::on_actionTest_triggered()
{
    if(wordModel->rowCount() < 4) {
        QMessageBox::warning(this, tr("Test"), tr("You need at least 4 words to start a test!"));
        return;
    }

    // 使用TestWidget实现测试功能
    TestWidget *testWidget = new TestWidget(wordModel, this);
    stackedWidget->addWidget(testWidget);
    stackedWidget->setCurrentWidget(testWidget);

    // 连接返回主页信号
    connect(testWidget, &TestWidget::backToHome, this, [this]() {
        on_actionHome_triggered();
    });
    connect(testWidget, &TestWidget::testFinished, this, &MainWindow::saveTestResult);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!currentFile.isEmpty()) {
        saveWordsToCSV(currentFile);
    }
    event->accept();
}

void MainWindow::on_actionHome_triggered()
{
    // 返回到单词列表页（索引0）
    stackedWidget->setCurrentIndex(0);

    // 删除除单词列表页和统计页面外的所有页面
    while (stackedWidget->count() > 1) {
        QWidget *w = stackedWidget->widget(1);
        // 如果是统计页面，跳过不删除
        if (w == statisticsWidget) {
            break;
        }
        stackedWidget->removeWidget(w);
        delete w;
    }
}

PlayButtonDelegate::PlayButtonDelegate(QObject *parent) :
    QItemDelegate(parent)
{
}


// 修改后：第二个参数应为 QModelIndex
void PlayButtonDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QStyleOptionButton buttonOption;
    buttonOption.rect = option.rect.adjusted(2, 2, -2, -2);
    buttonOption.icon = QApplication::style()->standardPixmap(QStyle::SP_MediaVolume);
    buttonOption.iconSize = QSize(16, 16);
    buttonOption.text = "";
    buttonOption.state = QStyle::State_Enabled;

    // 使用Qt默认样式绘制按钮
    QApplication::style()->drawControl(QStyle::CE_PushButton, &buttonOption, painter);
}

bool PlayButtonDelegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent *e = static_cast<QMouseEvent*>(event);
        if (option.rect.adjusted(2, 2, -2, -2).contains(e->pos()))
        {
            QString word = model->data(model->index(index.row(), 0)).toString();
            emit playClicked(word);
            return true;
        }
    }
    return false;
}

void MainWindow::on_actionStatistics_triggered()
{
    // 如果统计页面尚未创建，则创建
    if (!statisticsWidget) {
        statisticsWidget = new StatisticsWidget(this);
        stackedWidget->addWidget(statisticsWidget);
    }

    // 加载最新统计数据
    statisticsWidget->loadStatistics();
    stackedWidget->setCurrentWidget(statisticsWidget);
}

void MainWindow::saveTestResult(double accuracy, int wordCount)
{
    // 确保统计页面存在
    if (!statisticsWidget) {
        statisticsWidget = new StatisticsWidget(this);
        stackedWidget->addWidget(statisticsWidget);
    }
    statisticsWidget->addTestResult(accuracy, wordCount);
}


void MainWindow::showAboutDialog()
{
    AboutDialog dialog(this);
    dialog.exec();
}

// 添加新的槽函数来打开GitHub链接
void MainWindow::on_actionTheUserWW_triggered()
{
    QDesktopServices::openUrl(QUrl("https://github.com/TheUserWW"));
}


void MainWindow::onWordListSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    // 当有选中行时启用删除按钮，否则禁用
    ui->actionDelete->setEnabled(!selected.isEmpty());
}



void MainWindow::on_actionDelete_triggered()
{
    // 获取选中的行
    QModelIndexList selectedIndexes = ui->WordList->selectionModel()->selectedRows();
    if (selectedIndexes.isEmpty()) {
        QMessageBox::information(this, tr("提示"), tr("请先选择要删除的单词"));
        return;
    }

    // 按行号降序删除，避免索引变化问题
    std::sort(selectedIndexes.begin(), selectedIndexes.end(), [](const QModelIndex &a, const QModelIndex &b) {
        return a.row() > b.row();
    });

    // 删除选中的行
    for (const QModelIndex &index : selectedIndexes) {
        wordModel->removeRow(index.row());
    }

    // 保存更改
    if (!currentFile.isEmpty()) {
        saveWordsToCSV(currentFile);
    }
}

void MainWindow::on_actionFlashcard_triggered()
{
    if(wordModel->rowCount() == 0) {
        QMessageBox::warning(this, tr("Flashcard"), tr("No words available! Please add words first."));
        return;
    }

    FlashcardSetupWidget *setupWidget = new FlashcardSetupWidget(wordModel);
    stackedWidget->addWidget(setupWidget);
    stackedWidget->setCurrentWidget(setupWidget);

    connect(setupWidget, &FlashcardSetupWidget::backToHome, this, [this]() {
        on_actionHome_triggered();
    });

    connect(setupWidget, &FlashcardSetupWidget::startPractice, this, [this](int startIdx, int endIdx) {
        qDebug() << "Starting flashcard practice with range:" << startIdx << "to" << endIdx;

        // 确保移除旧的小部件
        if(stackedWidget->count() > 1) {
            QWidget *oldWidget = stackedWidget->widget(1);
            if(oldWidget) {
                stackedWidget->removeWidget(oldWidget);
                delete oldWidget;
            }
        }

        FlashcardDisplayWidget *displayWidget = new FlashcardDisplayWidget(wordModel, startIdx, endIdx);
        stackedWidget->addWidget(displayWidget);
        stackedWidget->setCurrentWidget(displayWidget);

        connect(displayWidget, &FlashcardDisplayWidget::backToHome, this, [this]() {
            on_actionHome_triggered();
        });
    });
}


void MainWindow::on_actionSChinese_triggered()
{
    changeLanguage("zh_CN");
    settings->setValue("LastLanguage", "zh_CN");
}

void MainWindow::on_actionEnglish_triggered()
{
    changeLanguage("en");
    settings->setValue("LastLanguage", "en");
}

void MainWindow::on_actionTChinese_triggered()
{
    changeLanguage("zh_TW");
    settings->setValue("LastLanguage", "zh_TW");
}

void MainWindow::on_actionArabic_triggered()
{
    changeLanguage("ar_SA");
    settings->setValue("LastLanguage", "ar_SA");
}

void MainWindow::on_actionFrench_triggered()
{
    changeLanguage("fr_FR");
    settings->setValue("LastLanguage", "fr_FR");
}

void MainWindow::on_actionTibetan_triggered()
{
    changeLanguage("bo_CN");
    settings->setValue("LastLanguage", "bo_CN");
}

void MainWindow::on_actionUyghur_triggered()
{
    changeLanguage("ug_CN");
    settings->setValue("LastLanguage", "ug_CN");
}

void MainWindow::on_actionKazakh_triggered()
{
    changeLanguage("kk_CN");
    settings->setValue("LastLanguage", "kk_CN");
}

void MainWindow::on_actionSyriac_triggered()
{
    changeLanguage("syr_SY");
    settings->setValue("LastLanguage", "syr_SY");
}

void MainWindow::on_actionSpanish_triggered()
{
    changeLanguage("es_ES");
    settings->setValue("LastLanguage", "es_ES");
}

// 添加俄语槽函数实现
void MainWindow::on_actionRussian_triggered()
{
    changeLanguage("ru_RU");
    settings->setValue("LastLanguage", "ru_RU");
}

void MainWindow::changeLanguage(const QString &languageCode)
{
    // 移除当前所有翻译器
    for (auto translator : translators.values()) {
        qApp->removeTranslator(translator);
    }

    // 加载新翻译
    if (translators.contains(languageCode)) {
        QTranslator *translator = translators[languageCode];
        QString qmFile;

        if (languageCode == "zh_CN") {
            qmFile = "Memorize_zh_CN.qm";
        } else if (languageCode == "en") {
            qmFile = "Memorize_en_US.qm";
        } else if (languageCode == "zh_TW") {
            qmFile = "Memorize_zh_TW.qm";
        } else if (languageCode == "ar_SA") {
            qmFile = "Memorize_ar_SA.qm";
        } else if (languageCode == "fr_FR") {
            qmFile = "Memorize_fr_FR.qm";
        } else if (languageCode == "bo_CN") {
            qmFile = "Memorize_bo_CN.qm";
        } else if (languageCode == "ug_CN") {
            qmFile = "Memorize_ug_CN.qm";
        } else if (languageCode == "kk_CN") {
            qmFile = "Memorize_kk_CN.qm";
        } else if (languageCode == "syr_SY") {
            qmFile = "Memorize_syr_SY.qm";
        } else if (languageCode == "es_ES") {
            qmFile = "Memorize_es_ES.qm";
        } else if (languageCode == "ru_RU") {
            qmFile = "Memorize_ru_RU.qm";
        }
        if (translator->load(qmFile, qApp->applicationDirPath())) {
            qApp->installTranslator(translator);
        } else {
            qDebug() << "Failed to load translation file for" << languageCode;
        }
    }

    // 重新翻译UI
    ui->retranslateUi(this);

    // 更新窗口标题
    if (!currentFile.isEmpty()) {
        setWindowTitle(tr("Memorize - ") + QFileInfo(currentFile).fileName());
    } else {
        setWindowTitle(tr("Memorize"));
    }

    // 如果统计窗口存在，也需要重新翻译
    if (statisticsWidget) {
        // 假设StatisticsWidget有retranslate方法
        // statisticsWidget->retranslateUi();
    }
}
