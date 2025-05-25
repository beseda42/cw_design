#include "chat_client.hpp"
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>
#include <QScreen>
#include <QGuiApplication>
#include <QScrollArea>
#include <QButtonGroup>
#include <QRadioButton>
#include <QPainter>
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
        "background-color: none;"
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

    nameEditR = new QLineEdit(authWidget);
    nameEditR->setPlaceholderText("Имя");
    loginEditR = new QLineEdit(authWidget);
    loginEditR->setPlaceholderText("Логин");
    passwordEditR = new QLineEdit(authWidget);
    passwordEditR->setPlaceholderText("Пароль");
    passwordEditR->setEchoMode(QLineEdit::Password);

    QPushButton *registerButton = new QPushButton("Зарегистрироваться", authWidget);
    QPushButton *backButton = new QPushButton("Назад", authWidget);

    clientFunctions::setupLineEditStyle(nameEditR);
    clientFunctions::setupLineEditStyle(loginEditR);
    clientFunctions::setupLineEditStyle(passwordEditR);

    clientFunctions::setupButtonStyle(registerButton);
    clientFunctions::setupButtonStyle(backButton);

    authLayout->addWidget(imageLabel);
    authLayout->addWidget(nameEditR, 0, Qt::AlignHCenter);
    authLayout->addWidget(loginEditR, 0, Qt::AlignHCenter);
    authLayout->addWidget(passwordEditR, 0, Qt::AlignHCenter);
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

