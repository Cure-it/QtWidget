#include "testtab.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

//! [0]
TestTab::TestTab(QWidget *parent)
    : QWidget(parent)
{
    auto descriptionLabel = new QLabel(tr("There are currently no contacts in your address book. "
                                          "\nClick Add to add new contacts."));

    auto addButton = new QPushButton(tr("Add"));

    connect(addButton, &QAbstractButton::clicked, this, &TestTab::addEntry);

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(descriptionLabel);
    mainLayout->addWidget(addButton, 0, Qt::AlignCenter);

    setLayout(mainLayout);
}
//! [0]

//! [1]
void TestTab::addEntry()
{

}
//! [1]
