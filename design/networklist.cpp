#include "networklist.h"



InterfacesTableModel::InterfacesTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
    //insertRows(1, )
}

InterfacesTableModel::InterfacesTableModel(const QList<Interface> &interfaces, QObject *parent)
    : QAbstractTableModel(parent), interfaces(interfaces)
{
}



int InterfacesTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : interfaces.size();
}

int InterfacesTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 3;
}



QVariant InterfacesTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= interfaces.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &interface = interfaces.at(index.row());

        switch (index.column()) {
            case 0:
                return interface.name;
            case 1:
                return interface.ipv4;
            case 2:
                return interface.ipv6;
            default:
                break;
        }
    }
    return QVariant();
}



QVariant InterfacesTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return "Name";
            case 1:
                return "ipv4";
            case 2:
                return "ipv6";
            default:
                break;
        }
    }
    return QVariant();
}



bool InterfacesTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginInsertRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        interfaces.insert(position, { QString(), QString(), QString() });

    endInsertRows();
    return true;
}



bool InterfacesTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);

    beginRemoveRows(QModelIndex(), position, position + rows - 1);

    for (int row = 0; row < rows; ++row)
        interfaces.removeAt(position);

    endRemoveRows();
    return true;
}



bool InterfacesTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto interface = interfaces.value(row);

        switch (index.column()) {
            case 0:
                interface.name = value.toString();
                break;
            case 1:
                interface.ipv4 = value.toString();
                break;
            case 2:
                interface.ipv6 = value.toString();
                break;
            default:
                return false;
        }
        interfaces.replace(row, interface);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}



Qt::ItemFlags InterfacesTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}



const QList<Interface> &InterfacesTableModel::getInterfaces() const
{
    return interfaces;
}







#include <QItemSelection>
#include <QTabWidget>
#include <QtWidgets>

NetworkList::NetworkList()
    : interfaces_model(new InterfacesTableModel(this))
{
    setModel(interfaces_model);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->hide();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(true);
}



void NetworkList::insertData(const QString &name, const QString &ipv4, const QString &ipv6)
{
    interfaces_model->insertRows(0, 1, QModelIndex());

    QModelIndex index = interfaces_model->index(0, 0, QModelIndex());
    interfaces_model->setData(index, name, Qt::EditRole);
    index = interfaces_model->index(0, 1, QModelIndex());
    interfaces_model->setData(index, ipv4, Qt::EditRole);
    index = interfaces_model->index(0, 2, QModelIndex());
    interfaces_model->setData(index, ipv6, Qt::EditRole);
}


void NetworkList::clearData()
{
   auto size = interfaces_model->rowCount(QModelIndex());
   if (size)
       interfaces_model->removeRows(0, size, QModelIndex());
}



void NetworkList::refreshData(int index)
{
    if (index == tab_index) {
        clearData();

        auto NetworkIntefaces = QNetworkInterface::allInterfaces();

        for (auto it : NetworkIntefaces) {
            QString name = it.humanReadableName();
            QString ipv4;
            QString ipv6;

            auto flag = it.flags();
            if ((flag & QNetworkInterface::InterfaceFlag::IsRunning) &&
                    !(flag & QNetworkInterface::InterfaceFlag::IsLoopBack))
            {
                auto addresses = it.addressEntries();
                for (auto it2 : addresses) {
                    auto ip = it2.ip();
                    if (ip.protocol() == QAbstractSocket::IPv4Protocol)
                        ipv4 = ip.toString();
                    else if (ip.protocol() == QAbstractSocket::IPv6Protocol)
                        // "toString()" adds type of interface after ipv6 address.
                        // If it unnecessary, it can be removed by deleting everything after separating symbol '%'
                        ipv6 = ip.toString();

                }
                insertData(name, ipv4, ipv6);
            } // if flags correct
        } // interfaces loop
    }
}


void NetworkList::setTabIndex (int index)
{
    tab_index = index;
}



