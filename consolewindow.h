#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTimeEdit>
#include <QCheckBox>
#include <QRadioButton>
#include <QIntValidator>
#include <QFileDialog>
#include <QOverload>

namespace Ui {
class Form;
}

class ConsoleWindow : public QWidget {
    Q_OBJECT

public:
    ConsoleWindow(QWidget *parent = nullptr);
    void consoleLog(const std::string&& outString);
private slots:
    void writeToConsole();
    void repeatRadioButtonChange();
    void onDestDirIndexChanged();

private:
    Ui::Form *ui;
    QTextEdit* console;
    QPushButton* startButton;  
    QWidget *horizontalLayoutWidget;
    QHBoxLayout *horizontalLayout;
    QTextEdit *textEdit;
    QVBoxLayout *mainLayout;
    QVBoxLayout *maskLayout;
    QLabel *maskLabel;
    QLineEdit *maskLineEdit;
    QVBoxLayout *deleteLayout;
    QCheckBox *deleteInCheckBox;
    QVBoxLayout *destDirLayout;
    QLabel *destDirLabel;
    QComboBox *destDirComboBox;
    QVBoxLayout *onExistLayout;
    QLabel *actionOnExistLabel;
    QComboBox *actionOnExistComboBox;
    QVBoxLayout *repeatLayout;
    QLabel *needRepeatLabel;
    QRadioButton *repeatNoRadioButton;
    QRadioButton *repeatYesRadioButton;
    QVBoxLayout *timerLayout;
    QLabel *repeatTimerLabel;
    QTimeEdit *repeatTimerTimeEdit;
    QVBoxLayout *operatorLayout;
    QLabel *modifierOperatorLabel;
    QComboBox *modifierOperatorComboBox;
    QVBoxLayout *modifierLayout;
    QLabel *modifierValueLabel;
    QLineEdit *modifierValueLineEdit;
    QPushButton *pushButton;
};

#endif // CONSOLEWINDOW_H
