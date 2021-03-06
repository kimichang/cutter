#include "BacktraceWidget.h"
#include "ui_BacktraceWidget.h"
#include "utils/JsonModel.h"

#include "MainWindow.h"

BacktraceWidget::BacktraceWidget(MainWindow *main, QAction *action) :
    CutterDockWidget(main, action),
    ui(new Ui::BacktraceWidget)
{
    ui->setupUi(this);

    // setup backtrace model
    QString PC = Core()->getRegisterName("PC");
    QString SP = Core()->getRegisterName("SP");
    modelBacktrace->setHorizontalHeaderItem(0, new QStandardItem(PC));
    modelBacktrace->setHorizontalHeaderItem(1, new QStandardItem(SP));
    modelBacktrace->setHorizontalHeaderItem(2, new QStandardItem(tr("Frame Size")));
    modelBacktrace->setHorizontalHeaderItem(3, new QStandardItem(tr("Func Name")));
    modelBacktrace->setHorizontalHeaderItem(4, new QStandardItem(tr("Description")));
    viewBacktrace->setStyleSheet("QTableView {font-family: mono}");
    viewBacktrace->setModel(modelBacktrace);
    ui->verticalLayout->addWidget(viewBacktrace);

    connect(Core(), &CutterCore::refreshAll, this, &BacktraceWidget::updateContents);
    connect(Core(), &CutterCore::seekChanged, this, &BacktraceWidget::updateContents);
}

BacktraceWidget::~BacktraceWidget() {}

void BacktraceWidget::updateContents()
{
    setBacktraceGrid();
}

void BacktraceWidget::setBacktraceGrid()
{
    QJsonArray backtraceValues = Core()->getBacktrace().array();
    int i = 0;
    for (QJsonValueRef value : backtraceValues) {
        QJsonObject backtraceItem = value.toObject();
        QString progCounter = RAddressString(backtraceItem["pc"].toVariant().toULongLong());
        QString stackPointer = RAddressString(backtraceItem["sp"].toVariant().toULongLong());
        int frameSize = backtraceItem["frame_size"].toInt();
        QString funcName = backtraceItem["fname"].toString();
        QString desc = backtraceItem["desc"].toString();

        QStandardItem *rowPC = new QStandardItem(progCounter);
        QStandardItem *rowSP = new QStandardItem(stackPointer);
        QStandardItem *rowFrameSize = new QStandardItem(frameSize);
        QStandardItem *rowFuncName = new QStandardItem(funcName);
        QStandardItem *rowDesc = new QStandardItem(desc);

        modelBacktrace->setItem(i, 0, rowPC);
        modelBacktrace->setItem(i, 1, rowSP);
        modelBacktrace->setItem(i, 2, rowFrameSize);
        modelBacktrace->setItem(i, 3, rowFuncName);
        modelBacktrace->setItem(i, 4, rowDesc);
        i++;
    }
    viewBacktrace->setModel(modelBacktrace);
    viewBacktrace->resizeColumnsToContents();;
}
