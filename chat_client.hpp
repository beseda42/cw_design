#ifndef CHAT_CLIENT_HPP
#define CHAT_CLIENT_HPP

#include <QMainWindow>
#include <QListWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QTabWidget>
#include <QPropertyAnimation>
#include <QTimer>
#include <QStackedWidget>
#include <QEnterEvent>
#include <QMouseEvent>
#include <QScrollArea>

class ChatClient : public QMainWindow {
    Q_OBJECT

public:
    explicit ChatClient(QWidget *parent = nullptr);
    ~ChatClient();

private slots:
    void onSendMessageButtonClicked();
    void onChatSelected(QListWidgetItem *item);
    void onCreateChatButtonClicked();
    void onInviteUserButtonClicked();
    void onGetUserListButtonClicked();
    void onDeleteFromChatButtonClicked();
    void onDeleteAccountButtonClicked();
    void onGetChatUsersButtonClicked();
    void onStartCallButtonClicked();
    void onStopCallButtonClicked();
    void onRegisterButtonClicked();
    void onLoginButtonClicked();

    void createChat();

    void requestsButtonClicked();
    void handleRequest(const QString &username, bool accepted);
    void addContactButtonClicked();
    void searchContactButtonClicked();
    void editProfile();
    void deleteAccount();
    void exitAccount();

private:
    void setupGUI();
    void registrationGUI();
    void clientGUI();

    void showAuthScreen();
    void showChatScreen();
    void appendMessage(const QString &userName, const QString &text, qint64 date);
    void showError(const QString& message);
    void hideError();
    void updateErrorPosition();
    void resizeEvent(QResizeEvent *event);
    void clearAuthWidget();
    void setupLineEditStyle(QLineEdit *lineEdit);
    void setupButtonStyle(QPushButton *button);
    void oncontactsButtonEnter();
    void oncontactsButtonLeave();
    void openChat(const QString &chatName, bool isVoiceChat);
    void addSampleMessage(QVBoxLayout* layout, const QString& time, const QString& user, const QString& message);

    QWidget *authWidget;
    QWidget *chatWidget;
    QTabWidget *mainTabs;
    QLineEdit *loginEdit;
    QLineEdit *passwordEdit;
    QLineEdit *nameEdit;
    QLineEdit *loginEditR;
    QLineEdit *passwordEditR;
    QLineEdit *nameEditR;
    QPushButton *registerButton;
    QPushButton *registrationButton;
    QPushButton *loginButton;
    QListWidget *chatListWidget;
    QTextEdit *messageDisplay;
    QLineEdit *messageInput;
    QPushButton *sendMessageButton;
    QPushButton *createChatButton;
    QLineEdit *chatNameInput;
    QPushButton *inviteUserButton;
    QComboBox *userComboBox;
    QPushButton *getUserListButton;
    QPushButton *deleteFromChatButton;
    QPushButton *deleteAccountButton;
    QPushButton *getChatUsersButton;
    QPushButton *startCallButton;
    QPushButton *stopCallButton;
    QLabel *connectionStatusLabel;
    QLabel *voipStatusLabel;
    QLabel *errorLabel;
    QWidget *errorContainer;
    QStackedWidget *contentStack;
    QPushButton *contactsButton;
    QWidget* createContactsList(QWidget* parent);
    QWidget* createChatsList(QWidget* parent);

    QString yourLogin;
    QString m_currentChatName;
    QMap<QString, bool> m_chatStates;
    QWidget* m_chatHistoryWidget = nullptr;
    QVBoxLayout* m_chatHistoryLayout = nullptr;
    QScrollArea* m_messageHistoryArea = nullptr;
    QWidget* m_mainContainer = nullptr;
    QWidget* m_messageInputContainer = nullptr;

};

class HoverButton : public QPushButton {
    Q_OBJECT

public:
    explicit HoverButton(const QString& iconPrefix, QWidget *parent = nullptr)
        : QPushButton(parent), m_iconPrefix(iconPrefix)
    {
        setIcon(QIcon(QString(":/images/%1_icon.png").arg(iconPrefix)));
    }

protected:
    void enterEvent(QEnterEvent *event) override {
        setIcon(QIcon(QString(":/images/%1_icon_hovered.png").arg(m_iconPrefix)));
        QPushButton::enterEvent(event);
    }

    void leaveEvent(QEvent *event) override {
        setIcon(QIcon(QString(":/images/%1_icon.png").arg(m_iconPrefix)));
        QPushButton::leaveEvent(event);
    }

    void mousePressEvent(QMouseEvent *event) override {
        setIcon(QIcon(QString(":/images/%1_icon_pressed.png").arg(m_iconPrefix)));
        QPushButton::mousePressEvent(event);
    }

    void mouseReleaseEvent(QMouseEvent *event) override {
        setIcon(QIcon(QString(":/images/%1_icon_hovered.png").arg(m_iconPrefix)));
        QPushButton::mouseReleaseEvent(event);
    }

private:
    QString m_iconPrefix;
};

#endif // CHAT_CLIENT_HPP
