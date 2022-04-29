#include "prismacloudproxy.h"
#include "qheaderview.h"


IncidentsTableModel::IncidentsTableModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}



int IncidentsTableModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : m_incidents.size();
}

int IncidentsTableModel::columnCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : 4;
}



bool IncidentsTableModel::insertRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    if (position < 0)
        return false;
    if (rows)
    {
        beginInsertRows(QModelIndex(), position, position + rows - 1);

        for (int row = 0; row < rows; ++row)
            m_incidents.insert(position, { QString(), QString(), QString(), QString() });

        endInsertRows();
    }
    return true;
}

bool IncidentsTableModel::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);

    if (rows)
    {
        if (m_incidents.size() < position + rows)
            return false;

        beginRemoveRows(QModelIndex(), position, position + rows - 1);

        for (int row = 0; row < rows; ++row)
            m_incidents.removeAt(position);

        endRemoveRows();
    }
    return true;
}


/* For DisplayRole it returns columns 0-2 (date, name, short_description)
 * For DisplayPropertyRole it always returns column 3 (reference)
*/
QVariant IncidentsTableModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_incidents.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole) {
        const auto &incident = m_incidents.at(index.row());

        switch (index.column()) {
            case 0:
                return incident.date;
            case 1:
                return incident.name;
            case 2:
                return incident.short_description;
            default:
                break;
        }
    } else if (role == Qt::DisplayPropertyRole) {
        const auto &incident = m_incidents.at(index.row());
        return incident.reference;
    }
    return QVariant();
}

QVariant IncidentsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal) {
        switch (section) {
            case 0:
                return "Date";
            case 1:
                return "Name";
            case 2:
                return "Short description";
            default:
                break;
        }
    }
    return QVariant();
}



bool IncidentsTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        const int row = index.row();
        auto incident = m_incidents.value(row);

        switch (index.column()) {
            case 0:
                incident.date = value.toString();
                break;
            case 1:
                incident.name = value.toString();
                break;
            case 2:
                incident.short_description = value.toString();
                break;
            case 3:
                incident.reference = value.toString();
                break;
            default:
                return false;
        }
        m_incidents.replace(row, incident);
        emit dataChanged(index, index, {Qt::DisplayRole, Qt::EditRole});

        return true;
    }

    return false;
}



Qt::ItemFlags IncidentsTableModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;

    return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
}



const QList<Incident> &IncidentsTableModel::getIncidents() const
{
    return m_incidents;
}





IncidentsTable::IncidentsTable()
    : m_incidents_model(new IncidentsTableModel(this))
{
    setModel(m_incidents_model);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    horizontalHeader()->setStretchLastSection(true);
    verticalHeader()->hide();
    setEditTriggers(QAbstractItemView::NoEditTriggers);
    setSelectionMode(QAbstractItemView::SingleSelection);
    setSortingEnabled(true);

    setToolTip("Double-click for more information");

    /* Connect double-click event with start browser window
    */
    connect (this, &QAbstractItemView::doubleClicked, this, &IncidentsTable::showInBrowser);
}



/* Replaces data in existing table row. Add row at the end if it is not existing
*/
void IncidentsTable::replaceData(int index, const Incident &incident)
{
    if (index >= m_incidents_model->rowCount(QModelIndex()))
        m_incidents_model->insertRows(index, 1, QModelIndex());

    QModelIndex q_index = m_incidents_model->index(index, 0, QModelIndex());
    m_incidents_model->setData(q_index, incident.date, Qt::EditRole);
    q_index = m_incidents_model->index(index, 1, QModelIndex());
    m_incidents_model->setData(q_index, incident.name, Qt::EditRole);
    q_index = m_incidents_model->index(index, 2, QModelIndex());
    m_incidents_model->setData(q_index, incident.short_description, Qt::EditRole);
    q_index = m_incidents_model->index(index, 3, QModelIndex());
    m_incidents_model->setData(q_index, incident.reference, Qt::EditRole);
}



