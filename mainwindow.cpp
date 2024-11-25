#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), dropWidth(10), dropHeight(30), rainSpeed(5),
    human(width() / 2, height() - 50, 20, 40), points(0), lives(3), gameOver(false), showCollisionMessage(false), collisionCount(0), collisionProcessed(false), lifeRectangleActive(false) {
    ui->setupUi(this);

    background.load(":/image/background_rain.jpg"); // Replace with your image path

    // Setup collision timer
    collisionTimer = new QTimer(this);
    collisionTimer->setSingleShot(true);
    connect(collisionTimer, &QTimer::timeout, this, [this]() {
        showCollisionMessage = false;
        update();
    });

    //Elapsed Timer
    elapsedTimer.start();

    // Set up the timer for updating raindrops
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateRaindrops);
    timer->start(50); // Update every 50 ms

    // Initialize points timer
    pointsTimer = new QTimer(this);
    connect(pointsTimer, &QTimer::timeout, this, &MainWindow::updatePoints);
    pointsTimer->start(150); // Update points every 0.15 seconds

    // Initialize speed increase timer
    speedIncreaseTimer = new QTimer(this);
    connect(speedIncreaseTimer, &QTimer::timeout, this, &MainWindow::increaseSpeed);
    speedIncreaseTimer->start(5000); // Increase speed every 5 seconds

    // Initialize angle adjustment timer
    angleAdjustmentTimer = new QTimer(this);
    connect(angleAdjustmentTimer, &QTimer::timeout, this, &MainWindow::adjustRaindropAngle);
    angleAdjustmentTimer->start(40000); // Adjust angle every 40 seconds

    // Initialize life rectangle timer
    lifeRectangleTimer = new QTimer(this);
    connect(lifeRectangleTimer, &QTimer::timeout, this, &MainWindow::spawnLifeRectangle);
    lifeRectangleTimer->start(20000); // Spawn life rectangle every 20 seconds

    // Add initial raindrops
    addRaindrops(10); // Start with 10 raindrops
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::spawnLifeRectangle() {
    if (!gameOver && !lifeRectangleActive) {
        lifeRectangle.position.setX(static_cast<float>(QRandomGenerator::global()->bounded(width())));
        lifeRectangle.position.setY(0); // Start at the top of the screen
        lifeRectangle.width = 20;
        lifeRectangle.height = 20;
        lifeRectangleActive = true;
    }
}

void MainWindow::updateLifeRectangle() {
    if (lifeRectangleActive) {
        lifeRectangle.position.setY(lifeRectangle.position.y() + rainSpeed);

        if (lifeRectangle.position.y() > height()) {
            // Deactivate life rectangle if it goes out of bounds
            lifeRectangleActive = false;
        }
    }
}

void MainWindow::updatePoints() {
    if (!gameOver) {
        points += 1; // Increment points
    } else {
        pointsTimer->stop(); // Stop updating points when game is over
    }
}

void MainWindow::increaseSpeed() {
    rainSpeed += 1; // Increase rain speed
}

void MainWindow::adjustRaindropAngle() {
    for (Raindrop &drop : raindrops) {
        drop.angle += 5; // Increase angle by 5 degrees
        if (drop.angle > 45) {
            drop.angle = 45; // Cap angle at 45 degrees
        }
    }
}

void MainWindow::addRaindrops(int count) {
    for (int i = 0; i < count; ++i) {
        float x = static_cast<float>(QRandomGenerator::global()->bounded(width()));
        float angle = 0;
        raindrops.append(Raindrop(x, 0, angle));
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);

    // Draw the background image scaled to the window size
    painter.drawPixmap(rect(), background);

    if (gameOver) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 30, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "GAME OVER");

        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(10, 40, QString(" Points: %1").arg(points));

        painter.setFont(QFont("Arial", 20, QFont::Bold));
        qint64 elapsedMs = elapsedTimer.elapsed();
        qint64 seconds = elapsedMs / 1000;
        qint64 minutes = seconds / 60;
        qint64 hours = minutes / 60;
        seconds %= 60;
        minutes %= 60;

        QString elapsedTime = QString("Time Elapsed: %1:%2:%3")
                                  .arg(hours, 2, 10, QChar('0'))
                                  .arg(minutes, 2, 10, QChar('0'))
                                  .arg(seconds, 2, 10, QChar('0'));

        QRect timeRect(0, 0, width(), 30);
        painter.drawText(timeRect, Qt::AlignHCenter | Qt::AlignTop, elapsedTime);

        return;
    }

    // Display points
    painter.setPen(Qt::red);
    painter.setFont(QFont("Arial", 20, QFont::Bold));
    painter.drawText(10, 30, QString("Points: %1").arg(points));

    // Calculate and display elapsed time
    qint64 elapsedMs = elapsedTimer.elapsed();
    qint64 seconds = elapsedMs / 1000;
    qint64 minutes = seconds / 60;
    qint64 hours = minutes / 60;
    seconds %= 60;
    minutes %= 60;

    QString elapsedTime = QString("Time Elapsed: %1:%2:%3")
                              .arg(hours, 2, 10, QChar('0'))
                              .arg(minutes, 2, 10, QChar('0'))
                              .arg(seconds, 2, 10, QChar('0'));

    QRect timeRect(0, 0, width(), 30);
    painter.drawText(timeRect, Qt::AlignHCenter | Qt::AlignTop, elapsedTime);



    // Draw lives as red rectangles
    painter.setBrush(Qt::red);
    for (int i = 0; i < lives; ++i) {
        painter.drawRect(10 + i * 20, 50, 15, 15);
    }

    // Show collision message if needed
    if (showCollisionMessage) {
        painter.setPen(Qt::red);
        painter.setFont(QFont("Arial", 20, QFont::Bold));
        painter.drawText(rect(), Qt::AlignCenter, "OOPS.. carefull!!");
    }

    // Draw raindrops
    painter.setPen(Qt::black);
    painter.setBrush(Qt::blue);
    for (const Raindrop &drop : raindrops) {
        QRectF rect(drop.position.x() - dropWidth / 2.0, drop.position.y(), dropWidth, dropHeight);

        painter.save();
        painter.translate(rect.center());
        painter.rotate(-drop.angle);
        painter.drawRect(-dropWidth / 2.0, -dropHeight, dropWidth, dropHeight);
        painter.restore();
    }

    // Draw the human figure
    painter.setBrush(Qt::yellow); // Head
    painter.drawEllipse(human.position.x() + 5, human.position.y() - 30, 20, 20);

    painter.setBrush(Qt::green); // Torso
    painter.drawRect(human.position.x() + 5, human.position.y() - 10, 20, 30);

    painter.setPen(Qt::NoPen); // Arms
    painter.setBrush(Qt::red);
    painter.drawRect(human.position.x() - 10, human.position.y() - 10, 10, 5);  // Left arm
    painter.drawRect(human.position.x() + 25, human.position.y() - 10, 10, 5); // Right arm

    painter.setBrush(Qt::red); // Legs
    painter.drawRect(human.position.x() + 5, human.position.y() + 20, 8, 20);  // Left leg
    painter.drawRect(human.position.x() + 17, human.position.y() + 20, 8, 20); // Right leg

    // Draw the life rectangle if active
    if (lifeRectangleActive) {
        painter.setBrush(Qt::red);
        painter.drawRect(lifeRectangle.position.x(), lifeRectangle.position.y(), lifeRectangle.width, lifeRectangle.height);
    }

    handleCollision(); // Ensure collision is checked after drawing
}