// ----------------------------------------------------- КОНТАКТЫ -----------------------------------------------------
    QWidget *searchWidget = new QWidget();
    QVBoxLayout *searchLayout = new QVBoxLayout(searchWidget);
    searchLayout->setContentsMargins(0, 0, 0, 0);
    searchLayout->setSpacing(0);

    QWidget *contactsHeader = new QWidget(searchWidget);
    contactsHeader->setStyleSheet(
        "background-color: #1b191d;"
        "color: white;"
        "border: 2px solid #3A393E;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        );
    contactsHeader->setFixedHeight(50);

    QHBoxLayout *headerLayout = new QHBoxLayout(contactsHeader);
    headerLayout->setContentsMargins(10, 0, 10, 0);
    headerLayout->setSpacing(10);

    QLabel *contactsLabel = new QLabel("Контакты", contactsHeader);
    contactsLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    contactsLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );

    QPushButton *requestsButton = new QPushButton("⇩", contactsHeader);
    requestsButton->setFixedSize(30, 30);
    requestsButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 30px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );

    QPushButton *addContactButton = new QPushButton("+", contactsHeader);
    addContactButton->setFixedSize(30, 30);
    addContactButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 30px;"
        "   font-weight: bold;"
        "   padding-bottom: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );

    headerLayout->addWidget(contactsLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(requestsButton);
    headerLayout->addWidget(addContactButton);

    QWidget *contactsContent = new QWidget(searchWidget);
    contactsContent->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    QVBoxLayout *contactsContentLayout = new QVBoxLayout(contactsContent);
    contactsContentLayout->setContentsMargins(10, 10, 10, 10);
    contactsContentLayout->setSpacing(10);

    QWidget *searchContainer = new QWidget(contactsContent);
    QHBoxLayout *searchContainerLayout = new QHBoxLayout(searchContainer);
    searchContainerLayout->setContentsMargins(0, 0, 0, 0);
    searchContainerLayout->setSpacing(0);

    QLineEdit *searchContactEdit = new QLineEdit(searchContainer);
    searchContactEdit->setPlaceholderText("Имя...");
    searchContactEdit->setStyleSheet(
        "background-color: #141315;"
        "color: #FFFFFF;"
        "border: 1px solid #555555;"
        "border-right: none;"
        "border-top-left-radius: 5px;"
        "border-bottom-left-radius: 5px;"
        "border-top-right-radius: 0px;"
        "border-bottom-right-radius: 0px;"
        "padding: 5px;"
        "font-size: 14px;"
        );

    HoverButton *searchButton = new HoverButton("search", searchContainer);
    searchButton->setFixedSize(33, 33);
    searchButton->setIconSize(QSize(20, 20));
    searchButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: 1px solid #555555;"
        "   border-left: none;"
        "   border-top-left-radius: 0px;"
        "   border-bottom-left-radius: 0px;"
        "   border-top-right-radius: 5px;"
        "   border-bottom-right-radius: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );

    searchContainerLayout->addWidget(searchContactEdit);
    searchContainerLayout->addWidget(searchButton);

    contactsContentLayout->addWidget(searchContainer);
    contactsContentLayout->addWidget(createContactsList(contactsContent));

    searchLayout->addWidget(contactsHeader);
    searchLayout->addWidget(contactsContent);
    searchLayout->setStretchFactor(contactsContent, 1);

    connect(requestsButton, &QPushButton::clicked, this, [this]() {
        requestsButtonClicked();
    });


    connect(addContactButton, &QPushButton::clicked, this, [this]() {
        addContactButtonClicked();
    });


    connect(searchButton, &QPushButton::clicked, this, [this]() {
        searchContactButtonClicked();
    });



    // ----------------------------------------------------- ЧАТЫ -----------------------------------------------------
    QWidget *chatWidget = new QWidget();
    QVBoxLayout *chatLayout = new QVBoxLayout(chatWidget);
    chatLayout->setContentsMargins(0, 0, 0, 0);
    chatLayout->setSpacing(0);

    QWidget *chatHeader = new QWidget(chatWidget);
    chatHeader->setStyleSheet(
        "background-color: #1b191d;"
        "color: white;"
        "border: 2px solid #3A393E;"
        "border-top-left-radius: 5px;"
        "border-top-right-radius: 5px;"
        );
    chatHeader->setFixedHeight(50);

    QHBoxLayout *chatHeaderLayout = new QHBoxLayout(chatHeader);
    chatHeaderLayout->setContentsMargins(10, 0, 10, 0);
    chatHeaderLayout->setSpacing(10);

    QLabel *chatLabel = new QLabel("Чаты", chatHeader);
    chatLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    chatLabel->setStyleSheet(
        "font-size: 18px;"
        "font-weight: bold;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );

    QPushButton *createChatButton = new QPushButton("+", chatHeader);
    createChatButton->setFixedSize(30, 30);
    createChatButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 30px;"
        "   font-weight: bold;"
        "   padding-bottom: 6px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );

    chatHeaderLayout->addWidget(chatLabel);
    chatHeaderLayout->addStretch();
    chatHeaderLayout->addWidget(createChatButton);

    QWidget *chatContent = new QWidget(chatWidget);
    chatContent->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    QVBoxLayout *chatContentLayout = new QVBoxLayout(chatContent);
    chatContentLayout->setContentsMargins(10, 10, 10, 10);
    chatContentLayout->setSpacing(10);
    chatContentLayout->addWidget(createChatsList(chatContent));

    chatLayout->addWidget(chatHeader);
    chatLayout->addWidget(chatContent);
    chatLayout->setStretchFactor(chatContent, 1);

    connect(createChatButton, &QPushButton::clicked, this, [this]() {
        createChat();
    });

// ----------------------------------------------------- НАСТРОЙКИ -----------------------------------------------------
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

    QWidget *mainContainer = new QWidget(settingsWidget);
    mainContainer->setStyleSheet(
        "background-color: #1b191d;"
        "border: 2px solid #3A393E;"
        "border-top: none;"
        "border-bottom-left-radius: 5px;"
        "border-bottom-right-radius: 5px;"
        );

    QVBoxLayout *containerLayout = new QVBoxLayout(mainContainer);
    containerLayout->setContentsMargins(10, 10, 10, 10);
    containerLayout->setSpacing(10);

    QVBoxLayout *buttonsLayout = new QVBoxLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(10);

    QPushButton *editProfileButton = new QPushButton("Редактировать профиль", mainContainer);
    clientFunctions::setupButtonStyle(editProfileButton);

    QPushButton *deleteAccountButton = new QPushButton("Удалить аккаунт", mainContainer);
    clientFunctions::setupButtonStyle(deleteAccountButton);

    buttonsLayout->addWidget(editProfileButton, 0, Qt::AlignHCenter);
    buttonsLayout->addWidget(deleteAccountButton, 0, Qt::AlignHCenter);

    QPushButton *exitButton = new QPushButton("Выйти из аккаунта", mainContainer);
    clientFunctions::setupButtonStyle(exitButton);

    containerLayout->addLayout(buttonsLayout);
    containerLayout->addStretch(1);
    containerLayout->addWidget(exitButton, 0, Qt::AlignHCenter);

    settingsLayout->addWidget(settingsLabel);
    settingsLayout->addWidget(mainContainer);
    settingsLayout->setStretchFactor(mainContainer, 1);

    connect(editProfileButton, &QPushButton::clicked, this, [this]() {
        editProfile();
    });

    connect(deleteAccountButton, &QPushButton::clicked, this, [this]() {
        if (clientFunctions::confirmWindow("Вы уверены, что хотите удалить аккаунт?")){
            deleteAccount();
        }
    });

    connect(exitButton, &QPushButton::clicked, this, [this]() {
        if (clientFunctions::confirmWindow("Вы уверены, что хотите выйти?")) {
            exitAccount();
        }
    });

// ----------------------------------------------------- ДРУГОЕ -----------------------------------------------------
    contentStack->addWidget(searchWidget);
    contentStack->addWidget(chatWidget);
    contentStack->addWidget(settingsWidget);

    contentStack->setCurrentIndex(1);

    QHBoxLayout *navLayout = new QHBoxLayout();
    navLayout->setContentsMargins(0, 0, 0, 0);
    navLayout->setSpacing(0);

    contactsButton = new HoverButton("contacts");
    contactsButton->setIconSize(QSize(60, 60));
    contactsButton->setFixedSize(60, 60);

    HoverButton *chatButton = new HoverButton("chat");
    chatButton->setIconSize(QSize(60, 60));
    chatButton->setFixedSize(60, 60);

    HoverButton *settingsButton = new HoverButton("settings");
    settingsButton->setIconSize(QSize(60, 60));
    settingsButton->setFixedSize(60, 60);

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

// ФУНКЦИИ ДЛЯ КОННЕКТА С КЛИЕНТСКОЙ ЧАСТЬЮ

void ChatClient::onRegisterButtonClicked()
{
    if (loginEditR->text().isEmpty() || passwordEditR->text().isEmpty() || nameEditR->text().isEmpty()) {
        loginEditR->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        passwordEditR->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        nameEditR->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #dd2d4a;"
            "border-radius: 5px;"
            "padding: 5px;"
            "font-size: 14px;"
            );
        showError("Введите имя, логин и пароль");
        return;
    }
    // todo: register
    bool isExists = false;
    if (loginEditR->text() == "существует"){
        isExists = true;
    }

    if (isExists){
        showError("Логин уже занят");
        return;
    }
    clientGUI();
    yourLogin = loginEditR->text();
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
    bool isExist = true;
    if (loginEdit->text() == "несуществующий"){
        isExist = false;
    }

    if (!isExist) {
        showError("Аккаунта не существует");
        return;
    }

    if (loginEdit->text() == "существующий" && passwordEdit->text() != "12345"){
        showError("Неверный пароль");
        return;
    }


    clientGUI();
    yourLogin = loginEdit->text();
}

// ----------------------------------------------------- КОНТАКТЫ -----------------------------------------------------

void ChatClient::requestsButtonClicked()
{
    QDialog *requestsDialog = new QDialog(this);
    requestsDialog->setWindowTitle("Запросы на добавление");
    requestsDialog->setMinimumSize(400, 300);
    requestsDialog->setStyleSheet(
        "background-color: #1B191D;"
        "color: white;"
        "border: 2px solid #3A393E;"
        "border-radius: 5px;"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(requestsDialog);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // todo: get requests
    QStringList requests = {
        "Пользователь1",
        "Пользователь2",
        "Пользователь3"
    };

    QScrollArea *scrollArea = new QScrollArea(requestsDialog);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: #1B191D; border: none;");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(5);

    for (const QString &request : requests) {
        QWidget *requestWidget = new QWidget();
        requestWidget->setStyleSheet("background-color: #141315; border-radius: 5px;");

        QHBoxLayout *requestLayout = new QHBoxLayout(requestWidget);
        requestLayout->setContentsMargins(10, 5, 10, 5);

        QLabel *nameLabel = new QLabel(request);
        nameLabel->setStyleSheet("color: white; font-size: 14px;");
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QPushButton *rejectButton = new QPushButton("✖");
        rejectButton->setFixedSize(30, 30);
        rejectButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #dd2d4a;"
            "   border: none;"
            "   border-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #b43f52;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #a4152b;"
            "}"
            );

        QPushButton *acceptButton = new QPushButton("✔");
        acceptButton->setFixedSize(30, 30);
        acceptButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #512799;"
            "   border: none;"
            "   border-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #3c2760;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #463366;"
            "}"
            );

        connect(rejectButton, &QPushButton::clicked, this, [=]() {
            handleRequest(request, false);
            scrollLayout->removeWidget(requestWidget);
            requestWidget->deleteLater();
        });

        connect(acceptButton, &QPushButton::clicked, this, [=]() {
            handleRequest(request, true);
            scrollLayout->removeWidget(requestWidget);
            requestWidget->deleteLater();
        });

        requestLayout->addWidget(nameLabel);
        requestLayout->addWidget(rejectButton);
        requestLayout->addWidget(acceptButton);

        scrollLayout->addWidget(requestWidget);
    }

    scrollLayout->addStretch();
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    requestsDialog->exec();
}

