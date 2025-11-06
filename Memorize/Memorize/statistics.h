#ifndef STATISTICS_H
#define STATISTICS_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QScatterSeries>
#include <QtCharts/QChartView>
#include <QSettings>

QT_USE_NAMESPACE

    namespace Ui {
    class StatisticsWidget;
}

class StatisticsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsWidget(QWidget *parent = nullptr);
    ~StatisticsWidget();

    void loadStatistics();
    void saveStatistics();
    void addTestResult(double accuracy, int wordCount);
    void initChart();

signals:
    void backToHome();

private slots:
    void onBackClicked();
    void on_ResetButton_clicked(); // 添加重置按钮槽函数声明

private:
    Ui::StatisticsWidget *ui;
    QSettings *settings;
    QLineSeries *accuracySeries;
    QScatterSeries *scatterSeries;
    QChart *chart;
    QChartView *chartView;

    double highestAccuracy = 0.0;
    double cumulativeAccuracy = 0.0;
    int totalTests = 0;
    int totalWords = 0;

    // 添加以下两个私有函数声明
    void updateUI();
    void adjustChartRange();
};

#endif // STATISTICS_H
