#include "pch.h"
#include "boydsa.h"

auto canvasWidth = 0;
auto canvasHeight = 0;
auto seedMargin = 0;

std::vector<Boyda> boyds = {};

void InitializeBoydsa(int width, int height, int averageMargin, int amountPerGroup)
{
    canvasWidth = width;
    canvasHeight = height;
    seedMargin = averageMargin;

    _Create(amountPerGroup, 1);
    _Create(amountPerGroup, 2);
    _Create(amountPerGroup, 3);
}

void IterateBoydsa()
{
    const auto& numBoyds = boyds.size();

    for (auto i = 0; i < numBoyds; ++i)
    {
        auto forceX = 0.0;
        auto forceY = 0.0;
        auto& boydA = boyds[i];

        for (auto j = 0; j < numBoyds; ++j)
        {
            if (j != i)
            {
                const auto& boydB = boyds[j];
                const auto& attraction = attractor[boydA.group * groupCount + boydB.group];
                const double& dX = boydA.positionX - boydB.positionX;
                if (dX != 0)
                {
                    const double& dY = boydA.positionY - boydB.positionY;
                    if (dY != 0)
                    {
                        const auto& distance = dX * dX + dY * dY;
                        if (distance < radiusThresholdSquared)
                        {
                            /* classical newton */
                            const auto& perceivedForce = attraction / sqrt(distance);
                            forceX += perceivedForce * dX;
                            forceY += perceivedForce * dY;
                        }
                    }
                }
            }
        }

        /* accelerate */
        boydA.velocityX = acceleration * (boydA.velocityX + forceX);
        boydA.velocityY = acceleration * (boydA.velocityY + forceY);
        
        /* translate */
        boydA.positionX += boydA.velocityX;
        boydA.positionY += boydA.velocityY;
        
        /* bounce */
        if (boydA.positionX <= 0)
        {
            boydA.velocityX = -boydA.velocityX;
            boydA.positionX = 0;
        }

        if (boydA.positionX >= canvasWidth)
        {
            boydA.velocityX = -boydA.velocityX;
            boydA.positionX = canvasWidth;
        }

        if (boydA.positionY <= 0)
        {
            boydA.velocityY = -boydA.velocityY;
            boydA.positionY = 0;
        }

        if (boydA.positionY >= canvasHeight)
        {
            boydA.velocityY = -boydA.velocityY;
            boydA.positionY = canvasHeight;
        }
    }
}

void RenderBoydsa(cairo_t* const& drawingSink)
{
    for (auto& boyd : boyds)
    {
        auto const& r = 1. - (1. / abs(boyd.velocityX));
        auto const& g = static_cast<double>(boyd.group) / 3.;
        auto const& b = 1. / abs(boyd.velocityY);

        cairo_set_source_rgb(drawingSink, r, g, b);
        cairo_rectangle(
            drawingSink,
            boyd.positionX - 1,
            boyd.positionY - 1,
            boydSize,
            boydSize
        );

        cairo_fill(drawingSink);
    }
}

void TeardownBoydsa()
{
    boyds.clear();
}

double _Random()
{
    return (static_cast<double>(std::rand()) / static_cast<double>(RAND_MAX)) * (canvasHeight - seedMargin * 2) + seedMargin;
};

void _Create(int amount, char group)
{
    for (auto i = 0; i < amount; ++i)
    {
        boyds.push_back(Boyda{ 
            static_cast<double>(_Random()),
            static_cast<double>(_Random()),
            group 
        });
    }
}