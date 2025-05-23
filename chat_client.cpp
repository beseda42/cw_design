#include "chat_client.hpp"
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QScreen>
#include <QGuiApplication>
#include "functions.hpp"

ChatClient::ChatClient(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Васька <Design Mode>");
    setWindowIcon(QIcon(":/images/app_icon.ico"));
    setMinimumSize(300, 490);
    setupGUI();
    showAuthScreen();

    resize(350, 500);
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            QGuiApplication::primaryScreen()->availableGeometry()
            )
        );
}

ChatClient::~ChatClient()
{
    // ничего
}

void ChatClient::setupGUI()
{
    authWidget = new QWidget(this);
    QVBoxLayout *authLayout = new QVBoxLayout(authWidget);

    QLabel *imageLabel = new QLabel(this);
    QPixmap logo(":/images/logo.png");

    if(!logo.isNull()) {
        logo = logo.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(logo);
        imageLabel->setAlignment(Qt::AlignCenter);
    }

    loginEdit = new QLineEdit(this);
    loginEdit->setPlaceholderText("Логин");
    passwordEdit = new QLineEdit(this);
    passwordEdit->setPlaceholderText("Пароль");
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Вход", this);
    registrationButton = new QPushButton("Регистрация", this);

    clientFunctions::setupLineEditStyle(loginEdit);
    clientFunctions::setupLineEditStyle(passwordEdit);

    clientFunctions::setupButtonStyle(loginButton);
    clientFunctions::setupButtonStyle(registrationButton);

    authLayout->addWidget(imageLabel);

    // Контейнер для ошибок
    errorContainer = new QWidget(authWidget);
    errorContainer->setStyleSheet("background: transparent;");
    errorContainer->setFixedHeight(0);
    errorContainer->setVisible(false);

    errorLabel = new QLabel(errorContainer);
    errorLabel->setStyleSheet(
        "color: #dd2d4a;"
        "font-size: 12px;"
        "padding: 2px;"
        "margin-top: 2px;"
        "background-color: #1b191d;"
        "border-radius: 4px;"
        );
    errorLabel->setAlignment(Qt::AlignCenter);
    errorLabel->setWordWrap(true);
    errorLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout *errorLayout = new QVBoxLayout(errorContainer);
    errorLayout->setContentsMargins(0, 0, 0, 0);
    errorLayout->setSpacing(0);
    errorLayout->addWidget(errorLabel);

    authLayout->addWidget(loginEdit, 0, Qt::AlignHCenter);
    authLayout->addWidget(passwordEdit, 0, Qt::AlignHCenter);
    authLayout->addWidget(loginButton, 0, Qt::AlignHCenter);
    authLayout->addWidget(registrationButton, 0, Qt::AlignHCenter);
    authLayout->addStretch();

    authWidget->setStyleSheet(
        "QLineEdit { background-color: #141315; color: #FFFFFF; border: 1px solid #555555; border-radius: 5px; padding: 5px; font-size: 14px; }"
        "QPushButton { background-color: #512799; color: #ffffff; border: none; border-radius: 5px; padding: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: #3c2760; }"
        "QPushButton:pressed { background-color: #463366; }"
        "QLabel { color: #FFFFFF; font-size: 14px; }"
        );

    authLayout->setSpacing(10);
    authLayout->setContentsMargins(20, 20, 20, 20);

    connect(loginButton, &QPushButton::clicked, this, &ChatClient::onLoginButtonClicked);
    connect(registrationButton, &QPushButton::clicked, this, &ChatClient::registrationGUI);

    chatWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(chatWidget);
    mainTabs = new QTabWidget(this);
    QWidget *chatTab = new QWidget();
    QWidget *callTab = new QWidget();
    QWidget *settingsTab = new QWidget();

    QVBoxLayout *chatLayout = new QVBoxLayout(chatTab);
    chatListWidget = new QListWidget(this);
    messageDisplay = new QTextEdit(this);
    messageDisplay->setReadOnly(true);
    messageInput = new QLineEdit(this);
    sendMessageButton = new QPushButton("Send", this);
    createChatButton = new QPushButton("Create Chat", this);
    chatNameInput = new QLineEdit(this);
    chatNameInput->setPlaceholderText("Chat Name");
    inviteUserButton = new QPushButton("Invite", this);
    userComboBox = new QComboBox(this);
    getUserListButton = new QPushButton("User List", this);
    deleteFromChatButton = new QPushButton("Remove from Chat", this);
    deleteAccountButton = new QPushButton("Delete Account", this);
    getChatUsersButton = new QPushButton("Chat Users", this);
    connectionStatusLabel = new QLabel("Chat: Design Mode", this);
    connectionStatusLabel->setStyleSheet("color: blue;");

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->addWidget(messageInput);
    inputLayout->addWidget(sendMessageButton);

    QHBoxLayout *chatCreationLayout = new QHBoxLayout();
    chatCreationLayout->addWidget(chatNameInput);
    chatCreationLayout->addWidget(createChatButton);

    QHBoxLayout *userManagementLayout = new QHBoxLayout();
    userManagementLayout->addWidget(userComboBox);
    userManagementLayout->addWidget(inviteUserButton);
    userManagementLayout->addWidget(getUserListButton);
    userManagementLayout->addWidget(deleteFromChatButton);
    userManagementLayout->addWidget(deleteAccountButton);
    userManagementLayout->addWidget(getChatUsersButton);

    chatLayout->addWidget(chatListWidget);
    chatLayout->addWidget(messageDisplay);
    chatLayout->addLayout(inputLayout);
    chatLayout->addLayout(chatCreationLayout);
    chatLayout->addLayout(userManagementLayout);
    chatLayout->addWidget(connectionStatusLabel);

    QVBoxLayout *callLayout = new QVBoxLayout(callTab);
    startCallButton = new QPushButton("Start Call", this);
    stopCallButton = new QPushButton("Stop Call", this);
    stopCallButton->setEnabled(false);
    voipStatusLabel = new QLabel("VoIP: Design Mode", this);
    voipStatusLabel->setStyleSheet("color: blue;");
    callLayout->addWidget(startCallButton);
    callLayout->addWidget(stopCallButton);
    callLayout->addWidget(voipStatusLabel);
    callLayout->addStretch();

    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsTab);
    QLabel *settingsPlaceholder = new QLabel("Settings will be implemented later", this);
    settingsLayout->addWidget(settingsPlaceholder);
    settingsLayout->addStretch();

    mainTabs->addTab(chatTab, "Chat");
    mainTabs->addTab(callTab, "Calls");
    mainTabs->addTab(settingsTab, "Settings");
    mainLayout->addWidget(mainTabs);

    chatWidget->setStyleSheet(
        "QListWidget { background-color: #141315; color: #FFFFFF; border: 1px solid #8380b6; border-radius: 5px; font-size: 14px; }"
        "QListWidget::item { padding: 5px; }"
        "QListWidget::item:selected { background-color: #512799; }"
        "QTextEdit { background-color: #1b191d; color: #FFFFFF; border: 1px solid #8380b6; border-radius: 5px; font-size: 14px; }"
        "QLineEdit { background-color: #141315; color: #FFFFFF; border: 1px solid #8380b6; border-radius: 5px; padding: 5px; font-size: 14px; }"
        "QComboBox { background-color:  #141315; color: #FFFFFF; border: 1px solid #8380b6; border-radius: 5px; padding: 5px; font-size: 14px; }"
        "QComboBox::drop-down { border: none; width: 20px; }"
        "QComboBox QAbstractItemView { background-color: #141315; color: #FFFFFF; selection-background-color: #512799; }"
        "QPushButton { background-color: #512799; color: #FFFFFF; border: none; border-radius: 5px; padding: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: #3c2760; }"
        "QPushButton:pressed { background-color: #463366; }"
        "QTabWidget::pane { border: 1px solid #8380b6; background-color: #1b191d; }"
        "QTabBar::tab { background-color: #141315; color: #FFFFFF; padding: 8px; border-radius: 5px; }"
        "QTabBar::tab:selected { background-color: #512799; }"
        );

    chatListWidget->setMinimumHeight(200);
    messageDisplay->setMinimumHeight(300);
    chatListWidget->setMaximumWidth(300);

    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    setStyleSheet("QMainWindow { background-color: #1b191d; }");

    connect(sendMessageButton, &QPushButton::clicked, this, &ChatClient::onSendMessageButtonClicked);
    connect(chatListWidget, &QListWidget::itemClicked, this, &ChatClient::onChatSelected);
    connect(createChatButton, &QPushButton::clicked, this, &ChatClient::onCreateChatButtonClicked);
    connect(inviteUserButton, &QPushButton::clicked, this, &ChatClient::onInviteUserButtonClicked);
    connect(getUserListButton, &QPushButton::clicked, this, &ChatClient::onGetUserListButtonClicked);
    connect(deleteFromChatButton, &QPushButton::clicked, this, &ChatClient::onDeleteFromChatButtonClicked);
    connect(deleteAccountButton, &QPushButton::clicked, this, &ChatClient::onDeleteAccountButtonClicked);
    connect(getChatUsersButton, &QPushButton::clicked, this, &ChatClient::onGetChatUsersButtonClicked);
    connect(startCallButton, &QPushButton::clicked, this, &ChatClient::onStartCallButtonClicked);
    connect(stopCallButton, &QPushButton::clicked, this, &ChatClient::onStopCallButtonClicked);
}

