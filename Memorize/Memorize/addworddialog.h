#ifndef ADDWORDDIALOG_H
#define ADDWORDDIALOG_H

#include <QDialog>

namespace Ui {
class AddWordDialog;
}

class AddWordDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddWordDialog(QWidget *parent = nullptr);
    ~AddWordDialog();

    QString word() const;
    QString phonetic() const;
    QString partOfSpeech() const;
    QString translation() const;

private:
    Ui::AddWordDialog *ui;
};

#endif // ADDWORDDIALOG_H
