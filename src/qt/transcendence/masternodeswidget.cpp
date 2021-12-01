// Copyright (c) 2019 The PIVX developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "qt/transcendence/masternodeswidget.h"
#include "qt/transcendence/forms/ui_masternodeswidget.h"
#include "qt/transcendence/qtutils.h"
#include "qt/transcendence/mnrow.h"
#include "qt/transcendence/mninfodialog.h"

#include "qt/transcendence/masternodewizarddialog.h"

#include "activemasternode.h"
#include "clientmodel.h"
#include "guiutil.h"
#include "init.h"
#include "masternode-sync.h"
#include "masternodeconfig.h"
#include "masternodeman.h"
#include "sync.h"
#include "wallet/wallet.h"
#include "walletmodel.h"
#include "askpassphrasedialog.h"
#include "util.h"
#include "qt/transcendence/optionbutton.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include <fstream>
#include <QGraphicsLayout>

#define DECORATION_SIZE 65
#define NUM_ITEMS 3

class MNHolder : public FurListRow<QWidget*>
{
public:
    MNHolder();

    explicit MNHolder(bool _isLightTheme) : FurListRow(), isLightTheme(_isLightTheme){}

    MNRow* createHolder(int pos) override{
        if(!cachedRow) cachedRow = new MNRow();
        return cachedRow;
    }

    void init(QWidget* holder,const QModelIndex &index, bool isHovered, bool isSelected) const override{
        MNRow* row = static_cast<MNRow*>(holder);
        QString label = index.data(Qt::DisplayRole).toString();
        QString address = index.sibling(index.row(), MNModel::ADDRESS).data(Qt::DisplayRole).toString();
        QString status = index.sibling(index.row(), MNModel::STATUS).data(Qt::DisplayRole).toString();
        bool wasCollateralAccepted = index.sibling(index.row(), MNModel::WAS_COLLATERAL_ACCEPTED).data(Qt::DisplayRole).toBool();
        QString tier = "Tier: " + index.sibling(index.row(), MNModel::TIER).data(Qt::DisplayRole).toString();
        row->updateView("Address: " + address, label, status,tier,wasCollateralAccepted);
    }

    QColor rectColor(bool isHovered, bool isSelected) override{
        return getRowColor(isLightTheme, isHovered, isSelected);
    }

    ~MNHolder() override{}

