# ESPHome per Waveshare ESP32-S3-POE-ETH-8DI-8RO

Firmware ESPHome da banco prova per verificare le periferiche della scheda:

- 8 relè tramite TCA9554 a `0x20`;
- 8 ingressi digitali optoisolati;
- Ethernet/PoE tramite W5500;
- LED RGB WS2812 e buzzer;
- RTC PCF85063 a `0x51`;
- RS485 a 9600 baud con log TX/RX;
- Bluetooth Proxy;
- pulsante BOOT e diagnostica ESP32.

Il repository contiene anche `alarm-collector-advanced.yaml`, firmware per
raccogliere allarmi a contatto pulito da due centrali con memoria eventi,
segnalazioni locali e notifiche indipendenti tramite Telegram ed email Brevo.
La guida dedicata è in `README-alarm-collector.md`.

## Sicurezza

Il firmware forza tutti i relè su OFF ad ogni avvio e gli ingressi non comandano
automaticamente le uscite. Il test sequenziale aziona realmente tutti i relè.
Non collegare carichi durante il primo collaudo.

I contatti dei relè possono commutare tensioni pericolose. Togli alimentazione
prima di intervenire sui morsetti e usa protezioni adeguate al carico.

## Installazione

1. Copia `secrets.example.yaml` in `secrets.yaml`.
2. Genera la chiave API con `openssl rand -base64 32`.
3. Inserisci chiave API e password OTA in `secrets.yaml`.
4. Collega la USB-C, tieni premuto BOOT se necessario e installa:

   ```bash
   esphome run waveshare-8di-8ro-test.yaml
   ```

5. Dopo il primo flash, collega Ethernet/PoE. Gli aggiornamenti successivi
   possono essere eseguiti OTA.

## Collaudo consigliato

1. Alimentazione USB-C, senza carichi sui relè.
2. Verifica nel log la scansione I²C:
   - TCA9554 `0x20`
   - PCF85063 `0x51`
3. Premi `Test LED RGB` e `Test buzzer`.
4. Premi `Test relè sequenziale`: ogni relè deve scattare due volte.
5. Chiudi uno alla volta gli ingressi verso `DCOM`: le entità devono diventare
   ON.
6. Collega Ethernet e verifica IP, MAC, pagina web e API Home Assistant.
7. Per RS485 collega un dispositivo a 9600 8N1. Il pulsante invia una lettura
   Modbus RTU standard (slave 1, holding register 0); TX e RX vengono mostrati
   nel log.
8. Verifica che l’RTC mantenga l’ora dopo uno spegnimento con batteria collegata.

## Note hardware

| Funzione | Collegamento |
| --- | --- |
| DI1–DI8 | GPIO4–GPIO11, attivi bassi |
| Relè 1–8 | TCA9554 P0–P7, I²C `0x20` |
| I²C | SCL GPIO41, SDA GPIO42 |
| W5500 | INT 12, MOSI 13, MISO 14, CLK 15, CS 16 |
| RS485 | TX GPIO17, RX GPIO18 |
| RGB | GPIO38 |
| Buzzer | GPIO46 |
| BOOT | GPIO0 |
| RTC | PCF85063, I²C `0x51` |

Lo slot TF non è incluso nel test: la documentazione Waveshare di questa
revisione riporta il chip-select non collegato.

## Stato del progetto

- firmware di collaudo `0.1.2`;
- Alarm Contact Collector `0.4.0`;
- verificati su hardware reale: LED RGB, buzzer, otto relè senza carichi, otto
  ingressi digitali, Ethernet, RTC, Home Assistant e Telegram;
- integrazione email Brevo inclusa;
- prova RS485 in attesa dell'adattatore esterno.
