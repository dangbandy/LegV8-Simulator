#include <QtWidgets>
#include <QDebug>
#include "mainwindow.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
using namespace std;
MainWindow::MainWindow() : textEdit(new QPlainTextEdit)
{
    for(int i = 0; i < 1000; i++){
        MEM[i] = 0;
        STACK[i] = 0;
    }
    for(int i = 0; i < 32; i++)
        RFILE[i] = 0;
    setCentralWidget(textEdit);

    createActions();
    createStatusBar();
    createDockWindows();
    readSettings();

    connect(textEdit->document(), &QTextDocument::contentsChanged,
            this, &MainWindow::documentWasModified);

#ifndef QT_NO_SESSIONMANAGER
    QGuiApplication::setFallbackSessionManagementEnabled(false);
    connect(qApp, &QGuiApplication::commitDataRequest,
            this, &MainWindow::commitData);
#endif

    setCurrentFile(QString());
    setUnifiedTitleAndToolBarOnMac(true);
}
//! [2]

void MainWindow::itemClicked(QListWidgetItem * i){
    QString currentText = i->text();
    stringstream ss(currentText.toStdString());
    string temp;
    ss >> temp;
    int memNum;
    ss >> memNum;
    qDebug() << "regNum: " << memNum;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Memory Modification"),
                                         tr("Enter desired value[-128,127]: "), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if (ok && !text.isEmpty()){
        MEM[memNum] = text.toInt();
        qDebug() << QString(MEM[memNum]).toInt();
        QString newText = QString("MEMORY[ %1 ] = ").arg(memNum) + text;
        qDebug() << newText;
        i->setText(newText);
    }
}

void MainWindow::createDockWindows(){
    memoryDock = new QDockWidget(tr("Memory"), this);
    memoryDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    memoryList = new QListWidget(memoryDock);
    for(int i = 0; i < 1000; i++){
        QString r = QString("MEMORY[ %1 ] = ").arg(i);
        QString val = QString::number(MEM[i]);
        r += val;
        memoryList->addItems( QStringList() << r);
        //QListWidgetItem * currentItem = memoryList->item(i);
        //currentItem->setFlags(currentItem->flags () | Qt::ItemIsEditable);
    }
    connect(memoryList, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(itemClicked(QListWidgetItem *)));

    memoryDock->setWidget(memoryList);
    addDockWidget(Qt::RightDockWidgetArea, memoryDock);
    viewMenu->addAction(memoryDock->toggleViewAction());


    registerDock = new QDockWidget(tr("Registers Display"), this);
    registerList = new QListWidget(registerDock);
    for(int i = 0; i < 32; i++){
        QString r = QString("REGISTER[ %1 ] = ").arg(i);
        QString val = QString::number(RFILE[i]);
        r += val;
        registerList->addItems(QStringList() << r);
    }

    registerDock->setWidget(registerList);
    addDockWidget(Qt::RightDockWidgetArea, registerDock);
    viewMenu->addAction(registerDock->toggleViewAction());
}

void MainWindow::resetMem(){
    for(int i = 0; i < 1000; i++){
        MEM[i] = 0;
        STACK[i] = 0;
    }
    memoryList->clear();
    for(int i = 0; i < 1000; i++){
        QString r = QString("MEMORY[ %1 ] = ").arg(i);
        QString val = QString::number(MEM[i]);
        r += val;
        memoryList->addItems( QStringList() << r);
        //QListWidgetItem * currentItem = memoryList->item(i);
        //currentItem->setFlags(currentItem->flags () | Qt::ItemIsEditable);
    }
    connect(memoryList, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(itemClicked(QListWidgetItem *)));
}

void MainWindow::parse(string filename){
    ifstream inputFile(filename);
    string line;

    Instruction filler;
    PGM.push_back(filler);

    int number = 1;
    while( getline(inputFile,line) ){
        //cout << line << endl;
        Instruction in;
        in.parse(line, number);
        PGM.push_back(in);
        //cout << endl;
        number++;
    }
    inputFile.close();
    //cout << "Total lines: " << PGM.size() << endl;
}