    bool isLightTheme;
    MNRow* cachedRow = nullptr;
};
void MasterNodesWidget::showHideEmptyChart(bool showEmpty, bool loading, bool forceView) {
    if (forceView) {
        if (!ui->layoutChart->isVisible()) {
            ui->layoutChart->setVisible(!showEmpty);
        }
    }
}
void MasterNodesWidget::loadChart(){
        if (!chart) {
            showHideEmptyChart(false, false,true);
            initChart();
        }
}
MasterNodesWidget::MasterNodesWidget(TELOSGUI *parent) :
    PWidget(parent),
    ui(new Ui::MasterNodesWidget)
{
    ui->setupUi(this);

    delegate = new FurAbstractListItemDelegate(
            DECORATION_SIZE,
            new MNHolder(isLightTheme()),
            this
    );
    mnModel = new MNModel(this);

    this->setStyleSheet(parent->styleSheet());

    /* Containers */
    setCssProperty(ui->left, "container");
    ui->left->setContentsMargins(0,20,0,20);
    setCssProperty(ui->right, "container-right");
    ui->right->setContentsMargins(20,20,20,20);

    /* Light Font */
    QFont fontLight;
    fontLight.setWeight(QFont::Light);

    /* Title */
    ui->labelTitle->setText(tr("Masternodes"));
    setCssTitleScreen(ui->labelTitle);
    ui->labelTitle->setFont(fontLight);

    ui->labelSubtitle1->setText(tr("Full nodes that incentivize node operators to perform the core consensus functions\nand vote on the treasury system receiving a periodic reward."));
    setCssSubtitleScreen(ui->labelSubtitle1);

    /* Buttons */
    ui->pushButtonSave->setText(tr("Create Masternode Controller"));
    setCssBtnPrimary(ui->pushButtonSave);

    /* Options */
    ui->btnAbout->setTitleClassAndText("btn-title-grey", "What is a Masternode?");
    ui->btnAbout->setSubTitleClassAndText("text-subtitle", "FAQ explaining what Masternodes are");
    ui->btnAboutController->setTitleClassAndText("btn-title-grey", "What is a Controller?");
    ui->btnAboutController->setSubTitleClassAndText("text-subtitle", "FAQ explaining what is a Masternode Controller");
    ui->btnChartTiers->setTitleClassAndText("btn-title-grey", "Masternodes Tier Distribution Chart");
    ui->btnChartTiers->setSubTitleClassAndText("text-subtitle", "Shows a chart showing the distribution of various tiers");
    setCssProperty(ui->listMn, "container");
    ui->listMn->setItemDelegate(delegate);
    ui->listMn->setIconSize(QSize(DECORATION_SIZE, DECORATION_SIZE));
    ui->listMn->setMinimumHeight(NUM_ITEMS * (DECORATION_SIZE + 2));
    ui->listMn->setAttribute(Qt::WA_MacShowFocusRect, false);
    ui->listMn->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->layoutChart->setVisible(false);
    setCssProperty(ui->chartContainer, "container-chart");
    ui->emptyContainer->setVisible(false);
    setCssProperty(ui->pushImgEmpty, "img-empty-master");
    ui->labelEmpty->setText(tr("No active Masternode yet"));
    setCssProperty(ui->labelEmpty, "text-empty");
    connect(ui->pushButtonSave, SIGNAL(clicked()), this, SLOT(onCreateMNClicked()));
    connect(ui->listMn, SIGNAL(clicked(QModelIndex)), this, SLOT(onMNClicked(QModelIndex)));
    connect(ui->btnChartTiers, SIGNAL(clicked()), this, SLOT(onTierChartBtnClicked()));
    connect(ui->btnAbout, &OptionButton::clicked, [this](){window->openFAQ(9);});
    connect(ui->btnAboutController, &OptionButton::clicked, [this](){window->openFAQ(10);});
}

void MasterNodesWidget::showEvent(QShowEvent *event){
    if (mnModel) mnModel->updateMNList();
    if(!timer) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, [this]() {mnModel->updateMNList();});
    }
    timer->start(30000);
}

void MasterNodesWidget::hideEvent(QHideEvent *event){
    if(timer) timer->stop();
}

void MasterNodesWidget::loadWalletModel(){
    if(walletModel) {
        ui->listMn->setModel(mnModel);
        ui->listMn->setModelColumn(AddressTableModel::Label);
        updateListState();
    }
}

void MasterNodesWidget::updateListState() {
    if (mnModel->rowCount() > 0) {
        ui->listMn->setVisible(true);
        ui->emptyContainer->setVisible(false);
    } else {
        ui->listMn->setVisible(false);
        ui->emptyContainer->setVisible(true);
    }
}

