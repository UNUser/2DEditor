#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "vertex.h"
#include "scene.h"

#include <QMessageBox>
#include <QFileDialog>
#include <QGraphicsScene>
#include <QGraphicsItem>

#define MSG_TIMEOUT 2000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    scene.setSceneRect(0.0, 0.0, 1000.0, 500.0);

    animationController = new AnimationController(&scene);

    ui->graphicsView->setMouseTracking(true);
    ui->graphicsView->setScene(&scene);
    ui->graphicsView->setRenderHint(QPainter::Antialiasing);
    ui->graphicsView->show();

    actionGroup = new QActionGroup(this);

    actionGroup->setExclusive(true);

    ui->actionSelectionMode->setChecked(true);
    ui->actionSelectionMode->setData(Scene::SelectionMode);
    ui->actionSelectionMode->setIcon(QIcon(":/Icons/select.png"));

    ui->actionPolylineMode->setData(Scene::PolylineMode);
    ui->actionPolylineMode->setIcon((QIcon(":/Icons/polyline.png")));

    actionGroup->addAction(ui->actionSelectionMode);
    actionGroup->addAction(ui->actionPolylineMode);

    connect(actionGroup, SIGNAL(triggered(QAction*)),
            this,        SLOT(changeSceneMode(QAction*)));

    connect(ui->globalChanges, SIGNAL(stateChanged(int)),
            this,              SLOT(setChangesMode(int)));

    connect(ui->addFrame, SIGNAL(clicked()),
            this,          SLOT(addFrame()));
    connect(ui->actionAddFrame, SIGNAL(triggered()),
            ui->addFrame,       SLOT(click()));

    connect(ui->deleteFrame, SIGNAL(clicked()),
            this,            SLOT(deleteFrame()));
    connect(ui->actionDeleteFrame, SIGNAL(triggered()),
            ui->deleteFrame,       SLOT(click()));

    connect(ui->currentFrame, SIGNAL(valueChanged(int)),
            this,             SLOT(setCurrentFrame(int)));

    connect(ui->actionOpenFile, SIGNAL(triggered()),
            this,               SLOT(openFile()));

    connect(ui->actionSaveFile, SIGNAL(triggered()),
            this,               SLOT(saveFile()));

    connect(ui->actionSaveFileAs, SIGNAL(triggered()),
            this,                 SLOT(saveFileAs()));

    connect(ui->actionNextFrame, SIGNAL(triggered()),
            ui->currentFrame,    SLOT(stepUp()));

    connect(ui->actionPreviousFrame, SIGNAL(triggered()),
            ui->currentFrame,        SLOT(stepDown()));

    setWindowTitle("Untitled");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeSceneMode(QAction *action)
{
    scene.setMode(Scene::Mode(action->data().toInt()));
}

void MainWindow::setChangesMode(int value)
{
    animationController->setGlobalChangesMode(value);
}

void MainWindow::addFrame()
{
    animationController->addFrame();
    ui->currentFrame->setMaximum(animationController->totalFrames());
    ui->totalFrames->setText(QString::number(animationController->totalFrames()));
    ui->currentFrame->setValue(ui->currentFrame->value() + 1);

    ui->statusBar->showMessage("New frame has been added", MSG_TIMEOUT);
}

void MainWindow::deleteFrame()
{
    animationController->deleteFrame();
    ui->currentFrame->setMaximum(animationController->totalFrames());
    ui->totalFrames->setText(QString::number(animationController->totalFrames()));

    ui->statusBar->showMessage("The frame has been deleted", MSG_TIMEOUT);
}

void MainWindow::setCurrentFrame(int frameIndex)
{
    int offset = frameIndex - animationController->getCurrentFrameIndex() - 1;

    if(offset < 0)
        for(; offset < 0; offset++)
            animationController->previousFrame();
    else
        for(; offset > 0; offset--)
            animationController->nextFrame();
}

void MainWindow::saveFile(bool saveAnyway)
{
    if(fileName.isEmpty())
    {
        saveFileAs();
        return;
    }

    if(animationController->isChanged() || scene.isChanged() || saveAnyway)
    {
        animationController->saveAnimation(fileName);
        ui->statusBar->showMessage("The file has been saved", MSG_TIMEOUT);
    }
    else
        ui->statusBar->showMessage("There are no modifications to save", MSG_TIMEOUT);
}

void MainWindow::saveFileAs()
{
    fileName = QFileDialog::getSaveFileName(this, "Save file", "", "Animation files: (*.anm)");

    if(!fileName.isEmpty())
        saveFile(true);

    this->setWindowTitle(fileName);
}

void MainWindow::openFile()
{
    if(animationController->isChanged() || scene.isChanged())
    {
        QMessageBox messageBox;

        messageBox.setText("The file has been modified.");
        messageBox.setInformativeText("Do you want to save the file?");
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int answer = messageBox.exec();

        switch (answer)
        {
        case QMessageBox::Yes:
            saveFile();
            break;
        case QMessageBox::No:
            break;
        case QMessageBox::Cancel:
            return;
            break;
        }
    }

     fileName = QFileDialog::getOpenFileName(this, "Open file", "", "Animation files: (*.anm)");

     if(fileName.isEmpty())
         return;

     animationController->loadAnimation(fileName);

     disconnect(ui->currentFrame, SIGNAL(valueChanged(int)),
                this,             SLOT(setCurrentFrame(int)));
     ui->currentFrame->setValue(ui->currentFrame->minimum());
     connect(ui->currentFrame, SIGNAL(valueChanged(int)),
             this,             SLOT(setCurrentFrame(int)));

     ui->currentFrame->setMaximum(animationController->totalFrames());
     ui->totalFrames->setText(QString::number(animationController->totalFrames()));
     setCurrentFrame(ui->currentFrame->value());

     this->setWindowTitle(fileName);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(animationController->isChanged() || scene.isChanged())
    {
        QMessageBox messageBox;

        messageBox.setText("The file has been modified.");
        messageBox.setInformativeText("Do you want to save the file before exit?");
        messageBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        int answer = messageBox.exec();

        switch (answer)
        {
        case QMessageBox::Yes:
            saveFile();
            event->accept();
            break;
        case QMessageBox::No:
            event->accept();
            break;
        case QMessageBox::Cancel:
            event->ignore();
            break;
        }
    }
}

