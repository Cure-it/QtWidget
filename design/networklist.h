#ifndef NETWORKLIST_H
#define NETWORKLIST_H

#include <QWidget>
#include <QNetworkInterface>
#include <QTableView>

#include <QAbstractTableModel>
#include <QList>


//! [0]

struct Interface
{
    QString name;
    QString ipv4;
    QString ipv6;

    bool operator==(const Interface &other) const
    {
        return name == other.name && ipv4 == other.ipv4 && ipv6 == other.ipv6;
    }
};
/*
inline QDataStream &operator<<(QDataStream &stream, const Contact &contact)
{
    return stream << contact.name << contact.address;
}

inline QDataStream &operator>>(QDataStream &stream, Contact &contact)
{
    return stream >> contact.name >> contact.address;
}
*/
class InterfacesTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    InterfacesTableModel(QObject *parent = nullptr);
    InterfacesTableModel(const QList<Interface> &contacts, QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    bool insertRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;
    bool removeRows(int position, int rows, const QModelIndex &index = QModelIndex()) override;

    const QList<Interface> &getInterfaces() const;


private:
    QList<Interface> interfaces;
};




class NetworkList : public QTableView
{
    Q_OBJECT
public:
    explicit NetworkList();

    void setTabIndex (int index);

public slots:
    void refreshData(int index);


private:
    void insertData(const QString &name, const QString &ipv4, const QString &ipv6);

    void clearData();


    InterfacesTableModel* interfaces;
    int tab_index;
};

#endif // NETWORKLIST_H