void MainWindow::initSingle(){
    PGM.clear();
    for(int i = 0; i < 32; i++)
        RFILE[i] = 0;
    save();
    parse(curFile.toStdString());
    for(int i = 0; i < PGM.size(); i++){
        QString fl = QString::fromStdString(PGM[i].fullLine);
        //qDebug() << fl << endl;
    }
    findLabel("MAIN");
    execSingle();
}

void MainWindow::execSingle(){
    if( N > 0 && N < PGM.size() ){
        QString t = "Register Display: " + QString::fromStdString(PGM[N].fullLine);
        //qDebug() << t << endl;
        registerDock->setWindowTitle(t);
        int type = PGM[N].type;
        /*
           if(mode == 1){
           cout << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl;
           }
         */
        //outs << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl;
        if( type < 7 ){
            //R format
            execR1(type);
        }
        else if( type < 10 ){
            //R format
            execR2(type);
        }
        else if(type < 15){
            //I format
            execI1(type);
        }
        else if(type < 18){
            //I flags format
            execI2(type);
        }
        else if(type < 19){
            //B format
            execB(type);
        }
        else if(type < 31){
            //CB format
            execCB(type);
        }
        else if(type < 33){
            //D format
            execD1(type);
        }
        else if(type < 39){
            //D variant format
            execD2(type);
        }
        else if(type < 40){
            //BL statement
            execBL(type);
        }
        else if(type < 41){
            //BR statement
            execBR(type);
        }
        RFILE[31] = 0;

        registerList->clear();
        for(int i = 0; i < 32; i++){
            QString r = QString("REGISTER[%1] = ").arg(i);
            QString val = QString::number(RFILE[i]);
            r += val;
            registerList->addItems(QStringList() << r);
        }
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Please hit the Single-Step icon.");
        msgBox.exec();
    }

}
void MainWindow::execCommand(){
    PGM.clear();
    for(int i = 0; i < 32; i++)
        RFILE[i] = 0;
    //qDebug() << "Running to completion";
    save();
    parse(curFile.toStdString());
    qDebug() << curFile;
    for(int i = 0; i < PGM.size(); i++){
        QString fl = QString::fromStdString(PGM[i].fullLine);
        //qDebug() << fl << endl;
    }
    findLabel("MAIN");
    qDebug() << "N: " << N;
    //ofstream outs ("output.txt", ofstream::out);
    while( N > 0 && N < PGM.size() ){
        QString t = "Register Display: " + QString::fromStdString(PGM[N].fullLine);
        //qDebug() << t << endl;
        registerDock->setWindowTitle(t);
        int type = PGM[N].type;
        /*
           if(mode == 1){
           cout << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl;
           }
         */
        //outs << "Executing line " << PGM[N].lineNumber << ": " << PGM[N].fullLine<< endl;
        if( type < 7 ){
            //R format
            execR1(type);
        }
        else if( type < 10 ){
            //R format
            execR2(type);
        }
        else if(type < 15){
            //I format
            execI1(type);
        }
        else if(type < 18){
            //I flags format
            execI2(type);
        }
        else if(type < 19){
            //B format
            execB(type);
        }
        else if(type < 31){
            //CB format
            execCB(type);
        }
        else if(type < 33){
            //D format
            execD1(type);
        }
        else if(type < 39){
            //D variant format
            execD2(type);
        }
        else if(type < 40){
            //BL statement
            execBL(type);
        }
        else if(type < 41){
            //BR statement
            execBR(type);
        }
        RFILE[31] = 0;
        if(N < 1 || N > PGM.size()){
            registerList->clear();
            for(int i = 0; i < 32; i++){
                QString r = QString("REGISTER[%1] = ").arg(i);
                QString val = QString::number(RFILE[i]);
                r += val;
                qDebug() << r;
                registerList->addItems(QStringList() << r);
            }
        }

    }
    /*
       if( mode == 2 ){
       cout << "Finished executing program. Printing all registers...\n";
       for(int i = 0; i < 32; i++){
       cout << "REG[" << i << "] = " << RFILE[i] << endl;
       }
       }
     */



}