void ChatClient::handleRequest(const QString &username, bool accepted)
{
    // todo: handle request
}

void ChatClient::addContactButtonClicked()
{
    QDialog *addContactDialog = new QDialog(this);
    addContactDialog->setWindowTitle("Добавить контакт");
    addContactDialog->setFixedSize(300, 80);
    addContactDialog->setModal(true);
    addContactDialog->setStyleSheet(
        "background-color: #1B191D;"
        "color: white;"
        "border: 2px solid #3A393E;"
        "border-radius: 5px;"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(addContactDialog);
    mainLayout->setContentsMargins(10, 10, 10, 5);
    mainLayout->setSpacing(5);

    QHBoxLayout *inputLayout = new QHBoxLayout();
    inputLayout->setContentsMargins(0, 0, 0, 0);
    inputLayout->setSpacing(5);

    QLineEdit *nameInput = new QLineEdit();
    nameInput->setPlaceholderText("Введите имя...");
    nameInput->setStyleSheet(
        "background-color: #141315;"
        "color: #FFFFFF;"
        "border: 1px solid #555555;"
        "border-radius: 5px;"
        "padding: 5px;"
        "font-size: 14px;"
        );

    QPushButton *sendButton = new QPushButton("⇨");
    sendButton->setFixedSize(30, 30);
    sendButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 20px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #2a1a4a;"
        "}"
        );

    inputLayout->addWidget(nameInput);
    inputLayout->addWidget(sendButton);

    QLabel *statusLabel = new QLabel();
    statusLabel->setStyleSheet(
        "color: #555555;"
        "font-size: 12px;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setFixedHeight(20);

    mainLayout->addLayout(inputLayout);
    mainLayout->addWidget(statusLabel);

    sendButton->setDisabled(true);
    connect(nameInput, &QLineEdit::textChanged, [sendButton](const QString &text) {
        sendButton->setDisabled(text.trimmed().isEmpty());
    });

    connect(sendButton, &QPushButton::clicked, this, [this, nameInput, statusLabel]() {
        QString contactName = nameInput->text().trimmed();
        if (!contactName.isEmpty()) {
            bool isFound = false;
            // todo: add contact
            if (contactName == "существует"){
                isFound = true;
            }

            if (isFound) {
                statusLabel->setText("Запрос отправлен!");
            } else {
                statusLabel->setText("Пользователь не найден.");
            }
        }
    });

    connect(nameInput, &QLineEdit::returnPressed, this, [sendButton]() {
        if (!sendButton->isEnabled()) return;
        sendButton->click();
    });

    addContactDialog->exec();
}

void ChatClient::searchContactButtonClicked(){
    // todo: show searched contacts
    // searchContactEdit->text()
}

QWidget* ChatClient::createContactsList(QWidget* parent)
{
    QScrollArea *scrollArea = new QScrollArea(parent);
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: #1B191D; border: none;");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(5);

    // todo: get contacts
    QStringList contacts = {
        "Друг 1",
        "Друг 2",
        "Друг 3",
        "Друг 4",
        "Друг 5",
        "Друг 6",
        "Друг 7",
        "Друг 228"
    };

    for (const QString &contact : contacts) {
        QWidget *contactWidget = new QWidget();
        contactWidget->setStyleSheet("background-color: #141315; border-radius: 5px;");

        QHBoxLayout *contactLayout = new QHBoxLayout(contactWidget);
        contactLayout->setContentsMargins(10, 5, 10, 5);

        QLabel *nameLabel = new QLabel(contact);
        nameLabel->setStyleSheet("color: white; font-size: 14px;");
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        QPushButton *addContactToChatButton = new QPushButton("✚");
        addContactToChatButton->setFixedSize(30, 30);
        addContactToChatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #3A393E;"
            "   border: none;"
            "   border-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #313134;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #221E30;"
            "}"
            );

        connect(addContactToChatButton, &QPushButton::clicked, this, [this, contact, contactWidget]() {
            QDialog *addChatDialog = new QDialog(this);
            addChatDialog->setWindowTitle("Создать новый чат");
            addChatDialog->setFixedSize(300, 120);
            addChatDialog->setModal(true);
            addChatDialog->setStyleSheet(
                "background-color: #1B191D;"
                "color: white;"
                "border: 2px solid #3A393E;"
                "border-radius: 5px;"
                );

            QVBoxLayout *mainLayout = new QVBoxLayout(addChatDialog);
            mainLayout->setContentsMargins(10, 10, 10, 5);
            mainLayout->setSpacing(5);

            QHBoxLayout *inputLayout = new QHBoxLayout();
            inputLayout->setContentsMargins(0, 0, 0, 0);
            inputLayout->setSpacing(5);

            QLineEdit *chatNameInput = new QLineEdit();
            chatNameInput->setPlaceholderText("Введите название чата...");
            chatNameInput->setStyleSheet(
                "background-color: #141315;"
                "color: #FFFFFF;"
                "border: 1px solid #555555;"
                "border-radius: 5px;"
                "padding: 5px;"
                "font-size: 14px;"
                );

            QPushButton *createButton = new QPushButton("⇨");
            createButton->setFixedSize(30, 30);
            createButton->setStyleSheet(
                "QPushButton {"
                "   background-color: #512799;"
                "   border: none;"
                "   border-radius: 5px;"
                "   color: white;"
                "   font-size: 20px;"
                "   font-weight: bold;"
                "}"
                "QPushButton:hover {"
                "   background-color: #3c2760;"
                "}"
                "QPushButton:pressed {"
                "   background-color: #463366;"
                "}"
                );

            inputLayout->addWidget(chatNameInput);
            inputLayout->addWidget(createButton);

            QLabel *statusLabel = new QLabel();
            statusLabel->setStyleSheet(
                "color: #555555;"
                "font-size: 12px;"
                "background: transparent;"
                "border: none;"
                "padding: 0;"
                "margin: 0;"
                );
            statusLabel->setAlignment(Qt::AlignCenter);
            statusLabel->setFixedHeight(20);

            mainLayout->addLayout(inputLayout);
            mainLayout->addWidget(statusLabel);

            createButton->setDisabled(true);
            connect(chatNameInput, &QLineEdit::textChanged, [createButton](const QString &text) {
                createButton->setDisabled(text.trimmed().isEmpty());
            });

            connect(createButton, &QPushButton::clicked, this, [this, chatNameInput, statusLabel, addChatDialog]() {
                QString chatName = chatNameInput->text().trimmed();
                if (!chatName.isEmpty()) {
                    bool chatExists = false;
                    // todo: check if chat exists
                    if (chatName == "существует"){
                        chatExists == true;
                    }

                    if (!chatExists) {
                        statusLabel->setText("Чат не найден");
                    } else {
                        // todo: add contact to chat
                        statusLabel->setText("Пользователь добавлен в чат!");
                        addChatDialog->accept();
                    }
                } else {
                    statusLabel->setText("Название чата...");
                }
            });

            connect(chatNameInput, &QLineEdit::returnPressed, createButton, &QPushButton::click);

            addChatDialog->exec();
        });


        QPushButton *deleteButton = new QPushButton("✖");
        deleteButton->setFixedSize(30, 30);
        deleteButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #3A393E;"
            "   border: none;"
            "   border-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #313134;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #221E30;"
            "}"
            );

        connect(deleteButton, &QPushButton::clicked, this, [this, contact, contactWidget]() {
            if (clientFunctions::confirmWindow("Удалить контакт " + contact + "?")) {
                // todo: delete contact
                contactWidget->deleteLater();
            }
        });

        contactLayout->addWidget(nameLabel);
        contactLayout->addWidget(addContactToChatButton);
        contactLayout->addWidget(deleteButton);

        scrollLayout->addWidget(contactWidget);
    }

    scrollLayout->addStretch();
    scrollArea->setWidget(scrollContent);

    return scrollArea;
}

