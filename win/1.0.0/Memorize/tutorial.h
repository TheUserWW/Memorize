#ifndef TUTORIAL_H
#define TUTORIAL_H

#include <QDialog>
#include <QVector>
#include <QString>

namespace Ui {
class Tutorial;
}

class Tutorial : public QDialog
{
    Q_OBJECT

public:
    explicit Tutorial(QWidget *parent = nullptr);
    ~Tutorial();

private slots:
    void on_nextButton_clicked();
    void on_prevButton_clicked();
    void on_closeButton_clicked();

private:
    Ui::Tutorial *ui;
    int currentPage;
    QVector<QString> pageTitles;
    QVector<QString> pageContents;
    void updatePageContent();
};

#endif // TUTORIAL_H
