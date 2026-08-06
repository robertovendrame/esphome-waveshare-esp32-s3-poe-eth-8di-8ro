# Alarm Contact Collector per Waveshare ESP32-S3 8DI/8RO

Firmware ESPHome per usare la Waveshare ESP32-S3-POE-ETH-8DI-8RO come
concentratore autonomo di contatti puliti provenienti da centrali di allarme.

La configurazione iniziale gestisce due centrali, ma la scheda dispone di otto
ingressi digitali. I relè sono mantenuti spenti e non vengono esposti finché non
saranno progettate funzioni di comando specifiche e sicure.

## Funzioni principali

- due ingressi allarme configurabili singolarmente come NO o NC;
- memoria, durata, contatore e ultimo evento per ciascuna centrale;
- LED RGB, buzzer ed esclusione temporanea degli ingressi;
- Ethernet PoE, RTC hardware e sincronizzazione SNTP autonoma;
- registro circolare persistente degli ultimi 500 eventi;
- storico locale in pagina web, CSV e JSON;
- notifiche Telegram ed email Brevo indipendenti da Home Assistant;
- comandi Telegram da chat privata o gruppo autorizzato;
- più amministratori, audit del mittente e conferma a due passaggi;
- invio affidabile a due istanze Home Assistant mediante webhook autenticati;
- heartbeat, ritentativi e code indipendenti dopo blackout o assenza Internet.

Versione firmware corrente: **1.0.0-rc3**.

## File principali

| Percorso | Contenuto |
| --- | --- |
| `alarm-collector-advanced.yaml` | Firmware ESPHome |
| `secrets.example.yaml` | Modello dei secret richiesti |
| `components/event_history/` | Registro eventi persistente |
| `home-assistant/packages/` | Package ricevitore Home Assistant |
| `home-assistant/dashboards/` | Dashboard Lovelace pronta |
| `README-alarm-collector.md` | Installazione e configurazione completa |

## Installazione rapida

1. Copiare `secrets.example.yaml` come `secrets.yaml`.
2. Sostituire tutti i valori dimostrativi con le proprie credenziali.
3. Usare ESPHome 2026.7.0 o successivo.
4. Compilare e installare:

   ```bash
   esphome run alarm-collector-advanced.yaml
   ```

Il passaggio diretto da una versione precedente, inclusa la `0.8.0`, alla
`1.0.0-rc3` è supportato. Gli aggiornamenti successivi possono essere eseguiti
normalmente tramite ESPHome OTA.

## Collegamenti iniziali

| Centrale | Morsetti Waveshare |
| --- | --- |
| Centrale 1 | DI1 e DCOM |
| Centrale 2 | DI2 e DCOM |

Con logica NO la chiusura produce allarme. Con logica NC l'apertura produce
allarme e consente una protezione elementare contro il taglio del cavo. Non è
una supervisione con resistenza EOL.

## Sicurezza

- non pubblicare mai `secrets.yaml`;
- usare token webhook lunghi, casuali e differenti per ogni Home Assistant;
- autorizzare Telegram tramite ID numerici, non username;
- i comandi operativi richiedono un codice monouso legato a utente e chat;
- i contatti dei relè possono commutare tensioni pericolose, anche se il
  firmware attuale li mantiene spenti.

Questo progetto non sostituisce una centrale antintrusione certificata e non ne
replica certificazioni, supervisione o comunicazioni di sicurezza.

Licenza MIT.