// ----------------------------------------------------- ЧАТЫ -----------------------------------------------------
void ChatClient::createChat()
{
    QDialog *createChatDialog = new QDialog(this);
    createChatDialog->setWindowTitle("Создать чат");
    createChatDialog->setFixedSize(350, 200);
    createChatDialog->setStyleSheet(
        "background-color: #1B191D;"
        "color: white;"
        "border: 2px solid #3A393E;"
        "border-radius: 5px;"
        );

    QVBoxLayout *mainLayout = new QVBoxLayout(createChatDialog);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(15);

    QHBoxLayout *typeLayout = new QHBoxLayout();
    typeLayout->setContentsMargins(0, 0, 0, 0);
    typeLayout->setSpacing(10);

    QLabel *typeLabel = new QLabel("Тип чата:");
    typeLabel->setStyleSheet(
        "color: white;"
        "font-size: 14px;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );

    QButtonGroup *chatTypeGroup = new QButtonGroup(createChatDialog);
    QRadioButton *textChatButton = new QRadioButton("Текстовый");
    QRadioButton *voiceChatButton = new QRadioButton("Голосовой");

    textChatButton->setChecked(true);

    textChatButton->setStyleSheet(
        "QRadioButton {"
        "   color: white;"
        "   font-size: 14px;"
        "   spacing: 5px;"
        "border: 1px solid #1B191D;"
        "}"
        "QRadioButton::indicator {"
        "   width: 16px;"
        "   height: 16px;"
        "border: 1px solid #1B191D;"
        "}"
        "QRadioButton::indicator::unchecked {"
        "   border: 1px solid #555555;"
        "   border-radius: 8px;"
        "   background-color: #141315;"
        "border: 1px solid #1B191D;"
        "}"
        "QRadioButton::indicator::checked {"
        "   border: 1px solid #512799;"
        "   border-radius: 8px;"
        "   background-color: #512799;"
        "border: 1px solid #1B191D;"
        "}"
        );

    voiceChatButton->setStyleSheet(textChatButton->styleSheet());

    chatTypeGroup->addButton(textChatButton);
    chatTypeGroup->addButton(voiceChatButton);

    typeLayout->addWidget(typeLabel);
    typeLayout->addWidget(textChatButton);
    typeLayout->addWidget(voiceChatButton);
    typeLayout->addStretch();

    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Название чата...");
    nameEdit->setStyleSheet(
        "background-color: #141315;"
        "color: #FFFFFF;"
        "border: 1px solid #555555;"
        "border-radius: 5px;"
        "padding: 8px;"
        "font-size: 14px;"
        );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->addStretch();

    QPushButton *createButton = new QPushButton("Создать");
    createButton->setFixedSize(100, 30);
    createButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        "QPushButton:disabled {"
        "   background-color: #2a1a4a;"
        "}"
        );

    buttonLayout->addWidget(createButton);

    mainLayout->addLayout(typeLayout);
    mainLayout->addWidget(nameEdit);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    createButton->setDisabled(true);
    connect(nameEdit, &QLineEdit::textChanged, [createButton](const QString &text) {
        createButton->setDisabled(text.trimmed().isEmpty());
    });

    connect(createButton, &QPushButton::clicked, this, [=]() {
        QString chatName = nameEdit->text().trimmed();
        bool isVoiceChat = voiceChatButton->isChecked();

        // todo: create chat

        createChatDialog->close();
    });

    connect(nameEdit, &QLineEdit::returnPressed, this, [=]() {
        if (!createButton->isEnabled()) return;
        createButton->click();
    });

    createChatDialog->exec();
}

