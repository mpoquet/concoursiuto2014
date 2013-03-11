#include <string>


enum LoginResult
{
    LOGIN_OK,
    LOGIN_NO_MORE_ROOM,
    LOGIN_INVALID,
    LOGIN_ALREADY_USED,
    LOGIN_NETWORK_ERROR
};

enum RoundState
{
    ROUND_NORMAL,
    ROUND_END_OF_GAME,
    ROUND_NETWORK_ERROR
};


struct SessionData;


class Session
{
    private:

    SessionData* _data;


    public:

    Session();
    ~Session();

    bool connect(const std::string& host, int port);
    void disconnect();

    int login(const std::string& pseudo);
    bool waitInit();
    int waitRoundStarting();

    bool isConnected();
    std::string lastError();


    private:

    typedef std::pair<char, std::string> Message;
    void sendMessage(const Message& message);
    Message waitMessage(std::string expectedType);
    void setError(const std::string& error);
};