void MasterNodesWidget::onMNClicked(const QModelIndex &index){
    ui->listMn->setCurrentIndex(index);
    QRect rect = ui->listMn->visualRect(index);
    QPoint pos = rect.topRight();
    pos.setX(pos.x() - (DECORATION_SIZE * 2));
    pos.setY(pos.y() + (DECORATION_SIZE * 1.5));
    if(!this->menu){
        this->menu = new TooltipMenu(window, this);
        this->menu->setEditBtnText(tr("Start"));
        this->menu->setDeleteBtnText(tr("Delete"));
        this->menu->setCopyBtnText(tr("Info"));
        connect(this->menu, &TooltipMenu::message, this, &AddressesWidget::message);
        connect(this->menu, SIGNAL(onEditClicked()), this, SLOT(onEditMNClicked()));
        connect(this->menu, SIGNAL(onDeleteClicked()), this, SLOT(onDeleteMNClicked()));
        connect(this->menu, SIGNAL(onCopyClicked()), this, SLOT(onInfoMNClicked()));
        this->menu->adjustSize();
    }else {
        this->menu->hide();
    }
    this->index = index;
    menu->move(pos);
    menu->show();

    // Back to regular status
    ui->listMn->scrollTo(index);
    ui->listMn->clearSelection();
    ui->listMn->setFocus();
}
void MasterNodesWidget::onTierChartBtnClicked(){
 bool isVisible = ui->layoutChart->isVisible();
    if(!isVisible){
        ui->layoutChart->setVisible(true);
        initChart();
        ui->btnChartTiers->setRightIconClass("btn-dropdown", true);
    }else{
        ui->layoutChart->setVisible(false);
        ui->btnChartTiers->setRightIconClass("ic-arrow", true);
    }
}
void MasterNodesWidget::initChart() {
    series = new QPieSeries();
    //Get tier count
    mnodeman.CountTiers(ActiveProtocol(),tier1, tier2, tier3,tier4,tier5);
    if(tier1 !=0 && tier2 !=0 && tier3 !=0 && tier4 !=0 && tier5 != 0) {
    series->append("Tier 1", tier1);
    series->append("Tier 2", tier2);
    series->append("Tier 3", tier3);
    series->append("Tier 4", tier4);
    series->append("Tier 5", tier5); /* Finished appending tier data */
    }
    //Initialize chart
    chart = new QChart();
    chart->addSeries(series);
    chart->legend()->show();

    // Chart style
    chart->legend()->setAlignment(Qt::AlignTop);
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->setMargins({0, 0, 0, 0});
    chart->setBackgroundRoundness(0);
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    changeChartColors();

    //Finalize chartview
    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setRubberBand( QChartView::HorizontalRubberBand );
    chartView->setContentsMargins(0,0,0,0);
    //Add chartview to qhboxlayout
    QHBoxLayout *baseScreensContainer = new QHBoxLayout(this);
    baseScreensContainer->addWidget(chartView);
    ui->chartContainer->setLayout(baseScreensContainer);
    ui->chartContainer->setContentsMargins(0,0,0,0);
    setCssProperty(ui->chartContainer, "container-chart");
}
void MasterNodesWidget::onEditMNClicked(){
    if(walletModel) {
        if (index.sibling(index.row(), MNModel::WAS_COLLATERAL_ACCEPTED).data(Qt::DisplayRole).toBool()) {
            // Start MN
            QString strAlias = this->index.data(Qt::DisplayRole).toString();
            if (ask(tr("Start Masternode"), tr("Are you sure you want to start masternode %1?\n").arg(strAlias))) {
                if (!verifyWalletUnlocked()) return;
                startAlias(strAlias);
            }
        }else {
            inform(tr("Cannot start masternode, the collateral transaction has not been accepted by the network.\nPlease wait few more minutes."));
        }
    }
}

void MasterNodesWidget::startAlias(QString strAlias){
    QString strStatusHtml;
    strStatusHtml += "Alias: " + strAlias + " ";

    for (CMasternodeConfig::CMasternodeEntry mne : masternodeConfig.getEntries()) {
        if (mne.getAlias() == strAlias.toStdString()) {
            std::string strError;
            CMasternodeBroadcast mnb;
            if (CMasternodeBroadcast::Create(mne.getIp(), mne.getPrivKey(), mne.getTxHash(), mne.getOutputIndex(), strError, mnb)) {
                strStatusHtml += "successfully started.";
                mnodeman.UpdateMasternodeList(mnb);
                mnb.Relay();
                mnModel->updateMNList();
            } else {
                strStatusHtml += "failed to start.\nError: " + QString::fromStdString(strError);
            }
            break;
        }
    }
    inform(strStatusHtml);
}