QWidget* ChatClient::createChatsList(QWidget* parent)
{
    m_mainContainer = new QWidget(parent);
    QVBoxLayout* mainLayout = new QVBoxLayout(m_mainContainer);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* chatContainer = new QWidget();
    chatContainer->setStyleSheet("background-color: #1B191D; border: none;");
    QVBoxLayout* chatContainerLayout = new QVBoxLayout(chatContainer);
    chatContainerLayout->setContentsMargins(0, 0, 0, 0);
    chatContainerLayout->setSpacing(0);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setStyleSheet("background-color: #1B191D; border: none;");

    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);
    scrollLayout->setContentsMargins(0, 0, 0, 0);
    scrollLayout->setSpacing(5);

    QVector<QPair<QString, bool>> chats = {
        {"Текстовый чат", false},
        {"Голосовой чат 1", true},
        {"Чат", false},
        {"Тестовый чат", true},
        {"Тест", true}
    };

    m_chatStates.clear();
    for (const auto &chat : chats) {
        m_chatStates[chat.first] = false;
    }

    for (const auto &chat : chats) {
        QString chatName = chat.first;
        bool isVoiceChat = chat.second;

        QPushButton *chatButton = new QPushButton();
        chatButton->setFixedHeight(40);
        chatButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #141315;"
            "   border: none;"
            "   border-radius: 5px;"
            "   text-align: left;"
            "   padding: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #1E1B20;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #221E30;"
            "}"
            );

        QHBoxLayout *buttonLayout = new QHBoxLayout(chatButton);
        buttonLayout->setContentsMargins(5, 5, 5, 5);
        buttonLayout->setSpacing(10);

        QLabel *iconLabel = new QLabel();
        iconLabel->setFixedSize(30, 30);

        QString iconPath = isVoiceChat ? ":/images/voice_chat_icon.png" : ":/images/text_chat_icon.png";
        QPixmap chatIcon(iconPath);

        qDebug() << "Initial icon path:" << iconPath;

        if (!chatIcon.isNull()) {
            QPixmap scaledIcon = chatIcon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            iconLabel->setPixmap(scaledIcon);
        } else {
            iconLabel->clear();
        }

        iconLabel->setScaledContents(false);

        QLabel *nameLabel = new QLabel(chatName);
        nameLabel->setStyleSheet(
            "color: white;"
            "font-size: 14px;"
            "background: transparent;"
            );
        nameLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

        buttonLayout->addWidget(iconLabel);
        buttonLayout->addWidget(nameLabel);
        buttonLayout->addStretch();

        connect(chatButton, &QPushButton::clicked, this, [this, chatName, iconLabel, isVoiceChat]() {
            m_chatStates[chatName] = !m_chatStates[chatName];

            QString iconPath = m_chatStates[chatName]
                                   ? (isVoiceChat ? ":/images/voice_chat_ON.png" : ":/images/text_chat_ON.png")
                                   : (isVoiceChat ? ":/images/voice_chat_icon.png" : ":/images/text_chat_icon.png");

            QPixmap chatIcon(iconPath);
            if (!chatIcon.isNull()) {
                QPixmap scaledIcon = chatIcon.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
                iconLabel->setPixmap(scaledIcon);
            } else {
                iconLabel->clear();
            }

            openChat(chatName, isVoiceChat);
        });

        QPushButton *deleteButton = new QPushButton("✖");
        deleteButton->setFixedSize(30, 30);
        deleteButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #3A393E;"
            "   border: none;"
            "   border-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "   padding-left: 7px;"
            "   padding-bottom: 5px;"
            "}"
            "QPushButton:hover {"
            "   background-color: #313134;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #221E30;"
            "}"
            );

        connect(deleteButton, &QPushButton::clicked, this, [this, chatName, chatButton]() {
            if (clientFunctions::confirmWindow("Покинуть чат " + chatName + "?")) {
                m_chatStates.remove(chatName);
                chatButton->deleteLater();
            }
        });

        buttonLayout->addWidget(deleteButton);
        scrollLayout->addWidget(chatButton);
    }


    scrollLayout->addStretch();
    scrollArea->setWidget(scrollContent);

    m_chatHistoryWidget = new QWidget();
    m_chatHistoryWidget->setStyleSheet(
        "background-color: #1B191D;"
        "border: 2px solid #555555;"
        "border-radius: 5px;"
        "margin: 5px;"
        );
    m_chatHistoryWidget->setVisible(false);

    m_chatHistoryLayout = new QVBoxLayout(m_chatHistoryWidget);
    m_chatHistoryLayout->setContentsMargins(0, 0, 0, 0);

    m_messageHistoryArea = new QScrollArea();
    m_messageHistoryArea->setWidgetResizable(true);
    m_messageHistoryArea->setStyleSheet("background-color: #1B191D; border: none;");

    QWidget* messageHistoryContent = new QWidget();
    QVBoxLayout* messageHistoryLayout = new QVBoxLayout(messageHistoryContent);
    messageHistoryLayout->setContentsMargins(0, 0, 0, 0);
    messageHistoryLayout->setSpacing(0);

    addSampleMessage(messageHistoryLayout, "00:00", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User2", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");
    addSampleMessage(messageHistoryLayout, "00:01", "User1", "Привет");

    messageHistoryLayout->addStretch();
    m_messageHistoryArea->setWidget(messageHistoryContent);
    m_chatHistoryLayout->addWidget(m_messageHistoryArea);

    mainLayout->addWidget(scrollArea);
    mainLayout->addWidget(m_chatHistoryWidget);

    return m_mainContainer;
}

