#include "ConsoleWindow.h"
#include "./ui_consolewindow.h"

/*
 * I hope all code here is self-explaining
*/

ConsoleWindow::ConsoleWindow(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
    ui->setupUi(this);
    {
        console = ui->textEdit;
        console->setReadOnly(true);
        console->setStyleSheet("background-color: black; color: white;");
    }

    {
        startButton = ui->pushButton;
        connect(startButton, &QPushButton::clicked, this, &ConsoleWindow::startButtonPress);
    }

    {
        maskLineEdit = ui->maskLineEdit;
    }

    {
        deleteInCheckBox = ui->deleteInCheckBox;
    }

    {
        destDirComboBox = ui->destDirComboBox;
        destDirComboBox->setDuplicatesEnabled(false);

        destDirComboBox->addItem("<Обзор>");
        connect(destDirComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, &ConsoleWindow::onDestDirIndexChanged);

    }

    {
        actionOnExistComboBox = ui->actionOnExistComboBox;
        for(auto itemName: coreLogic.handler->getActionsOnExistNames()){
            actionOnExistComboBox->addItem(QString::fromStdString(itemName));
        }
    }

    {
        repeatNoRadioButton = ui->repeatNoRadioButton;
        repeatYesRadioButton = ui->repeatYesRadioButton;

        repeatNoRadioButton->setChecked(true);
        connect(repeatNoRadioButton, &QRadioButton::clicked, this, &ConsoleWindow::repeatRadioButtonChange);
        connect(repeatYesRadioButton, &QRadioButton::clicked, this, &ConsoleWindow::repeatRadioButtonChange);
    }

    {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &ConsoleWindow::ProcessFiles);

        repeatTimerTimeEdit = ui->repeatTimerTimeEdit;
        repeatTimerTimeEdit->setDisabled(true);
    }

    {
        modifierOperatorComboBox = ui->modifierOperatorComboBox;
        for(auto itemName: coreLogic.handler->getActionsNames()){
            modifierOperatorComboBox->addItem(QString::fromStdString(itemName));
        }
    }

    {
        modifierValueLineEdit = ui->modifierValueLineEdit;
        QIntValidator *validator = new QIntValidator(INT_MIN, INT_MAX, this);
        modifierValueLineEdit->setValidator(validator);
    }

}

void ConsoleWindow::consoleLog(const std::string&& outString){
    console->append(QString::fromStdString(outString));
}

void ConsoleWindow::repeatRadioButtonChange() {
    if(repeatNoRadioButton->isChecked()){
        repeatTimerTimeEdit->setDisabled(true);
    }
    else{
        repeatTimerTimeEdit->setDisabled(false);
    }
}

void ConsoleWindow::onDestDirIndexChanged() {
    if (destDirComboBox->currentText() == "<Обзор>"){
        QString dirName = QFileDialog::getExistingDirectory(this, tr("Путь до папки сохранения"), "./");
        destDirComboBox->setCurrentText(dirName);
        if(dirName != ""){
            destDirComboBox->addItem(dirName);
        }
        else{
            destDirComboBox->setCurrentIndex(-1);
        }
    }
}

void ConsoleWindow::ProcessFiles() {
    try{
        coreLogic.handler->processFiles(
            [this](const std::string&& outString) {
                consoleLog(std::move(outString));
            }
        );
    }
    catch(const std::exception& e)
    {
        consoleLog("Caught exception: '" + std::string(e.what()) + "'\n");
    }
    catch(...){
        consoleLog("Unpredictable error!\n");
    }
}

void ConsoleWindow::startButtonPress() {

    coreLogic.handler->setDir(destDirComboBox->currentText().toStdString());
    coreLogic.handler->setMaskString(maskLineEdit->text().toStdString());
    coreLogic.handler->setDeleteFlag(deleteInCheckBox->isChecked());
    coreLogic.handler->setActionOnExist(actionOnExistComboBox->currentText().toStdString());
    coreLogic.handler->setModifier(modifierValueLineEdit->text().toStdString());
    coreLogic.handler->setModifyAction(modifierOperatorComboBox->currentText().toStdString());

    if (repeatYesRadioButton->isChecked() || timer->isActive()){
        if (timer->isActive()){
            timer->stop();
            startButton->setText("Старт");
        }
        else{
            consoleLog("Timer seted: " + std::to_string(repeatTimerTimeEdit->time().msecsSinceStartOfDay()));
            startButton->setText("Стоп");
            timer->setInterval(repeatTimerTimeEdit->time().msecsSinceStartOfDay());
            timer->start();
        }
    }
    else{
        ProcessFiles();
    }
}