void IncidentsTable::deleteData(int index)
{
    m_incidents_model->removeRows(index, 1, QModelIndex());
}



/* Sets data to be shown into table
 * It sets 3 items from argument QList<Incident> incidents with indexes: start_index:start_index+2
*/
void IncidentsTable::refreshData(int start_index, QList<Incident> incidents)
{
    int data_index = start_index;
    int table_index = 0;

    while (data_index < start_index+3 )
    {
        /* if incidents contains data with such index, it place data into table
         * In other way it checks if table contain excessive rows and delete them
        */
        if ( data_index < incidents.size() )
        {
            /*  Check if table already contains data to be inserted
            */
            if (m_incidents_model->getIncidents().contains({ incidents[data_index].date, incidents[data_index].name,
                                                         incidents[data_index].short_description, incidents[data_index].reference }))
            {
                auto table_record_index = m_incidents_model->getIncidents().indexOf({ incidents[data_index].date, incidents[data_index].name,
                                                                                    incidents[data_index].short_description, incidents[data_index].reference });
                if (table_record_index == table_index)
                {
                    data_index++;
                    table_index++;
                    continue;
                }
                else
                    deleteData(table_record_index);
            }

            replaceData(table_index, incidents[data_index]);
            table_index++;
        }
        else
        {
            /* Delete excessive row */
            if (m_incidents_model->rowCount(QModelIndex()) > table_index)
                deleteData(table_index);
        }

        data_index++;
    }
}



/* Takes url reference from table information and puts it into Browser object
*/
void IncidentsTable::showInBrowser(const QModelIndex &index)
{
    auto reference = m_incidents_model->data(index, Qt::DisplayPropertyRole);
    Browser browserWindow(QUrl(reference.toString()), this);

    browserWindow.exec();
}




/* Opens dialog window with browser.
 * Browser loads page with argument's url.
*/
Browser::Browser(const QUrl& url, QWidget *parent) :
    QDialog(parent)
{
    auto main_layout = new QVBoxLayout;

    auto webview = new QWebEngineView(this);
    webview->load(url);
    webview->show();

    main_layout->addWidget(webview);
    setLayout(main_layout);
    setWindowTitle("Detailed info");
}






#include <QLabel>
#include <QRegularExpression>
PrismaCloudProxy::PrismaCloudProxy(QWidget *parent)
    : QWidget{parent},
      incidents_table(nullptr), start_index(0),
      web_page(this), base_site_url("http://status.prismacloud.com/history?page="), page_number(1),
      main_layout(new QVBoxLayout), button_prev(new QPushButton("Previous")), button_next(new QPushButton("Next"))
{

    connect (&web_page, &QWebEnginePage::loadFinished, this, &PrismaCloudProxy::pageLoadFinished);
    web_page.load(QUrl(base_site_url + QString::number(page_number)));



    auto descriptionLabel = new QLabel("It is start to load incidents from prismaCLOUD site. "
                                       "\nPlease, wait. If it takes too long, check internet connection and"
                                       "\nprismaCLOUD availability");
    descriptionLabel->setAlignment(Qt::AlignCenter);

    main_widget = descriptionLabel;
    main_layout->addWidget(descriptionLabel);

    auto button_layout = new QHBoxLayout;
    button_prev->setDisabled(true);
    button_prev->setFixedSize(150,75);
    button_next->setDisabled(true);
    button_next->setFixedSize(150,75);

    connect(button_next, &QAbstractButton::clicked, this, &PrismaCloudProxy::nextTablePage);
    connect(button_prev, &QAbstractButton::clicked, this, &PrismaCloudProxy::prevTablePage);

    button_layout->addWidget(button_prev, 0, Qt::AlignLeft);
    button_layout->addWidget(button_next, 0, Qt::AlignRight);
    main_layout->addLayout(button_layout);

    setLayout(main_layout);
}



