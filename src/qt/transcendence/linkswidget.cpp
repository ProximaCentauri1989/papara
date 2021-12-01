// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/transcendence/linkswidget.h"
#include "qt/transcendence/forms/ui_linkswidget.h"
#include <QScrollBar>
#include <QMetaObject>
#include "qt/transcendence/qtutils.h"

LinksWidget::LinksWidget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LinksWidget)
{
    ui->setupUi(this);

    this->setStyleSheet(parent->styleSheet());

    ui->labelTitle->setText(tr("Frequently Asked Questions"));
    ui->labelWebLink->setText(tr("You can read more here"));
    setCssProperty(ui->container, "container-welcome");
    setCssProperty(ui->labelTitle, "text-title-faq");
    setCssProperty(ui->labelWebLink, "text-content-white");

    // Content
    setCssProperty({
           ui->labelNumber1,
           ui->labelNumber2,
           ui->labelNumber3,
           ui->labelNumber4,
           ui->labelNumber5,
           ui->labelNumber6,
           ui->labelNumber7,
           ui->labelNumber8,
           ui->labelNumber9,
           ui->labelNumber10
        }, "container-number-faq");

    setCssProperty({
              ui->labelSubtitle1,
              ui->labelSubtitle2,
              ui->labelSubtitle3,
              ui->labelSubtitle4,
              ui->labelSubtitle5,
              ui->labelSubtitle6,
              ui->labelSubtitle7,
              ui->labelSubtitle8,
              ui->labelSubtitle9,
              ui->labelSubtitle10
            }, "text-subtitle-faq");


    setCssProperty({
              ui->labelContent1,
              ui->labelContent2,
              ui->labelContent3,
              ui->labelContent4,
              ui->labelContent5,
              ui->labelContent6,
              ui->labelContent7,
              ui->labelContent8,
              ui->labelContent9,
              ui->labelContent10
            }, "text-content-faq");


    setCssProperty({
              ui->pushButtonFaq1,
              ui->pushButtonFaq2,
              ui->pushButtonFaq3,
              ui->pushButtonFaq4,
              ui->pushButtonFaq5,
              ui->pushButtonFaq6,
              ui->pushButtonFaq7,
              ui->pushButtonFaq8,
              ui->pushButtonFaq9,
              ui->pushButtonFaq10
            }, "btn-faq-options");

    ui->labelContent3->setOpenExternalLinks(true);
    ui->labelContent5->setOpenExternalLinks(true);
    ui->labelContent8->setOpenExternalLinks(true);

    // Web Link
    ui->pushButtonWebLink->setText("https://PIVX.org/");
    setCssProperty(ui->pushButtonWebLink, "btn-faq-web");
    setCssProperty(ui->containerButtons, "container-faq-buttons");

    // Buttons
    connect(ui->pushButtonFaq1, SIGNAL(clicked()), this, SLOT(onFaq1Clicked()));
    connect(ui->pushButtonFaq2, SIGNAL(clicked()), this, SLOT(onFaq2Clicked()));
    connect(ui->pushButtonFaq3, SIGNAL(clicked()), this, SLOT(onFaq3Clicked()));
    connect(ui->pushButtonFaq4, SIGNAL(clicked()), this, SLOT(onFaq4Clicked()));
    connect(ui->pushButtonFaq5, SIGNAL(clicked()), this, SLOT(onFaq5Clicked()));
    connect(ui->pushButtonFaq6, SIGNAL(clicked()), this, SLOT(onFaq6Clicked()));
    connect(ui->pushButtonFaq7, SIGNAL(clicked()), this, SLOT(onFaq7Clicked()));
    connect(ui->pushButtonFaq8, SIGNAL(clicked()), this, SLOT(onFaq8Clicked()));
    connect(ui->pushButtonFaq9, SIGNAL(clicked()), this, SLOT(onFaq9Clicked()));
    connect(ui->pushButtonFaq10, SIGNAL(clicked()), this, SLOT(onFaq10Clicked()));

    if (parent)
        connect(parent, SIGNAL(windowResizeEvent(QResizeEvent*)), this, SLOT(windowResizeEvent(QResizeEvent*)));
}

void LinksWidget::showEvent(QShowEvent *event){
    if(pos != 0){
        QPushButton* btn = getButtons()[pos - 1];
        QMetaObject::invokeMethod(btn, "setChecked", Qt::QueuedConnection, Q_ARG(bool, true));
        QMetaObject::invokeMethod(btn, "clicked", Qt::QueuedConnection);
    }
}

void LinksWidget::setSection(int num){
    if (num < 1 || num > 10)
        return;
    pos = num;
}

void LinksWidget::onFaq1Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq1->y());
}

void LinksWidget::onFaq2Clicked(){
   ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq2->y());
}

void LinksWidget::onFaq3Clicked(){
   ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq3->y());
}

void LinksWidget::onFaq4Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq4->y());
}

void LinksWidget::onFaq5Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq5->y());
}

void LinksWidget::onFaq6Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq6->y());
}

void LinksWidget::onFaq7Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq7->y());
}

void LinksWidget::onFaq8Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq8->y());
}

void LinksWidget::onFaq9Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq9->y());
}

void LinksWidget::onFaq10Clicked(){
    ui->scrollAreaFaq->verticalScrollBar()->setValue(ui->widgetFaq10->y());
}

void LinksWidget::windowResizeEvent(QResizeEvent* event){
    QWidget* w = qobject_cast<QWidget*>(parent());
    this->resize(w->width(), w->height());
    this->move(QPoint(0, 0));
}

std::vector<QPushButton*> LinksWidget::getButtons(){
    return {
            ui->pushButtonFaq1,
            ui->pushButtonFaq2,
            ui->pushButtonFaq3,
            ui->pushButtonFaq4,
            ui->pushButtonFaq5,
            ui->pushButtonFaq6,
            ui->pushButtonFaq7,
            ui->pushButtonFaq8,
            ui->pushButtonFaq9,
            ui->pushButtonFaq10
    };
}

LinksWidget::~LinksWidget(){
    delete ui;
}


