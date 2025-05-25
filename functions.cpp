#include "functions.hpp"
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

namespace clientFunctions {

void setupLineEditStyle(QLineEdit *lineEdit) {
    lineEdit->setStyleSheet(
        "background-color: #141315; "
        "color: #FFFFFF; "
        "border: 1px solid #555555; "
        "border-radius: 5px; "
        "padding: 5px; "
        "font-size: 14px;"
        );
    lineEdit->setFixedHeight(30);
    lineEdit->setFixedWidth(300);
}

void setupButtonStyle(QPushButton *button) {
    button->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   color: #ffffff;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 8px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );
    button->setFixedHeight(35);
    button->setFixedWidth(300);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

void updateErrorPosition(QWidget* errorContainer, QWidget* loginEdit) {
    if (!errorContainer || !loginEdit)
        return;

    QPoint loginPos = loginEdit->pos();
    errorContainer->move(loginPos.x(), loginPos.y() - errorContainer->height() - 5);
    errorContainer->setFixedWidth(loginEdit->width());
}

void showError(QLabel* errorLabel, QWidget* errorContainer, QWidget* loginEdit, const QString& message) {
    errorLabel->setText(message);
    errorLabel->adjustSize();

    int targetHeight = errorLabel->sizeHint().height();
    errorContainer->setFixedHeight(targetHeight);
    errorContainer->setFixedWidth(loginEdit->width());

    updateErrorPosition(errorContainer, loginEdit);
    errorContainer->show();
}

void hideError(QLabel* errorLabel, QWidget* errorContainer) {
    errorContainer->hide();
    errorLabel->clear();
}

bool confirmWindow(const QString& message)
{
    QDialog dialog;
    dialog.setWindowTitle("Подтверждение");
    dialog.setWindowFlags(Qt::FramelessWindowHint);
    // Рамка только для всего окна
    dialog.setStyleSheet("QDialog {"
                         "background-color: #1B191D;"
                         "border: 2px solid #3A393E;"
                         "border-radius: 5px;"
                         "}");

    dialog.setFixedSize(250, 100);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    layout->setContentsMargins(10, 10, 10, 10); // Отступы внутри окна

    QLabel *messageLabel = new QLabel(message, &dialog);
    messageLabel->setStyleSheet(
        "color: #FFFFFF;"
        "font-size: 14px;"
        "background: transparent;"  // Прозрачный фон
        "border: none;"           // Убираем рамку
        "margin: 0;"              // Убираем внешние отступы
        "padding: 0;"             // Убираем внутренние отступы
        );
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    QPushButton *yesButton = new QPushButton("Да", &dialog);
    QPushButton *noButton = new QPushButton("Нет", &dialog);

    setupButtonStyle(yesButton);
    setupButtonStyle(noButton);

    yesButton->setFixedWidth(100);
    noButton->setFixedWidth(100);

    buttonsLayout->addWidget(yesButton);
    buttonsLayout->addWidget(noButton);
    buttonsLayout->setAlignment(Qt::AlignCenter);
    buttonsLayout->setSpacing(20);

    layout->addWidget(messageLabel);
    layout->addLayout(buttonsLayout);

    QObject::connect(yesButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    QObject::connect(noButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    return dialog.exec() == QDialog::Accepted;
}

}

