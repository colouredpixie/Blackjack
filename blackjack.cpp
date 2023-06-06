#include "blackjack.h"
#include "./ui_blackjack.h"

#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QInputDialog>
#include <QFileDialog>

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

    deck.shuffleDeck();

    //first deal - two cards each
    deal(deck, dealerPoints, this->ui->dealerHeaderPoints, this->ui->dealerCard_1);
    deal(deck, dealerPoints, this->ui->dealerHeaderPoints, this->ui->dealerCard_2);
    ui->dealerCardHidden->setVisible(true);
    setCardImage(ui->dealerCardHidden);

    deal(deck, playerPoints, this->ui->playerHeaderPoints, this->ui->playerCard_1);
    deal(deck, playerPoints, this->ui->playerHeaderPoints, this->ui->playerCard_2);
}

void Blackjack::on_hitButton_clicked()
{
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

    deal(deck, playerPoints, this->ui->playerHeaderPoints, playerCard);
    currentCardIndex++;

    //dealer logic
    if (dealerPoints <= 17) {
        deal(deck, dealerPoints, this->ui->dealerHeaderPoints, dealerCard);
    } else
        dealerStand = true;

    //preventing unnecessary hits
    if (playerPoints > 21)
        on_standButton_clicked();
}

void Blackjack::on_standButton_clicked()
{
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

        deal(deck, dealerPoints, this->ui->dealerHeaderPoints, dealerCard);
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
    reverseAddress = "F:/programs/Qt/projects/Blackjack/resources/classic.png";

    setCardImage(ui->dealerCardHidden);
    setCardImage(ui->deck);
}

void Blackjack::on_actionFantasy_triggered()
{
    reverseAddress = "F:/programs/Qt/projects/Blackjack/resources/fantasy.png";

    setCardImage(ui->dealerCardHidden);
    setCardImage(ui->deck);
}

//add custom image
void Blackjack::on_actionAddReverseImage_triggered()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Choose an image for deck reverse"), "", tr("Images(*.png *.jpg *.jpeg *.bmp"));

    if(QString::compare(filename, QString()) != 0) {
        QImage userImage;
        bool isValid = userImage.load(filename);
        if(isValid) {
            /*userImage = userImage.scaledToHeight(ui->deck->height(),Qt::SmoothTransformation);
            ui->dealerCardHidden->setPixmap(QPixmap::fromImage(userImage));
            ui->deck->setPixmap(QPixmap::fromImage(userImage));
            */
            reverseAddress = filename;
        }
    }

    setCardImage(ui->dealerCardHidden); //ui->dealerCardHidden->setPixmap(QPixmap(reverseAddress).scaled(ui->dealerCardHidden->width(), ui->dealerCardHidden->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    setCardImage(ui->deck);
}

//ask to bet
void Blackjack::on_actionNewBet_triggered()
{
    playerBid = QInputDialog::getInt(this, "Bet", "Make your bet:", playerBid, 1, playerBank, 1);
    ui->playerBidPoints->setText(QString::number(playerBid));
}

//TODO: set Ace point value
void Blackjack::on_actionEquals_1_triggered()
{
    aceValue = 1;
}

void Blackjack::on_actionEquals_11_triggered()
{
    aceValue = 11;
}


//Main gameplay
void Blackjack::deal(Deck& deck, int& points, QLabel *pointsLabel, QLabel *cardLabel) {
    deck.dealCard();
    points += deck.getCurrentCard().getCardValue();
    pointsLabel->setText(QString::number(points));
    deck.getCurrentCard().openCard(cardLabel);

    ui->deckLeft->setText(QString::number(52 - deck.getIndex()));
}

void Blackjack::setCardImage(QLabel *cardLabel) {
    cardLabel->setPixmap(QPixmap(reverseAddress).scaled(cardLabel->width(), cardLabel->height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void Blackjack::resetTable() {
    setCardImage(ui->deck);

    QLabel* starterCards[14] = {ui->dealerCard_1, ui->dealerCard_2, ui->dealerCard_3, ui->dealerCard_4, ui->dealerCard_5, ui->dealerCard_6, ui->dealerCard_7,
                                ui->playerCard_1, ui->playerCard_2, ui->playerCard_3, ui->playerCard_4, ui->playerCard_5, ui->playerCard_6, ui->playerCard_7};

    for (auto label : starterCards) {
        label->setPixmap(QPixmap("F:/programs/Qt/projects/Blackjack/resources/empty.png"));
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
        msgBox.setText("Congrats! You've won!");
        playerBank += playerBid;
    } else if (playerPoints == dealerPoints) {
        msgBox.setText("It's a draw!");
    } else {
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
