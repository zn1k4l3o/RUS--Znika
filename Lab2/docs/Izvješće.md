| Stavka          | Odgovor                                       |
| --------------- | --------------------------------------------- |
| Platforma       | ESP                                           |
| Varijanta       | B                                             |
| Sleep mode      | Light sleep                                   |
| Buđenje        | Timer                                         |
| Čuvanje stanja | RTC memorija                                  |
| Debouncing      | -                                             |
| Wokwi link     | https://wokwi.com/projects/463629542514000897 |

Analiza potrošnje

ESP32-S2 

    Active mode bez Wi-Fia 18-25 mA

    Light sleep 450 micro A

DHT22

    Active 1-2.5 mA

    Standby 40-60 micro A

HC-SR04

    Active 15 mA

    Standby 2mA

LED

    Active 10-20 mA


Loop

ESP32 -> Active 1000ms + Light sleep 2000ms

HC-SR -> Active 12ms

DHT - nedefinirano koliko dugo je upaljen (nema delaya)

LED -> 1012ms


Izračun (Najgori slučaj)


Baterija 2500 mAh

I_avg = (1012 ms * 0.025 A + 2000 ms * 0.00045 A + 12 ms * 0.015 A + 3000 ms * 0.002 A + 3012 ms * 0.00006 A + 1012 ms * 0.02 A) / 3012 ms

I_avg = (25.3 mAs + 0.9 mAs + 0.18 mAs + 6 mAs + 0.18072 mAs + 20.24 mAs) / 3012 ms

I_avg = (58.80072 / 3012) A = 0.01753 A = **17.53 mA**

T_total = 2500 mAh / 17.53 mA = **142.62 h**
