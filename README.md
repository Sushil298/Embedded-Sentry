# Introduction
The objective is to develop a hand movement-based unlocking system that utilizes data from a single ac-
celerometer and/or gyro. The system records a unique hand movement sequence, saves it on a microcontroller
(EEPROM), and requires the user to accurately replicate the sequence to unlock the system. A visual indicator
and sound effect confirm a successful/unsuccessful unlock.

# Project Outline
Initially, data is recorded from the onboard accelerometer in the x, y, and z directions. To optimize memory
usage, the recorded data arrays are combined into a single array. When the user attempts to unlock the device,
data is recorded in a similar manner. Both the stored pattern and the attempted pattern undergo a smoothing
process using a moving average function. Subsequently, the dynamic time warping algorithm is employed to
compare the two patterns. If the dynamic time warping distance between the patterns falls below the predefined
threshold distance, the system is successfully unlocked.
It is worth noting that while it is possible to compare x, y, and z direction data separately, this approach
would require more memory. To ensure the correct pattern can be stored even when power is off, we chose to
combine the datapoints, accommodating them within the available memory in the microcontroller.

# Users' Guide
## Saving the Password

To save the lock pattern to the microcontroller, the user needs to press and hold the left button. While holding
the button, they can create a gesture using the Circuit Playground board. The beginning and end of the gesture
recording process are indicated by beep sounds. Throughout the recording, the Neo Pixels on the board flicker
in a purple color, providing a visual feedback to the user.  
<img width="498" alt="Screen Shot 2023-10-29 at 5 36 29 PM" src="https://github.com/Sushil298/Embedded-Sentry/assets/80779647/f87bf5c0-a076-41d3-8f67-c31146caf8b3">


## Unlocking the System
To attempt to unlock the microcontroller, the user should press and hold the right button. While holding
the button, they can create a gesture using the Circuit Playground board. The start and end of the gesture
recording process are indicated by beep sounds. During the recording, the Neo Pixels on the board flicker in a
blue color, serving as visual feedback. If the unlock attempt is successful, the Neo Pixels light up sequentially
in green color accompanied by a high-frequency beep sound. Conversely, if the unlock attempt fails, all Neo
Pixels illuminate simultaneously in red color with double beeps. These visual and auditory cues provide clear
feedback to the user regarding the outcome of the unlock attempt.  
<img width="488" alt="Screen Shot 2023-10-29 at 5 37 09 PM" src="https://github.com/Sushil298/Embedded-Sentry/assets/80779647/e1eecf1b-149a-4661-973e-763817412af9">


# Conclusion
The code for the embedded challenge successfully enabled us to record and compare movements as passwords.
It provided accurate feedback through lights and sounds, ensuring repeatability. However, we encountered a
limitation where symmetrical patterns could also unlock the system. This occurred due to our decision to
combine the x, y, and z acceleration data. While this issue could be resolved by comparing the data separately,
we chose to prioritize efficient EEPROM memory usage.
