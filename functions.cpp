#include "functions.hpp"

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

}
