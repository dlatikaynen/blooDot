#ifndef BOYDSA_H
#define BOYDSA_H
#include "../../cairo/include/cairo.h"

namespace blooDot::Algo::Boyds {
    typedef struct Boyda {
        double positionX = 0;
        double positionY = 0;
        char group = 0;

        double velocityX = 0;
        double velocityY = 0;
    } Boyda;

    constexpr int radiusThresholdSquared = 80 * 80;
    constexpr int groupCount = 4;
    constexpr int boydSize = 3;
    constexpr double acceleration = .5;

    constexpr double green_green = 0.878214014158254;
    constexpr double green_red = 0.383942932294564;
    constexpr double green_yellow = 0.3632328353781209;
    constexpr double green_blue = 0.4357079645785089;
    constexpr double red_green = -0.8131279812066854;
    constexpr double red_red = 0.8761564046567396;
    constexpr double red_yellow = 0.686246916739194;
    constexpr double red_blue = -0.42403398294928163;
    constexpr double yellow_green = -0.8283611643992606;
    constexpr double yellow_red = -0.8050409003234531;
    constexpr double yellow_yellow = 0.8422661062679588;
    constexpr double yellow_blue = -0.6206303204367405;
    constexpr double blue_green = -0.6276679142294777;
    constexpr double blue_red = -0.48726835984229977;
    constexpr double blue_yellow = -0.8155039608681607;
    constexpr double blue_blue = 0.49503848830455155;

    constexpr double attractor[] = {
        green_green,
        green_red,
        green_yellow,
        green_blue,
        red_green,
        red_red,
        red_yellow,
        red_blue,
        yellow_green,
        yellow_red,
        yellow_yellow,
        yellow_blue,
        blue_green,
        blue_red,
        blue_yellow,
        blue_blue
    };

    void InitializeBoydsa(int width, int height, int averageMargin, int amountPerGroup);
    void IterateBoydsa();
    void RenderBoydsa(cairo_t* const& drawingSink);
    void TeardownBoydsa();

    double RandomInternal();
    void CreateInternal(int amount, char group);
}
#endif //BOYDSA_H
