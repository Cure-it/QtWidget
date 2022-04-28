#include "mainwindow.h"
#include "networklist.h"
#include "prismacloudproxy.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setCentralWidget(&tabs);

    NetworkList* network_list = new NetworkList;
    connect(&tabs, &TabWidget::tabBarClicked, network_list, &NetworkList::refreshData);
    auto tab_index = tabs.addTab(network_list, "Network \ninterfaces");
    dynamic_cast<NetworkList*>(tabs.widget(tab_index))->setTabIndex(tab_index);
    emit tabs.tabBarClicked(tab_index);


    tabs.addTab(new PrismaCloudProxy, "PRISMAcloud");


    this->setStyleSheet("QMainWindow {background: #0A192B;}");
    tabs.setStyleSheet("QTabBar::tab:selected, QTabBar::tab:hover"
                       "{ background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #3377C9, stop: 0.4 #225695, stop: 0.5 #205084, stop: 1.0 #164164);}"

                       "QTabBar::tab {color: #C8C6FC; font-weight: bold; font-size: 20px;"
                                    "font-family: Georgia, serif;} "

                       "QTabBar::tab { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #225695, stop: 0.4 #15365C, stop: 0.5 #113156, stop: 1.0 #0E2436);}"
                       );
    setWindowTitle("Firmware");
}

MainWindow::~MainWindow()
{
}
