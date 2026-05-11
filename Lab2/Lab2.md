Link na labos:

https://wokwi.com/projects/463629542514000897


Sklop prati promjenu temperature i vlažnosti zraka te udaljenosti. Prilikom svakog očitanja senzora se podaci spremaju u nizove maksimalne veličine 10. Potom se ESP stavlja u light sleep mode radi očuvanja energije. Nakon 2 seknde timer budi mikrokontroler iz light sleep moda te ponovo mjeri podatke, sve dok se e izmjeri 10 puta. 

Sklop se sastoji od ESP32 mikrokontrolera, DHT22 komponente za mjerenje vlažnosti i temperature te senzora za udaljenost.
