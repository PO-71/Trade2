#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    addWindow = new AddWindow();
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(QApplication::applicationDirPath() + "/trade.sqlite");
    if(!db.open())
        QMessageBox::critical(this, tr("ERROR!"), db.lastError().text());
    else
    {
        db.exec("pragma foreign_keys=1;");
        table = new QSqlRelationalTableModel(0, db);
        ui->tableView->setModel(table);
    }

    ui->button_add->setEnabled(false);
    ui->button_delete->setEnabled(false);

    mapTable[QString("nomenclature")] = NOMENCLATURE;
    mapTable[QString("stock")] = STOCK;
    mapTable[QString("remnants")] = REMNANTS;
    mapTable[QString("purchase")] = PURCHASE;
    mapTable[QString("sale")] = SALE;

    connect(ui->action_nomenclature, SIGNAL(triggered()), this, SLOT(SelectNomenclature()));
    connect(ui->action_stock, SIGNAL(triggered()), this, SLOT(SelectStock()));
    connect(ui->action_remnants, SIGNAL(triggered()), this, SLOT(SelectRemnants()));
    connect(ui->action_purchase, SIGNAL(triggered()), this, SLOT(SelectPurchase()));
    connect(ui->action_sale, SIGNAL(triggered()), this, SLOT(SelectSale()));
    connect(addWindow, SIGNAL(sendOK()), this, SLOT(GetOK()));
    connect(addWindow, SIGNAL(sendCancel()), this, SLOT(GetCancel()));
    connect(addWindow, SIGNAL(sendChangeStock()), this, SLOT(GetChangeStock()));
}

MainWindow::~MainWindow()
{
    db.close();
    delete ui;
}

void MainWindow::SelectNomenclature()
{
    SelectTable(QString("nomenclature"));
}

void MainWindow::SelectStock()
{
    SelectTable(QString("stock"));
}

void MainWindow::SelectRemnants()
{
    SelectTable(QString("remnants"));
}

void MainWindow::SelectPurchase()
{
    SelectTable(QString("purchase"));
}

void MainWindow::SelectSale()
{
    SelectTable(QString("sale"));
}

void MainWindow::SelectTable(QString nameTable)
{
    ui->button_add->setEnabled(true);
    ui->button_delete->setEnabled(true);
    ui->tableView->setEditTriggers(QAbstractItemView::DoubleClicked);
    table = new QSqlRelationalTableModel(0, db);
    table->setTable(nameTable);
    table->setEditStrategy(QSqlTableModel::OnManualSubmit);

    switch (mapTable[nameTable]) {
    case NOMENCLATURE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Артикул"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Наценка, %"), Qt::DisplayRole);
        this->setWindowTitle("Trade / Номенклатура");
        break;
    }
    case STOCK:
    {
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        this->setWindowTitle("Trade / Склады");
        break;
    }
    case REMNANTS:
    {
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Сумма закупки, руб."), Qt::DisplayRole);
        table->setRelation(1, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(2, QSqlRelation("nomenclature", "art", "name"));
        ui->button_add->setEnabled(false);
        ui->button_delete->setEnabled(false);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        this->setWindowTitle("Trade / Остатки на складах");
        break;
    }
    case PURCHASE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(5, Qt::Horizontal, QObject::tr("Цена закупки, руб."), Qt::DisplayRole);
        table->setRelation(2, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(3, QSqlRelation("nomenclature", "art", "name"));
        ui->button_delete->setEnabled(false);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        this->setWindowTitle("Trade / Закупки");
        break;
    }
    case SALE:
    {
        table->setHeaderData(0, Qt::Horizontal, QObject::tr("Номер"), Qt::DisplayRole);
        table->setHeaderData(1, Qt::Horizontal, QObject::tr("Дата"), Qt::DisplayRole);
        table->setHeaderData(2, Qt::Horizontal, QObject::tr("Склад"), Qt::DisplayRole);
        table->setHeaderData(3, Qt::Horizontal, QObject::tr("Номенклатура"), Qt::DisplayRole);
        table->setHeaderData(4, Qt::Horizontal, QObject::tr("Количество, шт."), Qt::DisplayRole);
        table->setHeaderData(5, Qt::Horizontal, QObject::tr("Цена продажи, руб."), Qt::DisplayRole);
        table->setRelation(2, QSqlRelation("stock", "id_stock", "name"));
        table->setRelation(3, QSqlRelation("nomenclature", "art", "name"));
        ui->button_delete->setEnabled(false);
        ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        this->setWindowTitle("Trade / Продажи");
        break;
    }
    default:
        break;
    }

    table->select();
    ui->tableView->setModel(table);
    ui->tableView->horizontalHeader()->setStretchLastSection(true);
    ui->tableView->setItemDelegate(new QSqlRelationalDelegate(ui->tableView));
    ui->tableView->show();
    if(nameTable == "remnants" || nameTable == "stock")
        ui->tableView->setColumnHidden(0, true);
    else
        ui->tableView->setColumnHidden(0, false);
}