QString month_to_number(QString month)
{
    if (month == "Jan")
        return QString::number(1);
    else if (month == "Feb")
        return QString::number(2);
    else if (month == "Mar")
        return QString::number(3);
    else if (month == "Apr")
        return QString::number(4);
    else if (month == "May")
        return QString::number(5);
    else if (month == "Jun")
        return QString::number(6);
    else if (month == "Jul")
        return QString::number(7);
    else if (month == "Aug")
        return QString::number(8);
    else if (month == "Sep")
        return QString::number(9);
    else if (month == "Oct")
        return QString::number(10);
    else if (month == "Nov")
        return QString::number(11);
    else if (month == "Dec")
        return QString::number(12);
    else
        return 0;
}



/* Parses page and eject information from it
*/
void PrismaCloudProxy::pageLoadFinished()
{
    web_page.toHtml([this] (const QString &page) {
        int next_page_number = 1;

        /* Divide page for segments of mont's data */
        QRegularExpression re ("<div class=\"month\">[^{]+</div></div></div>");
        re.setPatternOptions(QRegularExpression::InvertedGreedinessOption);
        auto months_match = re.globalMatch(page);


        for (auto match : months_match)
        {
            re.setPattern("\"year\">(?<year>\\d\\d\\d\\d)");
            auto current_year = re.match(match.captured()).captured("year");

            /* Early completion if year is 2020 - because site keep data since 2021 */
            if (current_year == "2020")
                next_page_number = 1;
            else
                next_page_number = page_number + 1;

            /* Find container for concrete incidents data */
            re.setPattern("incident-container\">[^{]+</var>");
            auto incidents = re.globalMatch(match.captured());

            for (auto inc_match : incidents)
            {
                Incident this_incident{0};
                this_incident.date += current_year;
                this_incident.date += '.';

                re.setPattern("(?<month>Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov|Dec)");
                this_incident.date += month_to_number(re.match(inc_match.captured()).captured("month"));
                this_incident.date += '.';

                re.setPattern("data-var=\"date\">(?<day>.+)<");
                auto test = re.match(inc_match.captured()).captured("day");
                this_incident.date += re.match(inc_match.captured()).captured("day");

                re.setPattern("<a href=\"(?<reference>(.+))\"");
                this_incident.reference += re.match(inc_match.captured()).captured("reference");

                re.setPattern("incident-title font-large\">(?<name>(.+))<");
                this_incident.name += re.match(inc_match.captured()).captured("name");

                re.setPattern("class=\"message incident-body color-primary\">(?<shortly>(.+))<");
                this_incident.short_description += re.match(inc_match.captured()).captured("shortly");

                parsed_incidents_list.push_back(this_incident);
            }
        }

        this->refreshTable();

        /* Set next page to parse and if it start page (page_number == 1) - finilize the work */
        page_number = next_page_number;
        if (page_number != 1)
            this->loadNextPage();
  });
}



void PrismaCloudProxy::loadNextPage()
{
    web_page.load(QUrl(base_site_url + QString::number(page_number)));
}



void PrismaCloudProxy::refreshTable()
{
    if (incidents_table == nullptr)
    {
        incidents_table = new IncidentsTable();

        auto temp = main_layout->replaceWidget(main_widget,incidents_table);

        delete temp;
    }


    if (start_index == 0)
    {
        if (button_prev->isEnabled())
            button_prev->setDisabled(true);
    }
    else
    {
        if (!button_prev->isEnabled())
            button_prev->setEnabled(true);
    }

    if (start_index+3 < parsed_incidents_list.size())
    {
        if (!button_next->isEnabled())
            button_next->setEnabled(true);
    }
    else
    {
        if (button_next->isEnabled())
            button_next->setDisabled(true);
    }

    incidents_table->refreshData(start_index, this->parsed_incidents_list);
}



void PrismaCloudProxy::nextTablePage()
{
    start_index += 3;
    refreshTable();
}

void PrismaCloudProxy::prevTablePage()
{
    start_index -= 3;
    refreshTable();
}
