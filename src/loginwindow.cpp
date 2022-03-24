/*******************************************************************
MIT License

Copyright (c) 2022 Loyio Hex

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
********************************************************************/
#include "include/loginwindow.h"
#include "include/mainwindow.h"
#include "include/utils.hpp"
#include "ui_loginwindow.h"
#include "network/client.cpp"

LoginWindow::LoginWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LoginWindow)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}



void LoginWindow::on_pbtn_enter_clicked()
{
    try{
        QString ipaddress = ui->lineEdit_ipaddr->text();
        QString portnum = ui->lineEdit_port->text();
      if (ipaddress == "" || portnum == ""){
          ui->statusbar->setStyleSheet("color: red");
          ui->statusbar->showMessage("Please input the ip address and port number!!!");
      }else{

          boost::asio::io_context io_context;

          tcp::resolver resolver(io_context);
          auto endpoints = resolver.resolve(ipaddress.toStdString(), portnum.toStdString());

          chat_client c(io_context);

          c.start(endpoints);

          MainWindow w;
          w.show();
      }
    }
    catch (std::exception& e){
        ui->statusbar->setStyleSheet("color: red");
        QString errMsg = e.what();
        ui->statusbar->showMessage(errMsg);
    }
}

