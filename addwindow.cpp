#include "addwindow.h"
#include "ui_addwindow.h"

AddWindow::AddWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddWindow)
{
    ui->setupUi(this);
}

AddWindow::~AddWindow()
{
    delete ui;
}

int AddWindow::getNumber()
{
    return ui->number->value();
}

QString AddWindow::getDate()
{
    return ui->date->text();
}

QString AddWindow::getStock()
{
    return ui->stock->currentText();
}

QString AddWindow::getNomenclature()
{
    return ui->nomenclature->currentText();
}

int AddWindow::getQuantity()
{
    return ui->quantity->value();
}

double AddWindow::getPrice()
{
    return ui->price->value();
}

void AddWindow::setStock(QSqlQueryModel *model)
{
    ui->stock->setModel(model);
    ui->stock->setCurrentIndex(-1);
}

void AddWindow::setNomenclature(QSqlQueryModel *model)
{
    ui->nomenclature->setModel(model);
    ui->nomenclature->setCurrentIndex(-1);
}

void AddWindow::setEnabledPrice(bool flag)
{
    if(flag)
        ui->price->setEnabled(true);
    else
        ui->price->setEnabled(false);
}

void AddWindow::clearFields()
{
    ui->number->setValue(0);
    ui->date->setDate(QDate::currentDate());
    ui->stock->setCurrentIndex(-1);
    ui->nomenclature->setCurrentIndex(-1);
    ui->quantity->setValue(0);
    ui->price->setValue(0.0);
}

void AddWindow::on_OK_clicked()
{
    this->hide();
    emit sendOK();
}

void AddWindow::on_Cancel_clicked()
{
    this->hide();
    emit sendCancel();
}

void AddWindow::on_stock_currentIndexChanged(int index)
{
    emit sendChangeStock();
}