void MainWindow::findLabel(string label){
    bool found = false;
    N = 0;
    while(N < PGM.size() && !found){
        if( PGM[N].label == label )
            found = true;
        else
            N++;
    }
    /*
       if(N < PGM.size()){
       cout << label << " is at: " << N << endl;
       }
       else{
       cout << "Label not found.\n";
       N = PGM.size();
       }
     */
    if(N < 1 || N > PGM.size()){
        //cout << "Label not found.\n";
        N = PGM.size();
    }
}

void MainWindow::execR1(int type){
    int d = 0,j = 0,k = 0;
    d = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];

    switch(type){
    case 0://add
        RFILE[d] = RFILE[j] + RFILE[k];
        break;
    case 1://and
        RFILE[d] = RFILE[j] & RFILE[k];
        break;
    case 2://orr
        RFILE[d] = RFILE[j] | RFILE[k];
        break;
    case 3://eor
        RFILE[d] = RFILE[j] ^ RFILE[k];
        break;
    case 4://sub
        RFILE[d] = RFILE[j] - RFILE[k];
        break;
    case 5://lsr
        RFILE[d] = RFILE[j] >> k;
        break;
    case 6://lsl
        RFILE[d] = RFILE[j] << k;
        break;
    default:
        cout << "Error\n";
    }
    N++;
}

void MainWindow::execR2(int type){
    int d = 0,j = 0,k = 0;
    d = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];

    currentSFlagInstr = N;
    ConditionReg1 = RFILE[j];
    ConditionReg2 = RFILE[k];

    unsigned long long int t1;
    unsigned long long int t2;
    unsigned long long int t3;
    switch(type){
    case 7://adds
        RFILE[d] = RFILE[j] + RFILE[k];
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        if( (RFILE[j] > 0 && RFILE[k] > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && RFILE[k] < 0 && RFILE[d] > 0) )
            Overflow = 1;
        else
            Overflow = 0;
        t1 = RFILE[d];
        t2 = RFILE[j];
        t3 = RFILE[k];
        if( t1 < t2 || t1 < t3 )
            Carry = 1;
        else
            Carry = 0;
        break;
    case 8://ands
        RFILE[d] = RFILE[j] & RFILE[k];
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        Overflow = 0;//???
        Carry = 0;
        break;
    case 9://subs
        RFILE[d] = RFILE[j] - RFILE[k];
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        if( (RFILE[j] > 0 && RFILE[k] > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && RFILE[k] < 0 && RFILE[d] > 0) )
            Overflow = 1;
        else
            Overflow = 0;
        t1 = RFILE[d];
        t2 = RFILE[j];
        t3 = RFILE[k];
        if( t1 < t2 || t1 < t3 )
            Carry = 1;
        else
            Carry = 0;
        break;
    default:
        cout << "Error\n";
    }
    N++;
}

void MainWindow::execI1(int type){
    int d = 0, j = 0, k = 0;
    d = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];

    switch(type){
    case 10://orri
        RFILE[d] = RFILE[j] | k;
        break;
    case 11://eori
        RFILE[d] = RFILE[j] ^ k;
        break;
    case 12://addi
        RFILE[d] = RFILE[j] + k;
        break;
    case 13://andi
        RFILE[d] = RFILE[j] & k;
        break;
    case 14://subi
        RFILE[d] = RFILE[j] - k;
        break;
    default:
        cout << "Error\n";
    }
    N++;
}

