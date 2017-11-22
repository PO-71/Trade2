#ifndef ADDWINDOW_H
#define ADDWINDOW_H

#include <QWidget>
#include <QtSql>

namespace Ui {
class AddWindow;
}

class AddWindow : public QWidget
{
    Q_OBJECT
signals:
    void sendOK();
    void sendCancel();
    void sendChangeStock();
public:
    explicit AddWindow(QWidget *parent = 0);
    ~AddWindow();
    int getNumber();
    QString getDate();
    QString getStock();
    QString getNomenclature();
    int getQuantity();
    double getPrice();
    void setStock(QSqlQueryModel *model);
    void setNomenclature(QSqlQueryModel *model);
    void setEnabledPrice(bool flag);
    void clearFields();

private slots:
    void on_OK_clicked();
    void on_Cancel_clicked();
    void on_stock_currentIndexChanged(int index);

private:
    Ui::AddWindow *ui;
};

#endif // ADDWINDOW_H
