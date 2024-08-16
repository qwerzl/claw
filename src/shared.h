//
// Created by Tom Tang on 2024/8/13.
//

#ifndef CLAW_SHARED_H
#define CLAW_SHARED_H

enum command {
    // Joystick
    XPositive,
    XNegative,
    YPositive,
    YNegative,
    Still,

    // Button: electromagnet
    EM_ON,
    EM_OFF,

    //Button: Z axis
    Z_ON,
    Z_STILL,
};

#endif //CLAW_SHARED_H