void MainWindow::execI2(int type){
    int d = 0, j = 0, k = 0;
    d = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];

    currentSFlagInstr = N;
    ConditionReg1 = RFILE[j];
    ConditionReg2 = k;

    unsigned long long int t1;
    unsigned long long int t2;
    unsigned long long int t3;
    switch(type){
    case 15://addis
        RFILE[d] = RFILE[j] + k;
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        if( (RFILE[j] > 0 && k > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && k < 0 && RFILE[d] > 0) )
            Overflow = 1;
        else
            Overflow = 0;
        t1 = RFILE[d];
        t2 = RFILE[j];
        t3 = k;
        if( t1 < t2 || t1 < t3 )
            Carry = 1;
        else
            Carry = 0;
        break;
    case 16://subis
        RFILE[d] = RFILE[j] - k;
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        if( (RFILE[j] > 0 && k > 0 && RFILE[d] < 0) || (RFILE[j] < 0 && k < 0 && RFILE[d] > 0) )
            Overflow = 1;
        else
            Overflow = 0;
        t1 = RFILE[d];
        t2 = RFILE[j];
        t3 = k;
        if( t1 < t2 || t1 < t3 )
            Carry = 1;
        else
            Carry = 0;
        break;
        break;
    case 17://andis
        RFILE[d] = RFILE[j] & k;
        Negative = (RFILE[d] < 0 ? 1 : 0);
        Zero = (RFILE[d] == 0 ? 1 : 0);
        Overflow = 0;
        Carry = 0;
        break;
    default:
        cout << "Error\n";
    }
    N++;
}

void MainWindow::execB(int type){
    findLabel(PGM[N].paramLabel);
}

