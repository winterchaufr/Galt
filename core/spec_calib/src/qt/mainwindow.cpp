/*
 * mainwindow.cpp
 *
 *  Copyright (c) 2014 Nouka Technologies. All rights reserved.
 *
 *  This file is part of galt.
 *
 *  Created on: 11/7/2014
 *      Author: gareth
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "posecalibrationview.h"
#include "spectrumcalibrationview.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
  ui(new Ui::MainWindow), mainWidget_(0)
{
  ui->setupUi(this);
  
  QObject::connect(ui->actionCalibrate_Pose,SIGNAL(triggered(bool)),this,SLOT(calibratePoseAction(bool)));
  QObject::connect(ui->actionCalibrate_spectrum,SIGNAL(triggered(bool)),this,SLOT(calibrateSpectrumAction(bool)));
  QObject::connect(ui->actionQuit,SIGNAL(triggered(bool)),this,SLOT(quitAction(bool)));  
  
  setMode(CalibratePose);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{  
  //  trigger quit when window closed
  quit();
  QMainWindow::closeEvent(event);
}

void MainWindow::quit() {
  ros::shutdown();
}

void MainWindow::calibratePoseAction(bool) {
  //  switch modes
  setMode(MainWindow::CalibratePose);
}

void MainWindow::calibrateSpectrumAction(bool) {
  setMode(MainWindow::CalibrateSpectrum);
}

void MainWindow::quitAction(bool) {
  quit(); 
}

void MainWindow::setMode(MainWindow::Mode mode) {
  
  if (mode != mode_ || !mainWidget_) {
    
    if (mainWidget_) {
      delete mainWidget_;
      mainWidget_=0;
    }
    mode_ = mode;
    
    QString statusMessage;
    if (mode_ == CalibratePose) {
      mainWidget_ = new PoseCalibrationView(this);
      statusMessage = "Calibrating pose";
    } 
    else if (mode_ == CalibrateSpectrum) {
      mainWidget_ = new SpectrumCalibrationView(this);
      statusMessage = "Calibrating spectrum";
    }
    
    //  add to window
    if (mainWidget_) {
      ui->horizontalLayout->addWidget(mainWidget_);
    }
    this->statusBar()->showMessage(statusMessage);
  }
}