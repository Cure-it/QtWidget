#ifndef PRISMACLOUDPROXY_H
#define PRISMACLOUDPROXY_H

#include <QWidget>
#include <QWebEngineView>

#include <QAbstractTableModel>
#include <QTableView>
#include <QList>

struct Incident
{
    QString date;
    QString name;
    QString short_description;
    QString reference;

    bool operator==(const Incident &other) const
    {
        return name == other.name && date == other.date && short_description == other.short_description
                && reference == other.reference;
    }
};



class IncidentsTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    IncidentsTableModel(QObject *parent = nullptr);
    IncidentsTableModel(const QList<Incident> &interfaces, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    const QList<Incident> &getIncidents() const;


private:
    QList<Incident> incidents;
};





class IncidentsTable : public QTableView
{
    Q_OBJECT
public:
    explicit IncidentsTable();

public slots:
    void refreshData(int start_index, QList<Incident> incidents);


private:
    void replaceData(int index, const Incident &incident);

    void deleteData(int index);

    IncidentsTableModel* incidents_model;
};









/* Neccesary data must be downloaded using javascript scripts. This is cause using QWebEngine
*/
#include <QBoxLayout>
#include <QPushButton>
class PrismaCloudProxy : public QWidget
{
    Q_OBJECT
public:
    explicit PrismaCloudProxy(QWidget *parent = nullptr);

    void setTabIndex (int index);

public slots:
    void pageLoadFinished();

    void nextTablePage();
    void prevTablePage();



private:
    void loadNextPage();

    void refreshTable();

    QList<Incident> parsed_incidents_list;

    //QWebEngineView *m_webview;
    QWebEnginePage web_page;
    const QString base_site_url;
    int page_number;

    IncidentsTable* incidents_table;

    QVBoxLayout* main_layout;
    QWidget* main_widget;
    QPushButton* button_prev;
    QPushButton* button_next;

    int start_index;
};

#endif // PRISMACLOUDPROXY_H
