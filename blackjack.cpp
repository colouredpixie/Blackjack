#include "blackjack.h"
#include "./ui_blackjack.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QInputDialog>
#include <QFileDialog>
#include <QApplication>

#include <QSoundEffect>
#include <QMediaPlayer>

#include <array>
#include <ctime>
#include <cstdlib>


Blackjack::Blackjack(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Blackjack)
{
    ui->setupUi(this);

    resetTable();
    ui->hitButton->setEnabled(false);
    ui->standButton->setEnabled(false);
    ui->dealerHeaderPoints->setVisible(false);

    playerBank = 100;
    playerBid = 5;

    //background music
    QSoundEffect* player = new QSoundEffect();
    player->setSource(QUrl::fromLocalFile("F:/programs/Qt/projects/Blackjack/resources/Wilderness.wav"));
    player->setVolume(0.5);
    player->setLoopCount(QSoundEffect::Infinite);
    player->play();
}

Blackjack::~Blackjack()
{
    delete ui;
}


//button triggers
void Blackjack::on_startButton_clicked()
{
    //preparing the table
    resetTable();
    ui->hitButton->setEnabled(true);
    ui->standButton->setEnabled(true);
    ui->playerBankPoints->setText(QString::number(playerBank));
    on_actionNewBet_triggered();

    aceValueChanged = false;
    dealerHand.clear();
    playerHand.clear();

    deck.shuffleDeck();

    //first deal - two cards each
    deal(deck, dealerPoints, this->ui->dealerHeaderPoints, this->ui->dealerCard_1, dealerHand);
    deal(deck, dealerPoints, this->ui->dealerHeaderPoints, this->ui->dealerCard_2, dealerHand);
    coverCard(ui->dealerCardHidden);

    deal(deck, playerPoints, this->ui->playerHeaderPoints, this->ui->playerCard_1, playerHand);
    deal(deck, playerPoints, this->ui->playerHeaderPoints, this->ui->playerCard_2, playerHand);
}

void Blackjack::on_hitButton_clicked()
{
    makeSound("F:/programs/Qt/projects/Blackjack/resources/click.wav");

    //setting up card slot in hand. currentCardIndex iterator is for counting the correct spot
    QLabel* playerCard = this->ui->playerCard_3;
    QLabel* dealerCard = this->ui->dealerCard_3;

    if (currentCardIndex == 4) {
        playerCard = this->ui->playerCard_4;
        dealerCard = this->ui->dealerCard_4;
    } else if (currentCardIndex == 5) {
        playerCard = this->ui->playerCard_5;
        dealerCard = this->ui->dealerCard_5;
    } else if (currentCardIndex == 6) {
        playerCard = this->ui->playerCard_6;
        dealerCard = this->ui->dealerCard_6;
    } else if (currentCardIndex >= 7) {
        playerCard = this->ui->playerCard_7;
        dealerCard = this->ui->dealerCard_7;
    }

    deal(deck, playerPoints, this->ui->playerHeaderPoints, playerCard, playerHand);
    currentCardIndex++;

    //dealer logic
    if (dealerPoints <= 17) {
        deal(deck, dealerPoints, this->ui->dealerHeaderPoints, dealerCard, dealerHand);
    } else
        dealerStand = true;

    //automatic Ace points update
    if (playerPoints > 21 && !aceValueChanged)
        setAceValue(playerHand, playerPoints, this->ui->playerHeaderPoints);

    //preventing unnecessary hits
    if (playerPoints > 21)
            on_standButton_clicked();
}

void Blackjack::on_standButton_clicked()
{
   makeSound("F:/programs/Qt/projects/Blackjack/resources/click.wav");

    playerStand = true;

    //dealer keeps playing by itself
    QLabel* dealerCard = this->ui->dealerCard_3;
    while (dealerPoints <= 17) {
        if (currentCardIndex == 4) {
            dealerCard = this->ui->dealerCard_4;
        } else if (currentCardIndex == 5) {
            dealerCard = this->ui->dealerCard_5;
        } else if (currentCardIndex == 6) {
            dealerCard = this->ui->dealerCard_6;
        } else if (currentCardIndex >= 7) {
            dealerCard = this->ui->dealerCard_7;
        }

        deal(deck, dealerPoints, this->ui->dealerHeaderPoints, dealerCard, dealerHand);
        currentCardIndex++;
    }

    dealerStand = true;

    if (playerStand == true && dealerStand == true) {
        endgame();
    }
}


// menu triggers
//change of card reverse
void Blackjack::on_actionClassic_triggered()
{
    reverseAddress = ":images/resources/classic.png";

    setCardImage(ui->dealerCardHidden);
    setCardImage(ui->deck);
}

