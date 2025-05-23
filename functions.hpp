#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#pragma once

#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

namespace clientFunctions {

void setupLineEditStyle(QLineEdit *lineEdit);
void setupButtonStyle(QPushButton *button);
void updateErrorPosition(QWidget* errorContainer, QWidget* loginEdit);
void showError(QLabel* errorLabel, QWidget* errorContainer, QWidget* loginEdit, const QString& message);
void hideError(QLabel* errorLabel, QWidget* errorContainer);

}

#endif // FUNCTIONS_HPP
