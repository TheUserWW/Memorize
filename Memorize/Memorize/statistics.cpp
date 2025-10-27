#include "statistics.h"
#include "ui_statistics.h"
#include <QMessageBox> // 添加消息框头文件
#include <QDateTime>
#include <QStyle>
#include <QtCharts/QDateTimeAxis>
#include <QtCharts/QValueAxis>
#include <QApplication>
#include <QStandardPaths> // 添加QStandardPaths头文件

StatisticsWidget::StatisticsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsWidget),
    settings(new QSettings(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/Memorize.ini", QSettings::IniFormat, this))
{
    ui->setupUi(this);

    // 设置重置按钮图标
    ui->ResetButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));

    // 初始化图表
    initChart();

    // 将图表视图添加到布局
    ui->chartLayout->addWidget(chartView);

    // 加载统计数据
    loadStatistics();
}

StatisticsWidget::~StatisticsWidget()
{
    saveStatistics();
    delete ui;
}

void StatisticsWidget::initChart()
{
    // 创建图表对象
    chart = new QChart();
    chart->setTitle(tr("Test Accuracy History"));
    chart->setTheme(QChart::ChartThemeLight);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // 创建数据系列
    accuracySeries = new QLineSeries();
    accuracySeries->setName(tr("Accuracy"));
    chart->addSeries(accuracySeries);

    // 创建X轴（时间轴）
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setFormat("MMM d");
    axisX->setTitleText(tr("Test Date"));
    axisX->setTickCount(5);
    chart->addAxis(axisX, Qt::AlignBottom);
    accuracySeries->attachAxis(axisX);

    // 创建Y轴（百分比轴）
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 100);
    axisY->setTitleText("Accuracy (%)");
    axisY->setLabelFormat("%.0f%%");
    axisY->setTickCount(6);
    chart->addAxis(axisY, Qt::AlignLeft);
    accuracySeries->attachAxis(axisY);

    // 创建图表视图
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
}

void StatisticsWidget::loadStatistics()
{
    settings->beginGroup("Statistics");

    // 加载基本统计数据
    totalTests = settings->value("totalTests", 0).toInt();
    totalWords = settings->value("totalWords", 0).toInt();
    cumulativeAccuracy = settings->value("cumulativeAccuracy", 0.0).toDouble();
    highestAccuracy = settings->value("highestAccuracy", 0.0).toDouble();

    // 清除现有数据点
    accuracySeries->clear();

    // 加载历史测试数据
    int size = settings->beginReadArray("history");
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QDateTime date = settings->value("date").toDateTime();
        double accuracy = settings->value("accuracy").toDouble();
        accuracySeries->append(date.toMSecsSinceEpoch(), accuracy);
    }
    settings->endArray();

    settings->endGroup();

    // 更新UI显示
    updateUI();

    // 调整图表范围
    adjustChartRange();
}

void StatisticsWidget::saveStatistics()
{
    settings->beginGroup("Statistics");

    // 保存基本统计数据
    settings->setValue("totalTests", totalTests);
    settings->setValue("totalWords", totalWords);
    settings->setValue("cumulativeAccuracy", cumulativeAccuracy);
    settings->setValue("highestAccuracy", highestAccuracy);

    // 使用QSettings数组正确保存历史记录
    settings->beginWriteArray("history");
    for (int i = 0; i < accuracySeries->count(); ++i) {
        settings->setArrayIndex(i);
        settings->setValue("date", QDateTime::fromMSecsSinceEpoch(accuracySeries->at(i).x()));
        settings->setValue("accuracy", accuracySeries->at(i).y());
    }
    settings->endArray();

    settings->endGroup();
}

void StatisticsWidget::addTestResult(double accuracy, int wordCount)
{
    // 更新统计数据
    totalTests++;
    totalWords += wordCount;

    // 更新累计正确率
    double prevTotal = cumulativeAccuracy * (totalTests - 1);
    cumulativeAccuracy = (prevTotal + accuracy) / totalTests;

    // 更新最高正确率
    if (accuracy > highestAccuracy) {
        highestAccuracy = accuracy;
    }

    // 添加新数据点
    QDateTime now = QDateTime::currentDateTime();
    accuracySeries->append(now.toMSecsSinceEpoch(), accuracy);

    // 调整图表范围
    adjustChartRange();

    // 保存统计数据
    saveStatistics();

    // 更新UI显示
    updateUI();

    // 添加数据后需要更新图表
    chart->update();
    chartView->repaint();
}

void StatisticsWidget::updateUI()
{
    // 更新统计标签
    ui->totalTestsLabel->setText(QString::number(totalTests));
    ui->totalWordsLabel->setText(QString::number(totalWords));
    ui->cumulativeLabel->setText(QString::number(cumulativeAccuracy, 'f', 1) + "%");
    ui->highestLabel->setText(QString::number(highestAccuracy, 'f', 1) + "%");
}

void StatisticsWidget::adjustChartRange()
{
    if (accuracySeries->count() > 0) {
        // 获取最早和最晚的日期
        QDateTime minDate = QDateTime::fromMSecsSinceEpoch(accuracySeries->at(0).x());
        QDateTime maxDate = QDateTime::fromMSecsSinceEpoch(accuracySeries->at(accuracySeries->count()-1).x());

        // 扩展时间范围使数据点更清晰
        minDate = minDate.addDays(-1);
        maxDate = maxDate.addDays(1);

        // 设置X轴范围（使用Qt 6的新API）
        QList<QAbstractAxis*> xAxes = chart->axes(Qt::Horizontal);
        if (!xAxes.isEmpty()) {
            QDateTimeAxis* axisX = qobject_cast<QDateTimeAxis*>(xAxes.first());
            if (axisX) {
                axisX->setMin(minDate);
                axisX->setMax(maxDate);
            }
        }

        // 确保Y轴显示0-100%范围
        QList<QAbstractAxis*> yAxes = chart->axes(Qt::Vertical);
        if (!yAxes.isEmpty()) {
            QValueAxis* axisY = qobject_cast<QValueAxis*>(yAxes.first());
            if (axisY) {
                axisY->setRange(0, 100);
            }
        }
    }
}

void StatisticsWidget::onBackClicked()
{
    emit backToHome();
}

void StatisticsWidget::on_ResetButton_clicked()
{
    // 显示确认对话框
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Confirm Reset"), 
                                  tr("Are you sure you want to delete all statistics data?"),
                                  QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // 清除所有统计数据
        totalTests = 0;
        totalWords = 0;
        cumulativeAccuracy = 0.0;
        highestAccuracy = 0.0;
        accuracySeries->clear();

        // 保存清空后的数据并更新UI
        saveStatistics();
        updateUI();
        adjustChartRange();
        chart->update();
        chartView->repaint();
    }
}