void MainWindow::on_button_add_clicked()
{
    int count = table->rowCount();
    table->insertRow(count);
    if(mapTable[table->tableName()] == PURCHASE || mapTable[table->tableName()] == SALE)
    {
        addWindow->clearFields();
        QSqlQueryModel *queryModelStock = new QSqlQueryModel;
        queryModelStock->setQuery(QString("SELECT name FROM stock"));
        addWindow->setStock(queryModelStock);
        if(mapTable[table->tableName()] == PURCHASE)
        {
            QSqlQueryModel *queryModelNomenclature = new QSqlQueryModel;
            queryModelNomenclature->setQuery(QString("SELECT name FROM nomenclature"));
            addWindow->setNomenclature(queryModelNomenclature);
            addWindow->setEnabledPrice(true);
        }
        else if(mapTable[table->tableName()] == SALE)
        {
            GetChangeStock();
            addWindow->setEnabledPrice(false);
        }
        addWindow->show();
    }
}

void MainWindow::on_button_delete_clicked()
{
    int row = ui->tableView->selectionModel()->currentIndex().row();
    table->removeRow(row);
    if(!table->submitAll())
    {
        QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
        table->revertAll();
    }
}

void MainWindow::GetOK()
{
    int row = table->rowCount() - 1;
    QSqlQueryModel *queryModel = new QSqlQueryModel;
    int number = addWindow->getNumber();
    QString date = addWindow->getDate();
    QString stock = addWindow->getStock();
    QString nomenclature = addWindow->getNomenclature();
    int quantity = addWindow->getQuantity();
    double price = addWindow->getPrice();
    /* получаем внешние ключи */
    // получаем id_stock
    QString str_query = "SELECT id_stock FROM stock WHERE name = '" + stock + "'";
    queryModel->setQuery(str_query);
    QString id_stock = queryModel->data(queryModel->index(0, 0)).toString();
    // получаем art
    str_query = "SELECT art FROM nomenclature WHERE name = '" + nomenclature + "'";
    queryModel->setQuery(str_query);
    QString art = queryModel->data(queryModel->index(0, 0)).toString();
    if(number > 0)
        ui->tableView->model()->setData(ui->tableView->model()->index(row, 0), number);
    ui->tableView->model()->setData(ui->tableView->model()->index(row, 1), date);
    ui->tableView->model()->setData(ui->tableView->model()->index(row, 2), id_stock);
    ui->tableView->model()->setData(ui->tableView->model()->index(row, 3), art);
    ui->tableView->model()->setData(ui->tableView->model()->index(row, 4), quantity);
    ui->tableView->model()->setData(ui->tableView->model()->index(row, 5), price);

    if(mapTable[table->tableName()] == SALE)
    {
        // ищем записи об остатках по складу и номенклатуре
        str_query = "SELECT id_record, quantity, amount FROM remnants WHERE id_stock = " + id_stock + " AND art = " + art + " AND quantity >= " + QString("%1").arg(quantity);
        queryModel->setQuery(str_query);
        if(QDate::fromString(date, "dd.MM.yyyy") >= QDate::currentDate())
        {
            if(queryModel->rowCount() > 0)
            {
                // рассчет закупочной цены
                double purchase_price = queryModel->data(queryModel->index(0, 2)).toDouble() / queryModel->data(queryModel->index(0, 1)).toDouble();
                purchase_price = (floor(purchase_price * 100 + 0.5) / 100.0); // округлить цену до 2 знаков после запятой
                str_query = "SELECT extra_charge FROM nomenclature WHERE art = " + art; // определяем % наценки
                queryModel->setQuery(str_query);
                // рассчет цены для продажи = цена закупки * (100% + % наценки) / 100%
                double sale_price = purchase_price * double(100 + queryModel->data(queryModel->index(0, 0)).toInt()) / 100.0;
                sale_price = (floor(sale_price * 100 + 0.5) / 100.0); // округлить цену до 2 знаков после запятой
                ui->tableView->model()->setData(ui->tableView->model()->index(row, 5), sale_price);
                if(!table->submitAll())
                {
                    QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
                    addWindow->show();
                }
                else
                {
                    // списывание
                    double amount = double(quantity) * purchase_price;
                    str_query = "UPDATE remnants SET quantity = quantity - " + QString("%1").arg(quantity) + ", amount = amount - " + QString::number(amount, 'f', 2) + " WHERE id_stock = " + id_stock + " AND art = " + art;
                    queryModel->setQuery(str_query);
                }
            }
            else
            {
                QMessageBox::critical(this, tr("ERROR!"), QString("При списании номенклатуры '" + nomenclature + "' со склада '" + stock + "' образуется отрицательный остаток."));
                addWindow->show();
            }
        }
        else
        {
            QMessageBox::critical(this, tr("ERROR!"), QString("Дата продажи не может быть меньше текущей."));
            addWindow->show();
        }
    }
    else if(mapTable[table->tableName()] == PURCHASE)
    {
        if(QDate::fromString(date, "dd.MM.yyyy") <= QDate::currentDate())
        {
            if(!table->submitAll())
            {
                QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
                addWindow->show();
            }
            else
            {
                // ищем записи об остатках по складу и номенклатуре
                str_query = "SELECT id_record FROM remnants WHERE id_stock = " + id_stock + " AND art = " + art;
                queryModel->setQuery(str_query);
                // поступление
                double amount = double(quantity) * price;
                if(queryModel->rowCount() > 0)
                    str_query = "UPDATE remnants SET quantity = quantity + " + QString("%1").arg(quantity) + ", amount = amount + " + QString::number(amount, 'f', 2) + " WHERE id_stock = " + id_stock + " AND art = " + art;
                else
                    str_query = "INSERT INTO remnants(id_stock, art, quantity, amount) VALUES (" + id_stock + "," + art + "," + QString("%1").arg(quantity) + "," + QString::number(amount, 'f', 2) + ")";
                queryModel->setQuery(str_query);
            }
        }
        else
        {
            QMessageBox::critical(this, tr("ERROR!"), QString("Дата закупки не может быть больше текущей."));
            addWindow->show();
        }
    }
}

void MainWindow::GetCancel()
{
    table->revertAll();
}

void MainWindow::GetChangeStock()
{
    if(mapTable[table->tableName()] == SALE)
    {
        QSqlQueryModel *queryModel = new QSqlQueryModel;
        QString str_query = "SELECT id_stock FROM stock WHERE name = '" + addWindow->getStock() + "'";
        queryModel->setQuery(str_query);
        QString id_stock = queryModel->data(queryModel->index(0, 0)).toString();
        QSqlQueryModel *queryModelNomenclature = new QSqlQueryModel;
        queryModelNomenclature->setQuery(QString("SELECT DISTINCT name FROM remnants, nomenclature WHERE (remnants.art = nomenclature.art) AND (quantity > 0) AND (id_stock = " + id_stock + ")"));
        addWindow->setNomenclature(queryModelNomenclature);
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if(ui->tableView->hasFocus() && (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) && (mapTable[table->tableName()] == NOMENCLATURE || mapTable[table->tableName()] == STOCK))
    {
        if(!table->submitAll())
        {
            QMessageBox::critical(this, tr("ERROR!"), table->lastError().text());
            table->revertAll();
        }
    }
}