void MasterNodesWidget::onInfoMNClicked(){
    if(!verifyWalletUnlocked()) return;
    showHideOp(true);
    MnInfoDialog* dialog = new MnInfoDialog(window);
    QString label = index.data(Qt::DisplayRole).toString();
    QString address = index.sibling(index.row(), MNModel::ADDRESS).data(Qt::DisplayRole).toString();
    QString status = index.sibling(index.row(), MNModel::STATUS).data(Qt::DisplayRole).toString();
    QString txId = index.sibling(index.row(), MNModel::COLLATERAL_ID).data(Qt::DisplayRole).toString();
    QString outIndex = index.sibling(index.row(), MNModel::COLLATERAL_OUT_INDEX).data(Qt::DisplayRole).toString();
    QString pubKey = index.sibling(index.row(), MNModel::PUB_KEY).data(Qt::DisplayRole).toString();
    dialog->setData(pubKey, label, address, txId, outIndex, status);
    dialog->adjustSize();
    showDialog(dialog, 3, 17);
    if (dialog->exportMN){
        if (ask(tr("Remote Masternode Data"),
                tr("You are just about to export the required data to run a Masternode\non a remote server to your clipboard.\n\n\n"
                   "You will only have to paste the data in the pivx.conf file\nof your remote server and start it, "
                   "then start the Masternode using\nthis controller wallet (select the Masternode in the list and press \"start\").\n"
                ))) {
            // export data
            QString exportedMN = "masternode=1\n"
                                 "externalip=" + address.split(":")[0] + "\n" +
                                 "masternodeaddr=" + address + + "\n" +
                                 "masternodeprivkey=" + index.sibling(index.row(), MNModel::PRIV_KEY).data(Qt::DisplayRole).toString() + "\n";
            GUIUtil::setClipboard(exportedMN);
            inform(tr("Masternode exported!, check your clipboard"));
        }
    }

    dialog->deleteLater();
}

