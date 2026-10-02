#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QuickSearch.h"

class QuickSearch : public QMainWindow
{
    Q_OBJECT

public:
    QuickSearch(QWidget *parent = nullptr);
    ~QuickSearch();

private:
    Ui::QuickSearchClass ui;
};

