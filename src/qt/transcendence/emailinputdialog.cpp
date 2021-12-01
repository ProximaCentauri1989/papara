// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/transcendence/emailinputdialog.h"
#include "qt/transcendence/forms/ui_emailinputdialog.h"
#include "guiutil.h"
#include "qt/transcendence/qtutils.h"

EmailInputDialog::EmailInputDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EmailInputDialog)
{
    ui->setupUi(this);

    // Stylesheet
    this->setStyleSheet(parent ? parent->styleSheet() : GUIUtil::loadStyleSheet());

    // Container
    ui->frame->setProperty("cssClass", "container-dialog");

    // Text
    ui->labelTitle->setText("Dialog Title");
    ui->labelTitle->setProperty("cssClass", "text-title-dialog");

    ui->labelMessage->setText("Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.");
    ui->labelMessage->setProperty("cssClass", "text-main-grey");

    ui->labelInputEmail->setText(tr("Email:"));
    ui->labelInputEmail->setProperty("cssClass", "text-main-grey");
    ui->labelInputSmtp->setText(tr("SMTP server URL:"));
    ui->labelInputSmtp->setProperty("cssClass", "text-main-grey");
    ui->labelInputUser->setText(tr("Username:"));
    ui->labelInputUser->setProperty("cssClass", "text-main-grey");
    ui->labelInputPass->setText(tr("Password:"));
    ui->labelInputPass->setProperty("cssClass", "text-main-grey");
    
    initCssEditLine(ui->lineEditEmail, true);
    initCssEditLine(ui->lineEditSmtp, true);
    initCssEditLine(ui->lineEditUser, true);
    initCssEditLine(ui->lineEditPass, true);

    // Buttons
    ui->btnEsc->setText("");
    ui->btnEsc->setProperty("cssClass", "ic-close");

    ui->btnCancel->setProperty("cssClass", "btn-dialog-cancel");
    ui->btnSave->setText("OK");
    ui->btnSave->setProperty("cssClass", "btn-primary");

    connect(ui->btnEsc, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnCancel, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->btnSave, &QPushButton::clicked, [this](){this->isOk = true; accept();});
}

void EmailInputDialog::setText(QString title, QString message, QString okBtnText, QString cancelBtnText){
    if(!okBtnText.isNull()) ui->btnSave->setText(okBtnText);
    if(!cancelBtnText.isNull()){
        ui->btnCancel->setVisible(true);
        ui->btnCancel->setText(cancelBtnText);
    }else{
        ui->btnCancel->setVisible(false);
    }
    if(!message.isNull()) ui->labelMessage->setText(message);
    if(!title.isNull()) ui->labelTitle->setText(title);
}

const std::string EmailInputDialog::getEmail() const {
    QString str = ui->lineEditEmail->text();
    return str.toStdString();
}

const std::string EmailInputDialog::getUrl() const {
    QString str = ui->lineEditSmtp->text();
    return str.toStdString();
}

const std::string EmailInputDialog::getUsername() const {
    QString str = ui->lineEditUser->text();
    return str.toStdString();
}

const std::string EmailInputDialog::getPassword() const {
    QString str = ui->lineEditPass->text();
    return str.toStdString();
}

EmailInputDialog::~EmailInputDialog()
{
    delete ui;
}
