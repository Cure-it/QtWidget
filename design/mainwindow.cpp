#include "mainwindow.h"
#include "networklist.h"
#include "prismacloudproxy.h"


TabBar::TabBar(QWidget *parent) :
    QTabBar(parent)
{
}


/* Tabs size hint set a fixed width and
 * variable height, depended on amount of transfer symbols '\n'
*/
QSize TabBar::tabSizeHint(int index) const
{
    auto text = this->tabText(index);
    auto transfers = text.count("\n");

    return QSize(220, 50*(transfers+1));;
}


/*
 * At paint event we rotate rectangular area for tabs
 * and set css-style style-sheet options
*/
void TabBar::paintEvent(QPaintEvent*)
{
    QStylePainter painter(this);
    QStyleOptionTab opt;

    for(int i = 0;i < count();i++)
    {
        initStyleOption(&opt,i);
        painter.drawControl(QStyle::CE_TabBarTabShape, opt);
        painter.save();

        QSize s = opt.rect.size();
        s.transpose();
        QRect r(QPoint(), s);
        r.moveCenter(opt.rect.center());
        opt.rect = r;

        QPoint c = tabRect(i).center();
        painter.translate(c);
        painter.rotate(90);
        painter.translate(-c);
        painter.drawControl(QStyle::CE_TabBarTabLabel,opt);
        painter.restore();
    }

    setStyleSheet("QTabBar::tab:selected, QTabBar::tab:hover"
                           "{ background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #3377C9, stop: 0.4 #225695, stop: 0.5 #205084, stop: 1.0 #164164);}"

                           "QTabBar::tab {color: #C8C6FC; font-weight: bold; font-size: 20px;"
                                        "font-family: Georgia, serif;} "

                           "QTabBar::tab { background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #225695, stop: 0.4 #15365C, stop: 0.5 #113156, stop: 1.0 #0E2436);}"
                           );
}





MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_tabs(this)
{
    setCentralWidget(&m_tabs);


    NetworkList* network_list = new NetworkList;
    /* Connect click-on-tab event with refreshing network list function
    */
    connect(&m_tabs, &TabWidget::tabBarClicked, network_list, &NetworkList::refreshData);
    auto tab_index = m_tabs.addTab(network_list, "Network \ninterfaces");
    /* Save index of tab inside NetworkList object.
     * It used to refresh info every click on this tab
    */
    dynamic_cast<NetworkList*>(m_tabs.widget(tab_index))->setTabIndex(tab_index);
    emit m_tabs.tabBarClicked(tab_index);


    m_tabs.addTab(new PrismaCloudProxy, "PRISMAcloud");


    /* Paints space under tabs */
    this->setStyleSheet("QMainWindow {background: #0A192B;}");
    setWindowTitle("Firmware");
}

MainWindow::~MainWindow()
{
}