void ChatClient::addSampleMessage(QVBoxLayout* layout, const QString& time, const QString& user, const QString& message)
{
    QWidget* messageWidget = new QWidget();
    messageWidget->setStyleSheet("background-color: #141315; border-radius: 5px;");

    QVBoxLayout* messageLayout = new QVBoxLayout(messageWidget);
    messageLayout->setContentsMargins(10, 5, 10, 5);

    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(10);

    QLabel* timeLabel = new QLabel(time);
    timeLabel->setStyleSheet("color: #7A7A7A; font-size: 10px;");

    QLabel* userLabel = new QLabel(user);
    userLabel->setStyleSheet("color: #E1E1E1; font-size: 12px; font-weight: bold;");

    headerLayout->addWidget(userLabel);
    headerLayout->addWidget(timeLabel);
    headerLayout->addStretch();

    QLabel* messageLabel = new QLabel(message);
    messageLabel->setStyleSheet("color: white; font-size: 12px;");
    messageLabel->setWordWrap(true);

    messageLayout->addLayout(headerLayout);
    messageLayout->addWidget(messageLabel);

    layout->addWidget(messageWidget);
}

void ChatClient::openChat(const QString &chatName, bool isVoiceChat)
{
    qDebug() << "Opening chat:" << chatName;

    if (!isVoiceChat) {
        if (m_chatHistoryWidget->isVisible() && m_currentChatName == chatName) {
            m_chatHistoryWidget->setVisible(false);
            m_mainContainer->findChild<QScrollArea*>()->setMaximumHeight(QWIDGETSIZE_MAX);
            m_currentChatName.clear();
            return;
        }

        m_chatHistoryWidget->setVisible(true);
        m_chatHistoryWidget->setStyleSheet(
            "background-color: #1B191D;"
            "border: 2px solid #555555;"
            "border-radius: 5px;"
            "margin: 5px;"
            );

        m_mainContainer->findChild<QScrollArea*>()->setMaximumHeight(200);

        m_currentChatName = chatName;

        if (m_messageInputContainer) {
            m_messageInputContainer->deleteLater();
        }

        m_messageInputContainer = new QWidget(m_chatHistoryWidget);
        m_messageInputContainer->setStyleSheet(
            "background-color: #1B191D;"
            "border: none;"
            "margin: 10px;"
            );

        QVBoxLayout* historyLayout = qobject_cast<QVBoxLayout*>(m_chatHistoryWidget->layout());
        historyLayout->addWidget(m_messageInputContainer);

        QHBoxLayout *inputLayout = new QHBoxLayout(m_messageInputContainer);
        inputLayout->setContentsMargins(0, 0, 0, 0);
        inputLayout->setSpacing(0);

        QLineEdit *messageEdit = new QLineEdit();
        messageEdit->setFixedSize(290, 55);
        messageEdit->setPlaceholderText("Сообщение...");
        messageEdit->setStyleSheet(
            "background-color: #141315;"
            "color: #FFFFFF;"
            "border: 1px solid #555555;"
            "border-right: none;"
            "border-top-left-radius: 5px;"
            "border-bottom-left-radius: 5px;"
            "border-top-right-radius: 0px;"
            "border-bottom-right-radius: 0px;"
            "padding: 5px;"
            "font-size: 14px;"
            );

        QPushButton *sendButton = new QPushButton(">");
        sendButton->setFixedSize(55, 55);
        sendButton->setStyleSheet(
            "QPushButton {"
            "   background-color: #512799;"
            "   border: 1px solid #555555;"
            "   border-left: none;"
            "   border-top-left-radius: 0px;"
            "   border-bottom-left-radius: 0px;"
            "   border-top-right-radius: 5px;"
            "   border-bottom-right-radius: 5px;"
            "   color: white;"
            "   font-size: 16px;"
            "   font-weight: bold;"
            "}"
            "QPushButton:hover {"
            "   background-color: #3c2760;"
            "}"
            "QPushButton:pressed {"
            "   background-color: #463366;"
            "}"
            );

        inputLayout->addWidget(messageEdit);
        inputLayout->addWidget(sendButton);

        connect(sendButton, &QPushButton::clicked, this, [this, messageEdit, chatName]() {
            QString message = messageEdit->text();
            if (!message.isEmpty()) {
                qDebug() << "Sending message to" << chatName << ":" << message;
                // todo: Реализовать отправку сообщения
                messageEdit->clear();
            }
        });

        connect(messageEdit, &QLineEdit::returnPressed, sendButton, &QPushButton::click);

    } else {
        // todo: join voice chat
    }

    // todo: join text chat
}