void MainWindow::execCB(int type){
    int d = 0;
    unsigned long long int UnsignedVal1 = ConditionReg1;
    unsigned long long int UnsignedVal2 = ConditionReg2;
    switch(type){
    case 19://EQ
        if(Zero == 1)
            findLabel(PGM[N].paramLabel);
        else
            N++;
        break;
    case 20://NE
        if(Zero == 0)
            findLabel(PGM[N].paramLabel);
        else
            N++;
        break;
    case 21://LT
        if( ConditionReg1 < ConditionReg2){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 22://LE
        if( ConditionReg1 <= ConditionReg2){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 23://GT
        if( ConditionReg1 > ConditionReg2){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 24://GE
        if( ConditionReg1 >= ConditionReg2){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 25://LO
        if( UnsignedVal1 < UnsignedVal2 ){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 26://LS
        if( UnsignedVal1 <= UnsignedVal2 ){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 27://HI
        if( UnsignedVal1 > UnsignedVal2 ){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 28://HS
        if( UnsignedVal1 >= UnsignedVal2 ){
            findLabel(PGM[N].paramLabel);
        }
        else
            N++;
        break;
    case 29://CBZ
        d = PGM[N].params[0];
        if(RFILE[d] == 0)
            findLabel(PGM[N].paramLabel);
        else
            N++;
        break;
    case 30://CBNZ
        d = PGM[N].params[0];
        if(RFILE[d] != 0)
            findLabel(PGM[N].paramLabel);
        else
            N++;
        break;
    default:
        cout << "Error\n";
    }
}

void MainWindow::execD1(int type){
    int i = 0, j = 0, k = 0;
    i = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];

    switch(type){
    case 31://stur
        if( j == 28 ){//stack
            for(int t = 0; t <= 7; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                STACK[ RFILE[j] + t + k ] = b;
            }

        }
        else{
            for(int t = 0; t <= 7; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                MEM[ RFILE[j] + t + k ] = b;
            }
        }
        break;
    case 32://ldur
        if( j == 28 ){//stack
            RFILE[i] = 0;
            for(int t = 0; t <= 7; t++){
                RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
            }
        }
        else{
            RFILE[i] = 0;
            for(int t = 0; t <= 7; t++){
                RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
            }
        }
        break;
    default:
        cout << "Error\n";
    }
    memoryList->clear();
    for(int i = 0; i < 1000; i++){
        QString r = QString("MEMORY[ %1 ] = ").arg(i);
        QString val = QString::number(MEM[i]);
        r += val;
        memoryList->addItems( QStringList() << r);
        //QListWidgetItem * currentItem = memoryList->item(i);
        //currentItem->setFlags(currentItem->flags () | Qt::ItemIsEditable);
    }
    connect(memoryList, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(itemClicked(QListWidgetItem *)));

    N++;
}

void MainWindow::execD2(int type){
    int i = 0, j = 0, k = 0;
    i = PGM[N].params[0];
    j = PGM[N].params[1];
    k = PGM[N].params[2];
    switch(type){
    case 33://sturb
        if( j == 28 ){//stack
            for(int t = 0; t <= 0; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                STACK[ RFILE[j] + t + k ] = b;
            }

        }
        else{
            for(int t = 0; t <= 0; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                MEM[ RFILE[j] + t + k ] = b;
            }
        }
        break;
    case 34://ldurb
        if( j == 28 ){//stack
            RFILE[i] = 0;
            for(int t = 0; t <= 0; t++){
                RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
            }
        }
        else{
            RFILE[i] = 0;
            for(int t = 0; t <= 0; t++){
                RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
            }
        }
        break;
    case 35://sturh
        if( j == 28 ){//stack
            for(int t = 0; t <= 1; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                STACK[ RFILE[j] + t + k ] = b;
            }

        }
        else{
            for(int t = 0; t <= 1; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                MEM[ RFILE[j] + t + k ] = b;
            }
        }
        break;
    case 36://ldurh
        if( j == 28 ){//stack
            RFILE[i] = 0;
            for(int t = 0; t <= 1; t++){
                RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
            }
        }
        else{
            RFILE[i] = 0;
            for(int t = 0; t <= 1; t++){
                RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
            }
        }
        break;
    case 37://sturw
        if( j == 28 ){//stack
            for(int t = 0; t <= 3; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                STACK[ RFILE[j] + t + k ] = b;
            }

        }
        else{
            for(int t = 0; t <= 3; t++){
                int b = (RFILE[i] >> (8*t)) & 0xff;
                MEM[ RFILE[j] + t + k ] = b;
            }
        }
        break;
    case 38://ldursw
        if( j == 28 ){//stack
            RFILE[i] = 0;
            for(int t = 0; t <= 3; t++){
                RFILE[i] = RFILE[i] | (STACK[RFILE[j] + t + k] << (8*t));
            }
        }
        else{
            RFILE[i] = 0;
            for(int t = 0; t <= 3; t++){
                RFILE[i] = RFILE[i] | ( MEM[RFILE[j] + t + k] << (8*t));
            }
        }
        break;
    default:
        cout << "Error\n";
    }
    memoryList->clear();
    for(int i = 0; i < 1000; i++){
        QString r = QString("MEMORY[ %1 ] = ").arg(i);
        QString val = QString::number(MEM[i]);
        r += val;
        memoryList->addItems( QStringList() << r);
        //QListWidgetItem * currentItem = memoryList->item(i);
        //currentItem->setFlags(currentItem->flags () | Qt::ItemIsEditable);
    }
    connect(memoryList, SIGNAL(itemClicked(QListWidgetItem *)), SLOT(itemClicked(QListWidgetItem *)));

    N++;
}

void MainWindow::execBL(int type){
    //BLcounter++;
    RFILE[30] = N+1;
    findLabel(PGM[N].paramLabel);
}

void MainWindow::execBR(int type){
    /* if(BLcounter <= 0){
       N = PGM.size();
       }
       else{
       cout << BLcounter-- << endl << endl;
     */
    N = RFILE[30];
    //}
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile(QString());
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    if (dialog.exec() != QDialog::Accepted)
        return false;
    return saveFile(dialog.selectedFiles().first());
}

void MainWindow::about()
{
    //QMessageBox::aboutQt(this,"fuck");
    QMessageBox::information(this, tr("Guide"),
            tr("<font size = 5><b>About:</b></font><br>"
               "This program simulate the execution of a LegV8 program."
               "The simulator will read from the text file, which contains the LegV8 code."
               "The LegV8 code must be syntically and semantically correct for this program to work."
               "The simulator will execute the instruction with label MAIN as the"
               "first instruction. Therefore, please identify a MAIN label instruction in the LegV8 code"
               "The LegV8 text file can be pulled from an existing text or created using the text "
               "editor utilities of this program.<br>"

               "<font size = 5><b>How to execute single-step:<b></font><br>"
               "After uploading or creating a valid LegV8 program in "
               "the text editor view, select the wrench and hammer icon to simulate"
               " the first instruction of the LegV8 program. Then select the arrow icon to simulate "
               "the next instruction on forth. Similarly, the same steps will be applied when using the "
               "Single-Step and Next-Step options in the toolbar's Build selections.<br>"

               "<font size = 5><b>How to execute to completion:<b></font><br>"
               "After uploading or creating a valid LegV8 program in "
               "the text editor view, select the green triangle icon to simulate the LegV8 program to"
               "completion. Similarly, the same step will be applied when using the "
               "All-Step option in the toolbar's Build selections.<br>"

               "<font size = 5><b>How to set memory value:<b></font><br>"
               "Select the memory location you wish to modify. Enter the desired"
               " value into the input dialog and press OK.<br>"

               "<font size = 5><b>How to reset memory:<b></font><br>"
               "Select the reset icon to reset all memory values back to zero."
               "The same applies for selecting Reset-Mem option in the toolbar's Build selections."));
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::createActions()
{

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QToolBar *fileToolBar = addToolBar(tr("File"));
    const QIcon newIcon = QIcon::fromTheme("document-new", QIcon(":/images/new.png"));
    QAction *newAct = new QAction(newIcon, tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, &QAction::triggered, this, &MainWindow::newFile);
    fileMenu->addAction(newAct);
    fileToolBar->addAction(newAct);

    const QIcon openIcon = QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
    QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, &QAction::triggered, this, &MainWindow::open);
    fileMenu->addAction(openAct);
    fileToolBar->addAction(openAct);

    const QIcon saveIcon = QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
    QAction *saveAct = new QAction(saveIcon, tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, &QAction::triggered, this, &MainWindow::save);
    fileMenu->addAction(saveAct);
    fileToolBar->addAction(saveAct);

    const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
    QAction *saveAsAct = fileMenu->addAction(saveAsIcon, tr("Save &As..."), this, &MainWindow::saveAs);
    saveAsAct->setShortcuts(QKeySequence::SaveAs);
    saveAsAct->setStatusTip(tr("Save the document under a new name"));

    fileMenu->addSeparator();

    const QIcon exitIcon = QIcon::fromTheme("application-exit");
    QAction *exitAct = fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
    exitAct->setShortcuts(QKeySequence::Quit);

    exitAct->setStatusTip(tr("Exit the application"));


    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    QToolBar *editToolBar = addToolBar(tr("Edit"));

#ifndef QT_NO_CLIPBOARD
    const QIcon cutIcon = QIcon::fromTheme("edit-cut", QIcon(":/images/cut.png"));
    QAction *cutAct = new QAction(cutIcon, tr("Cu&t"), this);

    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, &QAction::triggered, textEdit, &QPlainTextEdit::cut);
    editMenu->addAction(cutAct);
    editToolBar->addAction(cutAct);

    const QIcon copyIcon = QIcon::fromTheme("edit-copy", QIcon(":/images/copy.png"));
    QAction *copyAct = new QAction(copyIcon, tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, &QAction::triggered, textEdit, &QPlainTextEdit::copy);
    editMenu->addAction(copyAct);
    editToolBar->addAction(copyAct);

    const QIcon pasteIcon = QIcon::fromTheme("edit-paste", QIcon(":/images/paste.png"));
    QAction *pasteAct = new QAction(pasteIcon, tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, &QAction::triggered, textEdit, &QPlainTextEdit::paste);
    editMenu->addAction(pasteAct);
    editToolBar->addAction(pasteAct);

    menuBar()->addSeparator();


#endif // !QT_NO_CLIPBOARD

    viewMenu = menuBar()->addMenu(tr("&View"));

    QMenu *buildMenu = menuBar()->addMenu(tr("&Build"));
    QToolBar *buildToolBar = addToolBar(tr("Build"));

    const QIcon resetMemIcon = QIcon(":/images/reset.png");
    QAction *resetMemAct = new QAction(resetMemIcon, tr("&Reset-Mem"), this);
    resetMemAct->setShortcuts(QKeySequence::Replace);
    resetMemAct->setStatusTip(tr("Reset memory."));
    connect(resetMemAct, &QAction::triggered, this, &MainWindow::resetMem);
    buildMenu->addAction(resetMemAct);
    buildToolBar->addAction(resetMemAct);
    menuBar()->addSeparator();

    const QIcon runIcon = QIcon(":/images/run.png");
    QAction *runAct = new QAction(runIcon, tr("&All-Steps"), this);
    runAct->setShortcuts(QKeySequence::Refresh);
    runAct->setStatusTip(tr("Run LegV8 program to end."));
    connect(runAct, &QAction::triggered, this, &MainWindow::execCommand);
    buildMenu->addAction(runAct);
    buildToolBar->addAction(runAct);

    const QIcon singleStepIcon = QIcon(":/images/single-step.png");
    QAction *singleStepAct = new QAction(singleStepIcon, tr("&Single-Step"), this);
    singleStepAct->setShortcuts(QKeySequence::Bold);
    singleStepAct->setStatusTip(tr("Run LegV8 program one instruction at a time."));
    connect(singleStepAct, &QAction::triggered, this, &MainWindow::initSingle);
    buildMenu->addAction(singleStepAct);
    buildToolBar->addAction(singleStepAct);
    menuBar()->addSeparator();

    const QIcon nextStepIcon = QIcon(":/images/next.png");
    QAction *nextStepAct = new QAction(nextStepIcon, tr("&Next-Step"), this);
    nextStepAct->setShortcuts(QKeySequence::AddTab);
    nextStepAct->setStatusTip(tr("Run next LegV8 program instruction."));
    connect(nextStepAct, &QAction::triggered, this, &MainWindow::execSingle);
    buildMenu->addAction(nextStepAct);
    buildToolBar->addAction(nextStepAct);
    menuBar()->addSeparator();

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *aboutAct = helpMenu->addAction(tr("&Guide"), this, &MainWindow::about);
    aboutAct->setStatusTip(tr("Show guide."));

#ifndef QT_NO_CLIPBOARD
    cutAct->setEnabled(false);

    copyAct->setEnabled(false);
    connect(textEdit, &QPlainTextEdit::copyAvailable, cutAct, &QAction::setEnabled);
    connect(textEdit, &QPlainTextEdit::copyAvailable, copyAct, &QAction::setEnabled);
#endif // !QT_NO_CLIPBOARD
}
void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::readSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (geometry.isEmpty()) {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    } else {
        restoreGeometry(geometry);
    }
}

void MainWindow::writeSettings()
{
    QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
    settings.setValue("geometry", saveGeometry());
}

bool MainWindow::maybeSave()
{
    if (!textEdit->document()->isModified())
        return true;
    const QMessageBox::StandardButton ret
            = QMessageBox::warning(this, tr("Application"),
                                   tr("The document has been modified.\n"
                                      "Do you want to save your changes?"),
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    switch (ret) {
    case QMessageBox::Save:
        return save();
    case QMessageBox::Cancel:
        return false;
    default:
        break;
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName), file.errorString()));
        return;
    }

    QTextStream in(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot write file %1:\n%2.")
                             .arg(QDir::toNativeSeparators(fileName),
                                  file.errorString()));
        return false;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = curFile;
    if (curFile.isEmpty())
        shownName = "untitled.txt";
    setWindowFilePath(shownName);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}
#ifndef QT_NO_SESSIONMANAGER
void MainWindow::commitData(QSessionManager &manager)
{
    if (manager.allowsInteraction()) {
        if (!maybeSave())
            manager.cancel();
    } else {
        // Non-interactive: save without asking
        if (textEdit->document()->isModified())
            save();
    }
}
#endif
