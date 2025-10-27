#include "addworddialog.h"
#include "ui_addworddialog.h"

AddWordDialog::AddWordDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddWordDialog)
{
    ui->setupUi(this);
    ui->wordLineEdit->setFocus();
}

AddWordDialog::~AddWordDialog()
{
    delete ui;
}

QString AddWordDialog::word() const
{
    return ui->wordLineEdit->text().trimmed();
}



QString AddWordDialog::translation() const
{
    return ui->translationLineEdit->text().trimmed();
}


QString AddWordDialog::phonetic() const
{
    return ui->phoneticLineEdit->text().trimmed();
}

QString AddWordDialog::partOfSpeech() const
{
    return ui->partOfSpeechLineEdit->text().trimmed();
}