// ----------------------------------------------------- НАСТРОЙКИ -----------------------------------------------------
void ChatClient::editProfile()
{
    QDialog *profileDialog = new QDialog(this);
    profileDialog->setWindowTitle("Редактировать профиль");
    profileDialog->setFixedSize(400, 150);
    profileDialog->setStyleSheet(
        "background-color: #1B191D;"
        "color: white;"
        "border: 2px solid #1B191D;"
        "border-radius: 5px;"
        );
    QVBoxLayout *mainLayout = new QVBoxLayout(profileDialog);
    mainLayout->setContentsMargins(15, 15, 15, 15);
    mainLayout->setSpacing(15);

    QWidget *fieldsContainer = new QWidget();
    QVBoxLayout *fieldsLayout = new QVBoxLayout(fieldsContainer);
    fieldsLayout->setContentsMargins(0, 0, 0, 0);
    fieldsLayout->setSpacing(10);

    QWidget *loginContainer = new QWidget();
    QHBoxLayout *loginLayout = new QHBoxLayout(loginContainer);
    loginLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *loginLabel = new QLabel("Логин:", loginContainer);
    loginLabel->setFixedWidth(60);
    loginLabel->setStyleSheet(
        "color: white;"
        "font-size: 14px;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );

    QLineEdit *loginEdit = new QLineEdit(loginContainer);

    loginEdit->setText(yourLogin);
    loginEdit->setReadOnly(true);
    loginEdit->setFixedHeight(30);
    loginEdit->setStyleSheet(
        "background-color: #141315;"
        "color: #AAAAAA;"
        "border: 1px solid #555555;"
        "border-radius: 5px;"
        "padding: 0 8px;"
        "font-size: 14px;"
        );

    loginLayout->addWidget(loginLabel);
    loginLayout->addWidget(loginEdit);
    fieldsLayout->addWidget(loginContainer);

    QWidget *nameContainer = new QWidget();
    QHBoxLayout *nameLayout = new QHBoxLayout(nameContainer);
    nameLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *nameLabel = new QLabel("Имя:", nameContainer);
    nameLabel->setFixedWidth(60);
    nameLabel->setStyleSheet(
        "color: white;"
        "font-size: 14px;"
        "background: transparent;"
        "border: none;"
        "padding: 0;"
        "margin: 0;"
        );

    QLineEdit *nameEdit = new QLineEdit(nameContainer);
    nameEdit->setPlaceholderText("Ваше новое имя...");
    nameEdit->setFixedHeight(30);
    nameEdit->setStyleSheet(
        "background-color: #141315;"
        "color: #FFFFFF;"
        "border: 1px solid #555555;"
        "border-radius: 5px;"
        "padding: 0 8px;"
        "font-size: 14px;"
        );

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameEdit);
    fieldsLayout->addWidget(nameContainer);

    QPushButton *saveButton = new QPushButton("Сохранить", profileDialog);
    saveButton->setFixedSize(100, 30);
    saveButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #512799;"
        "   border: none;"
        "   border-radius: 5px;"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #3c2760;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #463366;"
        "}"
        );

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);

    mainLayout->addWidget(fieldsContainer);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, this, [=]() {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            // todo: update profile
            profileDialog->close();
        }
    });

    profileDialog->exec();
}

void ChatClient::deleteAccount(){
    // todo: delete account
    clearAuthWidget();
    setupGUI();
    showAuthScreen();
}

void ChatClient::exitAccount()
{
    // todo: exit account
    clearAuthWidget();
    setupGUI();
    showAuthScreen();
}
