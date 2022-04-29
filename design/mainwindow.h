#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>

class TabBar: public QTabBar
{
    Q_OBJECT

public:
    TabBar(QWidget *parent = nullptr);


    QSize tabSizeHint(int index) const;


protected:
    void paintEvent(QPaintEvent*);
};



class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent=0):QTabWidget(parent)
    {
        setTabBar(new TabBar);
        setTabPosition(QTabWidget::West);
    }
};



class MainWindow : public QMainWindow
{
private:

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private:
    TabWidget m_tabs;
};
#endif // MAINWINDOW_H
