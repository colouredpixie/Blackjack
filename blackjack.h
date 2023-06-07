#ifndef BLACKJACK_H
#define BLACKJACK_H

#include <QMainWindow>

#include <QPixmap>
#include <QLabel>
#include <QPropertyAnimation>

#include <array>


QT_BEGIN_NAMESPACE
namespace Ui { class Blackjack; }
QT_END_NAMESPACE


class Card {
public:
    enum Suits {
        HEARTS,
        DIAMONDS,
        CLUBS,
        SPADES,
        MAX_SUITS
    };

    enum Ranks {
        ACE,
        TWO,
        THREE,
        FOUR,
        FIVE,
        SIX,
        SEVEN,
        EIGHT,
        NINE,
        TEN,
        JACK,
        DAME,
        KING,
        MAX_RANKS
    };


private:
    Suits m_suit;
    Ranks m_rank;

public:
    Card(Ranks rank = Ranks::MAX_RANKS, Suits suit = Suits::MAX_SUITS) : m_rank(rank), m_suit(suit) {}

    void openCard(QLabel *label) const  {
        char address[51] {"F:/programs/Qt/projects/Blackjack/resources/00.png"};
        switch (m_rank) {
        case ACE: address[44] = 'A'; break;
        case TWO: address[44] = '2'; break;
        case THREE: address[44] = '3'; break;
        case FOUR: address[44] = '4'; break;
        case FIVE: address[44] = '5'; break;
        case SIX: address[44] = '6'; break;
        case SEVEN: address[44] = '7'; break;
        case EIGHT: address[44] = '8'; break;
        case NINE: address[44] = '9'; break;
        case TEN: address[44] = '1'; break;
        case JACK: address[44] = 'J'; break;
        case DAME: address[44] = 'Q'; break;
        case KING: address[44] = 'K'; break;
        }

        switch (m_suit) {
        case HEARTS:  address[45] = 'H'; break;
        case DIAMONDS:  address[45] = 'D'; break;
        case CLUBS:  address[45] = 'C'; break;
        case SPADES: address[45] = 'S'; break;
        }

        QPixmap pixmap(address);
        label->setPixmap(pixmap);

        //animation
        QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry");
        animation->setDuration(150);
        animation->setStartValue(QRect(660, 200, label->width(), label->height())); //hardcoded x, y
        animation->setEndValue(label->geometry());

        animation->start();
    }

    int getCardValue() const {
        switch (m_rank) {
        case ACE:
            return 11;
        case JACK:
            return 10;
        case DAME:
            return 10;
        case KING:
            return 10;
        default:
            return (static_cast<int>(m_rank) + 1);
        }
    }

    Ranks getRank() const {
        return m_rank;
    }

};

class Deck {
private:
    std::array<Card, 52> m_deck;
    int m_cardIndex;

public:
    Deck() {
        m_cardIndex = 0;
        int cardIndex = 0;
        for (int rankIndex = 0; rankIndex < Card::MAX_RANKS; rankIndex++) {
            for (int suitIndex = 0; suitIndex < Card::MAX_SUITS; suitIndex++) {
                m_deck[cardIndex] = Card(static_cast<Card::Ranks>(rankIndex), static_cast<Card::Suits>(suitIndex));
                cardIndex++;
            }
        }
    }

    static int getRandomNumber(int min, int max)
    {
        static const double fraction = 1.0 / (static_cast<double>(RAND_MAX) + 1.0);
        return static_cast<int>(rand() * fraction * (max - min + 1) + min);
    }

    void swapCard(Card& card1, Card& card2) {
        Card tempCard = card1;
        card1 = card2;
        card2 = tempCard;
    }

    void shuffleDeck(){
        for (auto& card : m_deck) {
            swapCard(card, m_deck[(rand() % 52)]);
        }
        m_cardIndex = 0;
    }

    const Card& dealCard() {
        return m_deck[m_cardIndex++];
    }

    const Card& getCurrentCard() {
        return m_deck[m_cardIndex];
    }

    int getIndex() {
        return m_cardIndex;
    }
};

class Blackjack : public QMainWindow
{
    Q_OBJECT

private:
    std::array<Card, 52> m_deck;
    int m_cardIndex;

public:
    Deck deck;
    int currentCardIndex;

    int dealerPoints;
    int playerPoints;
    int playerBank;
    int playerBid;


    bool dealerStand;
    bool playerStand;

    int aceValue;

    QString reverseAddress{"F:/programs/Qt/projects/Blackjack/resources/classic.png"};
    QLabel* starterCards;

    bool aceValueChanged;
    std::vector<Card> dealerHand;
    std::vector<Card> playerHand;


    Blackjack(QWidget *parent = nullptr);
    ~Blackjack();

    void setCardImage(QLabel *cardLabel);
    void coverCard(QLabel *label);

    void makeSound(const QString source);

    void resetTable();
    void deal(Deck& deck, int& points, QLabel *pointsLabel, QLabel *cardLabel, std::vector<Card>& hand);
    void endgame();

    void setAceValue(const std::vector<Card> hand, int& points, QLabel *pointsLabel);


private slots:
    void on_startButton_clicked();
    void on_hitButton_clicked();
    void on_standButton_clicked();

    void on_actionClassic_triggered();
    void on_actionFantasy_triggered();
    void on_actionAddReverseImage_triggered();

    void on_actionNewBet_triggered();

private:
    Ui::Blackjack *ui;
};


#endif // BLACKJACK_H
