#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QStyleOptionTab>
#include <QStylePainter>
#include <QTabBar>
#include <QTabWidget>

class TabBar: public QTabBar{
    Q_OBJECT

public:
    QSize tabSizeHint(int index) const{
        auto text = this->tabText(index);
        auto transfers = text.count("\n");

        return QSize(220, 50*(transfers+1));;
    }
protected:
    void paintEvent(QPaintEvent * /*event*/){
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
    }
};

class TabWidget : public QTabWidget
{
    Q_OBJECT

public:
    TabWidget(QWidget *parent=0):QTabWidget(parent){
        setTabBar(new TabBar);
        setTabPosition(QTabWidget::West);
    }


    void emit_this_signal();
signals:
    void testmainwindow_signal();
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    TabWidget tabs;
};
#endif // MAINWINDOW_H
