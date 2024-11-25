#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QRandomGenerator>
#include <QVector>
#include <QPointF>
#include <QKeyEvent>
#include <QPixmap>

struct Raindrop {
    QPointF position;
    float angle; // Angle of the drop's tilt

    Raindrop(float x, float y, float a) : position(x, y), angle(a) {}
};

struct Human {
    QPointF position;
    float width;
    float height;

    Human(float x, float y, float w, float h)
        : position(x, y), width(w), height(h) {}
};

// New structure for the life rectangle
struct LifeRectangle {
    QPointF position;
    float width;
    float height;

    LifeRectangle(float x = 0, float y = 0, float w = 20, float h = 20)
        : position(x, y), width(w), height(h) {}
};

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updatePoints();
    void increaseSpeed();
    void adjustRaindropAngle();
    void updateRaindrops();
    void spawnLifeRectangle(); // Slot to spawn life rectangle

private:
    // Game state variables
    int lives; // Number of remaining lives
    int collisionCount; // Number of collisions
    QTimer *collisionTimer; // Timer for showing the "COLLISION!!" message
    bool showCollisionMessage; // Flag to control the "COLLISION!!" message
    bool collisionProcessed;
    QElapsedTimer elapsedTimer;
    bool gameOver; // Game over state

    // Life rectangle variables
    LifeRectangle lifeRectangle; // Life rectangle object
    bool lifeRectangleActive; // Whether the life rectangle is currently active
    QTimer *lifeRectangleTimer; // Timer for spawning the life rectangle
    void updateLifeRectangle(); // Method to update life rectangle's position

    // Background and UI
    QPixmap background;
    Ui::MainWindow *ui;

    // Points and speed
    QTimer *pointsTimer; // Timer for updating points
    int points; // Variable to store the points
    QTimer *speedIncreaseTimer; // Timer to increase speed
    QTimer *angleAdjustmentTimer; // Timer to adjust raindrop angles
    int rainSpeed; // Variable for controlling speed

    // Raindrops
    QVector<Raindrop> raindrops;
    QTimer *timer; // Timer for updating raindrops
    int dropWidth;
    int dropHeight;

    // Human object
    Human human; // Human object

    // Private methods
    void addRaindrops(int count); // Method to add raindrops
    void handleCollision(); // Method to check for collisions
    void keyPressEvent(QKeyEvent *event) override; // Key event handler
};

#endif // MAINWINDOW_H
