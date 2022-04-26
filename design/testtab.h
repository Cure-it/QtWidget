#ifndef TESTTAB_H
#define TESTTAB_H

#include <QWidget>
#include <QDialog>


class TestTab : public QWidget
{
    Q_OBJECT

public:
    TestTab(QWidget *parent = nullptr);

public slots:
    void addEntry();

signals:
    void sendDetails(const QString &name, const QString &address);
};

#endif // TESTTAB_H
