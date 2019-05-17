
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "instruction.hpp"
using namespace std;

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QDockWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QListWidget;
class QSessionManager;

QT_END_NAMESPACE

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

    void loadFile(const QString &fileName);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void itemClicked(QListWidgetItem * i);
    void newFile();
    void open();
    bool save();
    bool saveAs();
    void about();
    void documentWasModified();
#ifndef QT_NO_SESSIONMANAGER
    void commitData(QSessionManager &);
#endif

private:
    void createDockWindows();
    void createActions();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    bool saveFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    QString curFile;
    QMenu *viewMenu;
    QPlainTextEdit *textEdit;
    QListWidget *memoryList;
    QListWidget *registerList;
    QListWidget *stackList;
    QDockWidget *registerDock;
    QDockWidget *memoryDock;
    QDockWidget *stackDock;

    void parse(string filename);
    void resetMem();
    void editMemory();

    void execCommand();
    void initSingle();
    void execSingle();

    void findLabel(string label);

    void execR1(int type);
    void execR2(int type);
    void execI1(int type);
    void execI2(int type);
    void execB(int type);
    void execCB(int type);
    void execD1(int type);
    void execD2(int type);
    void execBL(int type);
    void execBR(int type);

    int mode = 0;

    vector<Instruction> PGM;
    long long int RFILE[32];
    char *MEM = new char[1000];
    char *STACK= new char[1000];

    int N = 0;

    int currentSFlagInstr = -1;
    int ConditionReg1 = 0;
    int ConditionReg2 = 0;

    int Zero = 0, Overflow = 0, Negative = 0, Carry = 0;
};
//! [0]

#endif