void MasterNodesWidget::onDeleteMNClicked(){
    QString qAliasString = index.data(Qt::DisplayRole).toString();
    std::string aliasToRemove = qAliasString.toStdString();

    if (!ask(tr("Delete Masternode"), tr("You are just about to delete Masternode:\n%1\n\nAre you sure?").arg(qAliasString)))
        return;

    std::string strConfFile = "masternode.conf";
    std::string strDataDir = GetDataDir().string();
    if (strConfFile != boost::filesystem::basename(strConfFile) + boost::filesystem::extension(strConfFile)){
        throw std::runtime_error(strprintf(_("masternode.conf %s resides outside data directory %s"), strConfFile, strDataDir));
    }

    boost::filesystem::path pathBootstrap = GetDataDir() / strConfFile;
    if (boost::filesystem::exists(pathBootstrap)) {
        boost::filesystem::path pathMasternodeConfigFile = GetMasternodeConfigFile();
        boost::filesystem::ifstream streamConfig(pathMasternodeConfigFile);

        if (!streamConfig.good()) {
            inform(tr("Invalid masternode.conf file"));
            return;
        }

        int lineNumToRemove = -1;
        int linenumber = 1;
        std::string lineCopy = "";
        for (std::string line; std::getline(streamConfig, line); linenumber++) {
            if (line.empty()) continue;

            std::istringstream iss(line);
            std::string comment, alias, ip, privKey, txHash, outputIndex;

            if (iss >> comment) {
                if (comment.at(0) == '#') continue;
                iss.str(line);
                iss.clear();
            }

            if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex)) {
                iss.str(line);
                iss.clear();
                if (!(iss >> alias >> ip >> privKey >> txHash >> outputIndex)) {
                    streamConfig.close();
                    inform(tr("Error parsing masternode.conf file"));
                    return;
                }
            }

            if (aliasToRemove == alias) {
                lineNumToRemove = linenumber;
            } else
                lineCopy += line + "\n";

        }

        if (lineCopy.size() == 0) {
            lineCopy = "# Masternode config file\n"
                                    "# Format: alias IP:port masternodeprivkey collateral_output_txid collateral_output_index\n"
                                    "# Example: mn1 127.0.0.2:51472 93HaYBVUCYjEMeeH1Y4sBGLALQZE1Yc1K64xiqgX37tGBDQL8Xg 2bcd3c84c84f87eaa86e4e56834c92927a07f9e18718810b92e0d0324456a67c 0\n";
        }

        streamConfig.close();

        if (lineNumToRemove != -1) {
            boost::filesystem::path pathConfigFile("masternode_temp.conf");
            if (!pathConfigFile.is_complete()) pathConfigFile = GetDataDir() / pathConfigFile;
            FILE* configFile = fopen(pathConfigFile.string().c_str(), "w");
            fwrite(lineCopy.c_str(), std::strlen(lineCopy.c_str()), 1, configFile);
            fclose(configFile);

            boost::filesystem::path pathOldConfFile("old_masternode.conf");
            if (!pathOldConfFile.is_complete()) pathOldConfFile = GetDataDir() / pathOldConfFile;
            if (boost::filesystem::exists(pathOldConfFile)) {
                boost::filesystem::remove(pathOldConfFile);
            }
            rename(pathMasternodeConfigFile, pathOldConfFile);

            boost::filesystem::path pathNewConfFile("masternode.conf");
            if (!pathNewConfFile.is_complete()) pathNewConfFile = GetDataDir() / pathNewConfFile;
            rename(pathConfigFile, pathNewConfFile);

            // Remove alias
            masternodeConfig.remove(aliasToRemove);
            // Update list
            mnModel->removeMn(index);
            updateListState();
        }
    } else{
        inform(tr("masternode.conf file doesn't exists"));
    }
}

void MasterNodesWidget::onCreateMNClicked(){
    if(verifyWalletUnlocked()) {
        if(walletModel->getBalance() <= (COIN * 1000)){
            inform(tr("Not enough balance to create a masternode, 1,000 or more TELOS required."));
            return;
        }
        showHideOp(true);
        MasterNodeWizardDialog *dialog = new MasterNodeWizardDialog(walletModel, window);
        if(openDialogWithOpaqueBackgroundY(dialog, window, 5, 7)) {
            if (dialog->isOk) {
                // Update list
                mnModel->addMn(dialog->mnEntry);
                updateListState();
                // add mn
                inform(dialog->returnStr);
            } else {
                warn(tr("Error creating masternode"), dialog->returnStr);
            }
        }
        dialog->deleteLater();
    }
}
void MasterNodesWidget::changeChartColors(){
    //declare vars
    QColor backgroundColor = QColor(0,0,0);
    QColor gridLineColorX  = QColor(0,0,0);
    QColor legendTextColor = QColor(0,0,0);
    if(isLightTheme()){
        gridLineColorX = QColor(255,255,255);
        backgroundColor = gridLineColorX;
    }else{
        gridLineColorX = QColor("#212121");
        backgroundColor = gridLineColorX;
        legendTextColor = QColor(225,225,225);
    }
    chart->setBackgroundBrush(QBrush(backgroundColor));
    chart->legend()->setBrush(QBrush(legendTextColor));
    chart->legend()->setPen(QPen(legendTextColor));
    chart->legend()->setLabelColor(legendTextColor);
}
void MasterNodesWidget::changeTheme(bool isLightTheme, QString& theme){
    static_cast<MNHolder*>(this->delegate->getRowFactory())->isLightTheme = isLightTheme;
#ifdef USE_QTCHARTS
    if (chart) this->changeChartColors();
#endif
}

MasterNodesWidget::~MasterNodesWidget()
{
    delete ui;
}