void Blackjack::on_actionFantasy_triggered()
{
    reverseAddress = ":images/resources/fantasy.png";

    setCardImage(ui->dealerCardHidden);
    setCardImage(ui->deck);
}

//add custom image
void Blackjack::on_actionAddReverseImage_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose an image for deck reverse"), "", tr("Images(*.png *.jpg *.jpeg *.bmp)"));

    if(QString::compare(filename, QString()) != 0) {
        QImage userImage;
        bool isValid = userImage.load(filename);
        if(isValid) {
            reverseAddress = filename;
        }
    }

    setCardImage(ui->dealerCardHidden);
    setCardImage(ui->deck);
}

//ask for bet
void Blackjack::on_actionNewBet_triggered()
{
    playerBid = QInputDialog::getInt(this, "Bet", "Make your bet:", playerBid, 1, playerBank, 1);
    ui->playerBidPoints->setText(QString::number(playerBid));
}


//main gameplay functions
void Blackjack::deal(Deck& deck, int& points, QLabel *pointsLabel, QLabel *cardLabel, std::vector<Card>& hand) {
    makeSound("F:/programs/Qt/projects/Blackjack/resources/arcade.wav");

    deck.dealCard();

    if (aceValueChanged && (deck.getCurrentCard().getRank() == Card::ACE))
        points -= 10;

    points += deck.getCurrentCard().getCardValue();
    pointsLabel->setText(QString::number(points));
    deck.getCurrentCard().openCard(cardLabel);

    hand.push_back(deck.getCurrentCard());
}

void Blackjack::setCardImage(QLabel *cardLabel) {
    cardLabel->setPixmap(QPixmap(reverseAddress).scaled(cardLabel->width(), cardLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

//cover card animation
void Blackjack::coverCard(QLabel *label) {
    label->setVisible(true);
    setCardImage(label);

    QPropertyAnimation *animation = new QPropertyAnimation(label, "geometry");
    animation->setDuration(150);
    animation->setStartValue(QRect(ui->deck->x(), ui->deck->y(), label->width(), label->height()));
    animation->setEndValue(label->geometry());

    animation->start();
}

void Blackjack::makeSound(const QString source) {
    QSoundEffect* effect = new QSoundEffect();
    effect->setSource(QUrl::fromLocalFile(source));
    effect->setVolume(0.25);
    effect->play();

}

//update Ace point value
void Blackjack::setAceValue(const std::vector<Card> hand, int& points, QLabel *pointsLabel) {
    int counter = 0;
    for (Card card : hand) {
        if (card.getRank() == Card::ACE) {
            counter++;
        }
    }

    points -= counter * 10;
    pointsLabel->setText(QString::number(points));

    aceValueChanged = true;
}

void Blackjack::resetTable() {
    setCardImage(ui->deck);

    QLabel* starterCards[14] = {ui->dealerCard_1, ui->dealerCard_2, ui->dealerCard_3, ui->dealerCard_4, ui->dealerCard_5, ui->dealerCard_6, ui->dealerCard_7,
                                ui->playerCard_1, ui->playerCard_2, ui->playerCard_3, ui->playerCard_4, ui->playerCard_5, ui->playerCard_6, ui->playerCard_7};

    for (auto label : starterCards) {
        label->setPixmap(QPixmap(":images/resources/empty.png"));
    }

    currentCardIndex = 3;

    dealerStand = false;
    playerStand = false;

    dealerPoints = 0;
    playerPoints = 0;
}

void Blackjack::endgame() {
    ui->dealerCardHidden->setVisible(false);

    QMessageBox msgBox;
    msgBox.setWindowTitle("Endgame");
    msgBox.setInformativeText("Want to play again?");
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (playerPoints <= 21 && (dealerPoints < playerPoints ||  dealerPoints > 21)) {
        makeSound("F:/programs/Qt/projects/Blackjack/resources/FFVictory.wav");

        msgBox.setText("Congrats! You've won!");
        playerBank += playerBid;
    } else if (playerPoints == dealerPoints) {
        msgBox.setText("It's a draw!");
    } else {
        makeSound("F:/programs/Qt/projects/Blackjack/resources/WilhelmScream.wav");

        msgBox.setText("You've lost!");
        playerBank -= playerBid;
        if (playerBank <= 0) {
            msgBox.setInformativeText("Game over!");
            msgBox.setStandardButtons(QMessageBox::Ok);
        }
    }

    int result = msgBox.exec();

    switch (result) {
    case QMessageBox::Yes:
        on_startButton_clicked();
        break;
    case QMessageBox::No:
        this->close();
        break;
    case QMessageBox::Ok:
        this->close();
        break;
    default: break;
    } 
}