void ChatClient::registrationGUI()
{
    clearAuthWidget();

    authWidget = new QWidget(this);
    QVBoxLayout *authLayout = new QVBoxLayout(authWidget);

    QLabel *imageLabel = new QLabel(authWidget);
    QPixmap logo(":/images/logo.png");

    if(!logo.isNull()) {
        logo = logo.scaled(300, 300, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        imageLabel->setPixmap(logo);
        imageLabel->setAlignment(Qt::AlignCenter);
    }

    nameEdit = new QLineEdit(authWidget);
    nameEdit->setPlaceholderText("Name");
    loginEdit = new QLineEdit(authWidget);
    loginEdit->setPlaceholderText("Login");
    passwordEdit = new QLineEdit(authWidget);
    passwordEdit->setPlaceholderText("Password");
    passwordEdit->setEchoMode(QLineEdit::Password);

    QPushButton *registerButton = new QPushButton("Зарегистрироваться", authWidget);
    QPushButton *backButton = new QPushButton("Назад", authWidget);

    clientFunctions::setupLineEditStyle(nameEdit);
    clientFunctions::setupLineEditStyle(loginEdit);
    clientFunctions::setupLineEditStyle(passwordEdit);

    clientFunctions::setupButtonStyle(registerButton);
    clientFunctions::setupButtonStyle(backButton);

    authLayout->addWidget(imageLabel);
    authLayout->addWidget(nameEdit, 0, Qt::AlignHCenter);
    authLayout->addWidget(loginEdit, 0, Qt::AlignHCenter);
    authLayout->addWidget(passwordEdit, 0, Qt::AlignHCenter);
    authLayout->addWidget(registerButton, 0, Qt::AlignHCenter);
    authLayout->addWidget(backButton, 0, Qt::AlignHCenter);
    authLayout->addStretch();

    authWidget->setStyleSheet(
        "QLineEdit { background-color: #141315; color: #FFFFFF; border: 1px solid #555555; border-radius: 5px; padding: 5px; font-size: 14px; }"
        "QPushButton { background-color: #512799; color: #ffffff; border: none; border-radius: 5px; padding: 8px; font-size: 14px; }"
        "QPushButton:hover { background-color: #3c2760; }"
        "QPushButton:pressed { background-color: #463366; }"
        "QLabel { color: #FFFFFF; font-size: 14px; }"
        );

    connect(registerButton, &QPushButton::clicked, this, &ChatClient::onRegisterButtonClicked);
    connect(backButton, &QPushButton::clicked, this, [this]() {
        clearAuthWidget();
        setupGUI();
        showAuthScreen();
    });

    showAuthScreen();
}

void ChatClient::clientGUI()
{
    clearAuthWidget();

    authWidget = new QWidget(this);
    QVBoxLayout *authLayout = new QVBoxLayout(authWidget);

    QStackedWidget *contentStack = new QStackedWidget(authWidget);

    // КОНТАКТЫ
    QWidget *searchWidget = new QWidget();
    QVBoxLayout *searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(0);

    QLabel *contactsLabel = new QLabel("Контакты", searchWidget);
    contactsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    contactsLabel->setFixedHeight(50);
    contactsLabel->setStyleSheet(
        "background-color: #1b191d;"
        "color: white;"
        "font-size: 18px;"
        "font-weight: bold;"
        "border: 2px solid #3A393E;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        );

    QWidget *contactsContent = new QWidget(searchWidget);
    contactsContent->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    searchLayout->addWidget(contactsLabel);
    searchLayout->addWidget(contactsContent);
    searchLayout->setStretchFactor(contactsContent, 1);

    // ЧАТЫ
    QWidget *chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(0);

    QLabel *chatLabel = new QLabel("Чаты", chatWidget);
    chatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    chatLabel->setFixedHeight(50);
    chatLabel->setStyleSheet(
        "background-color: #1b191d;"
        "color: white;"
        "font-size: 18px;"
        "font-weight: bold;"
        "border: 2px solid #3A393E;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        );

    QWidget *chatContent = new QWidget(chatWidget);
    chatContent->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    chatLayout->addWidget(chatLabel);
    chatLayout->addWidget(chatContent);
    chatLayout->setStretchFactor(chatContent, 1);

    // НАСТРОЙКИ
    QWidget *settingsWidget = new QWidget();
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsWidget);
    settingsLayout->setContentsMargins(0, 0, 0, 0);
    settingsLayout->setSpacing(0);

    QLabel *settingsLabel = new QLabel("Настройки", settingsWidget);
    settingsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    settingsLabel->setFixedHeight(50);
    settingsLabel->setStyleSheet(
        "background-color: #1b191d;"
        "color: white;"
        "font-size: 18px;"
        "font-weight: bold;"
        "border: 2px solid #3A393E;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        );

    QWidget *settingsContent = new QWidget(settingsWidget);
    settingsContent->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    settingsLayout->addWidget(settingsLabel);
    settingsLayout->addWidget(settingsContent);
    settingsLayout->setStretchFactor(settingsContent, 1);

    contentStack->addWidget(searchWidget);
    contentStack->addWidget(chatWidget);
    contentStack->addWidget(settingsWidget);

    // Окно клиента
    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(0);

    contactsButton = new HoverButton("contacts");
    contactsButton->setIconSize(QSize(60, 60));
    contactsButton->setFixedSize(50, 50);

    HoverButton *chatButton = new HoverButton("chat");
    chatButton->setIconSize(QSize(60, 60));
    chatButton->setFixedSize(50, 50);

    HoverButton *settingsButton = new HoverButton("settings");
    settingsButton->setIconSize(QSize(60, 60));
    settingsButton->setFixedSize(50, 50);

    QString buttonStyle =
        "QPushButton {"
        "   background-color: none;"
        "   border: none;"
        "   border-radius: 0;"
        "}"
        "QPushButton:hover {"
        "   background-color: none;"
        "}"
        "QPushButton:pressed {"
        "   background-color: none;"
        "}";

    contactsButton->setStyleSheet(buttonStyle);
    chatButton->setStyleSheet(buttonStyle);
    settingsButton->setStyleSheet(buttonStyle);

    navLayout->addWidget(contactsButton);
    navLayout->addWidget(chatButton);
    navLayout->addWidget(settingsButton);

    connect(contactsButton, &QPushButton::clicked, [contentStack]() {
        contentStack->setCurrentIndex(0);
    });

    connect(chatButton, &QPushButton::clicked, [contentStack]() {
        contentStack->setCurrentIndex(1);
    });

    connect(settingsButton, &QPushButton::clicked, [contentStack]() {
        contentStack->setCurrentIndex(2);
    });

    authLayout->addWidget(contentStack);
    authLayout->addLayout(navLayout);

    showAuthScreen();
}

void ChatClient::clearAuthWidget()
{
    if (authWidget) {
        authWidget->deleteLater();
        authWidget = nullptr;
    }
}


void ChatClient::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    clientFunctions::updateErrorPosition(errorContainer, loginEdit);
}


void ChatClient::showError(const QString& message)
{
    clientFunctions::showError(errorLabel, errorContainer, loginEdit, message);
}

void ChatClient::hideError()
{
    clientFunctions::hideError(errorLabel, errorContainer);
}

void ChatClient::updateErrorPosition()
{
    clientFunctions::updateErrorPosition(errorContainer, loginEdit);
}

void ChatClient::showAuthScreen()
{
    setCentralWidget(authWidget);
    authWidget->show();
    chatWidget->hide();
}

void ChatClient::showChatScreen()
{
    setCentralWidget(chatWidget);
    chatWidget->show();
    authWidget->hide();

    chatListWidget->clear();
    chatListWidget->addItem("Chat 1");
    chatListWidget->addItem("Chat 2");
    userComboBox->clear();
    userComboBox->addItem("User 1");
    userComboBox->addItem("User 2");
    appendMessage("User 1", "Hello!", QDateTime::currentMSecsSinceEpoch());
}

void ChatClient::appendMessage(const QString &userName, const QString &text, qint64 date)
{
    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(date);
    QString formattedMessage = QString("<b>%1</b> (%2): %3")
                                   .arg(userName)
                                   .arg(dateTime.toString("hh:mm:ss"))
                                   .arg(text);
    messageDisplay->append(formattedMessage);
}

void ChatClient::onRegisterButtonClicked()
{
    if (loginEdit->text().isEmpty() || passwordEdit->text().isEmpty() || nameEdit->text().isEmpty()) {
        loginEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        passwordEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        nameEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        showError("Введите логин и пароль");
        return;
    }
    // todo: register
    clientGUI();
}

void ChatClient::onLoginButtonClicked()
{
    if (loginEdit->text().isEmpty() || passwordEdit->text().isEmpty()) {
        loginEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        passwordEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        showError("Введите логин и пароль");
        return;
    }
    // todo: login
    clientGUI();
}

void ChatClient::onSendMessageButtonClicked()
{
    if (messageInput->text().isEmpty()) return;
    appendMessage("You", messageInput->text(), QDateTime::currentMSecsSinceEpoch());
    messageInput->clear();
}

void ChatClient::onChatSelected(QListWidgetItem *item)
{
    messageDisplay->clear();
    appendMessage("System", "Selected chat: " + item->text(), QDateTime::currentMSecsSinceEpoch());
}

void ChatClient::onCreateChatButtonClicked()
{
    if (chatNameInput->text().isEmpty()) {
        QMessageBox::warning(this, "Error", "Chat name cannot be empty");
        return;
    }
    chatListWidget->addItem(chatNameInput->text());
    chatNameInput->clear();
}

void ChatClient::onInviteUserButtonClicked()
{
    if (userComboBox->currentIndex() == -1) {
        QMessageBox::warning(this, "Error", "Select a user");
        return;
    }
    QMessageBox::information(this, "Invite", "Invited " + userComboBox->currentText() + " (design mode)");
}

void ChatClient::onGetUserListButtonClicked()
{
    userComboBox->clear();
    userComboBox->addItem("User 1");
    userComboBox->addItem("User 2");
    QMessageBox::information(this, "User List", "User list updated (design mode)");
}

void ChatClient::onDeleteFromChatButtonClicked()
{
    QMessageBox::information(this, "Remove", "User removed from chat (design mode)");
}

void ChatClient::onDeleteAccountButtonClicked()
{
    QMessageBox::information(this, "Delete Account", "Account deleted (design mode)");
    showAuthScreen();
}

void ChatClient::onGetChatUsersButtonClicked()
{
    userComboBox->clear();
    userComboBox->addItem("User 1");
    userComboBox->addItem("User 2");
    QMessageBox::information(this, "Chat Users", "Chat users updated (design mode)");
}

void ChatClient::onStartCallButtonClicked()
{
    startCallButton->setEnabled(false);
    stopCallButton->setEnabled(true);
    voipStatusLabel->setText("VoIP: Connected (design mode)");
    voipStatusLabel->setStyleSheet("color: green;");
}

void ChatClient::onStopCallButtonClicked()
{
    startCallButton->setEnabled(true);
    stopCallButton->setEnabled(false);
    voipStatusLabel->setText("VoIP: Disconnected (design mode)");
    voipStatusLabel->setStyleSheet("color: blue;");
}
