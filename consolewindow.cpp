#include "ConsoleWindow.h"
#include "./ui_consolewindow.h"

ConsoleWindow::ConsoleWindow(QWidget *parent) : QWidget(parent), ui(new Ui::Form) {
    ui->setupUi(this);
    {
        console = ui->textEdit;
        console->setReadOnly(true);
        console->setStyleSheet("background-color: black; color: white;");
    }

    {
        startButton = ui->pushButton;
        connect(startButton, &QPushButton::clicked, this, &ConsoleWindow::writeToConsole);
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
        actionOnExistComboBox->addItem("Удалить");
        actionOnExistComboBox->addItem("Заменить");
        actionOnExistComboBox->addItem("Добавить с счетчиком");
    }

    {
        repeatNoRadioButton = ui->repeatNoRadioButton;
        repeatYesRadioButton = ui->repeatYesRadioButton;

        repeatNoRadioButton->setChecked(true);
        connect(repeatNoRadioButton, &QRadioButton::clicked, this, &ConsoleWindow::repeatRadioButtonChange);
        connect(repeatYesRadioButton, &QRadioButton::clicked, this, &ConsoleWindow::repeatRadioButtonChange);
    }

    {
        repeatTimerTimeEdit = ui->repeatTimerTimeEdit;
        repeatTimerTimeEdit->setDisabled(true);
    }

    {
        modifierOperatorComboBox = ui->modifierOperatorComboBox;
        modifierOperatorComboBox->addItem("AND");
        modifierOperatorComboBox->addItem("OR");
        modifierOperatorComboBox->addItem("XOR");
        modifierOperatorComboBox->addItem("Сместить влево");
        modifierOperatorComboBox->addItem("Сместить вправо");
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

void ConsoleWindow::writeToConsole() {
    console->append("Console Output.");
}