void MainWindow::updateRaindrops() {
    if (gameOver) return;    // Prevent updates if game is over

    for (Raindrop &drop : raindrops) {
        float angleRad = qDegreesToRadians(drop.angle);
        drop.position.setX(drop.position.x() + rainSpeed * qSin(angleRad));
        drop.position.setY(drop.position.y() + rainSpeed);

        if (drop.position.y() > height()) {
            drop.position.setY(0);
            drop.position.setX(static_cast<float>(QRandomGenerator::global()->bounded(width())));
        }
    }

    // Update life rectangle position
    updateLifeRectangle();

    handleCollision(); // Check for collisions after updating positions
    update(); // Trigger a repaint
}

void MainWindow::handleCollision() {
    for (Raindrop &drop : raindrops) {
        QRectF raindropRect(drop.position.x() - dropWidth / 2.0, drop.position.y(), dropWidth, dropHeight);
        QRectF humanRect(human.position.x(), human.position.y(), human.width, human.height);

        if (raindropRect.intersects(humanRect)) {
            if (collisionProcessed) return; // Prevent multiple collisions in one frame

            qDebug() << "Collision detected!";
            qDebug() << "Lives before collision:" << lives;

            // Check if collision count is less than 3
            if (collisionCount < lives) {
                collisionCount++; // Increment collision count
                showCollisionMessage = true; // Show collision message
                collisionTimer->start(1000); // Show message for 1 second
            }

            lives--; // Decrease a life
            qDebug() << "Lives after collision:" << lives;

            if (lives <= 0) {
                timer->stop();
                pointsTimer->stop();
                gameOver = true; // Trigger game over
            }

            collisionProcessed = true; // Mark collision as processed for this frame
            update(); // Trigger a repaint
            return; // Exit after handling collision
        }
    }

    // Check for collision with the life rectangle
    QRectF lifeRect(lifeRectangle.position.x(), lifeRectangle.position.y(), lifeRectangle.width, lifeRectangle.height);
    QRectF humanRect(human.position.x(), human.position.y(), human.width, human.height);

    if (lifeRectangleActive && lifeRect.intersects(humanRect)) {
        lives++; // Grant an extra life
        collisionCount--;
        qDebug() << "Life rectangle collected! Lives:" << lives;
        lifeRectangleActive = false; // Deactivate the life rectangle
    }

    collisionProcessed = false; // Reset for the next frame
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (gameOver) return; // Prevent movement if game is over
    switch (event->key()) {
    case Qt::Key_Left:
        human.position.setX(human.position.x() - 10); // Move left
        if (human.position.x() < 0)
            human.position.setX(0); // Prevent moving out of the left boundary
        break;
    case Qt::Key_Right:
        human.position.setX(human.position.x() + 10); // Move right
        if (human.position.x() + human.width > width())
            human.position.setX(width() - human.width); // Prevent moving out of the right boundary
        break;
    case Qt::Key_Up:
        human.position.setY(human.position.y() - 10); // Move up
        if (human.position.y() < 0)
            human.position.setY(0); // Prevent moving out of the top boundary
        break;
    case Qt::Key_Down:
        human.position.setY(human.position.y() + 10); // Move down
        if (human.position.y() + human.height > height())
            human.position.setY(height() - human.height); // Prevent moving out of the bottom boundary
        break;
    default:
        QMainWindow::keyPressEvent(event); // Call the base class implementation for unhandled keys
    }
    update(); // Repaint the window to reflect the changes
}

