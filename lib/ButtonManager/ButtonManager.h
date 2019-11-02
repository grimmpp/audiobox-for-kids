#ifndef BUTTON_HELPER_H
#define BUTTON_HELPER_H

#include <ArduinoLog.h>
// https://github.com/thijse/Arduino-Log/blob/master/examples/Log/Log.ino

#include <JC_Button.h>

class ButtonManager {
public:
    ButtonManager(uint32_t longPressDelay, uint8_t btDownPin, uint8_t btPlayPin, uint8_t btUpPin) :
        LONG_PRESS(longPressDelay),
        downButton(btDownPin),
        playButton(btPlayPin),
        upButton(btUpPin) { 

        pinMode(btPlayPin, INPUT_PULLUP);
        pinMode(btUpPin, INPUT_PULLUP);
        pinMode(btDownPin, INPUT_PULLUP);
    }

    void readAllButtons() {
        playButton.read();
        upButton.read();
        downButton.read();
    }

    /**
     * Reset button delay for long push
     */
    void resetButtonDelayForLongPress() {
        if (!downButton.isPressed() && !upButton.isPressed() && !playButton.isPressed()){
        buttonDelayFactor = 1;
        }
    }

    void waitForButtonToBeReleased(Button b) {
        do { b.read(); }
        while (!b.wasReleased());
    }

    void restartOption() {
        if (upButton.pressedFor(LONG_PRESS) && downButton.pressedFor(LONG_PRESS)) {
            Log.notice(F("Restart Arduino!"));
            asm volatile ("  jmp 0"); 
        }
    }

    bool isPlayButtonPressedForLong() {
        return playButton.pressedFor(LONG_PRESS);
    }

    void waitForPlayButtonToBeReleased() {
        waitForButtonToBeReleased(playButton);
    }

    bool wasButtonReleased(Button b) {
        return buttonDelayFactor == 1 &&  b.wasReleased();
    }

    bool wasPlayButtonReleased() {
        return wasButtonReleased(playButton);
    }

    bool isButtonPressedForLong(Button btn) {
        bool result = btn.pressedFor(LONG_PRESS * buttonDelayFactor);
        if (result) buttonDelayFactor++;
        return result;
    }

    bool isUpButtonPressedForLong() {
        return isButtonPressedForLong(upButton);
    }

    bool wasUpButtonReleased() {
        return wasButtonReleased(upButton);
    }

    bool isDownButtonPressedForLong() {
        return isButtonPressedForLong(downButton);
    }

    bool wasDownButtonReleased() {
        return wasButtonReleased(downButton);
    }

private:
    Button playButton;
    Button upButton;
    Button downButton;

    uint32_t LONG_PRESS;
    // if button is press it will increasing or decreasing the volume in steps by a delay of LONG_PRESS
    uint8_t buttonDelayFactor = 1;
};

#endif