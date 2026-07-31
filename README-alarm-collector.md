# Alarm Contact Collector Advanced

Firmware ESPHome per raccogliere allarmi a contatto pulito da due centrali,
espandibile fino a otto ingressi, sulla Waveshare ESP32-S3-POE-ETH-8DI-8RO.

## Funzioni della versione 0.5.0

- due centrali configurabili NO/NC da Home Assistant;
- ingresso attivo, memoria allarme, ultimo evento, durata e contatore;
- esclusione individuale per manutenzione;
- buzzer tacitabile e LED RGB di stato;
- RTC locale e funzionamento anche senza Home Assistant;
- registro circolare persistente degli ultimi 500 eventi, su partizione NVS
  dedicata con scritture incrementali;
- storico consultabile localmente da browser ed esportabile in CSV o JSON;
- notifiche Telegram HTTPS dirette, indipendenti da Home Assistant e
  disattivabili da un interruttore;
- diagnostica Telegram completa e notifiche tecniche di avvio, perdita/ripristino
  Ethernet e perdita/ripristino Home Assistant;
- email transazionali dirette tramite API HTTPS Brevo, indipendenti da Home
  Assistant e disattivabili da un interruttore;
- sei ingressi di riserva;
- tutti gli otto relè forzati OFF e non esposti.

## Collegamenti iniziali

| Centrale | Waveshare |
| --- | --- |
| Contatto allarme centrale 1 | DI1 e DCOM |
| Contatto allarme centrale 2 | DI2 e DCOM |

Con logica `NO`, la chiusura del contatto genera l'allarme. Con logica `NC`,
l'apertura genera l'allarme e offre una protezione elementare contro
l'interruzione del cavo. Questa non è una supervisione EOL: allarme e cavo
interrotto non sono distinguibili.

## Telegram

1. Aprire Telegram e creare un bot con `@BotFather`.
2. Copiare il token ricevuto.
3. Scrivere almeno un messaggio al nuovo bot.
4. Ricavare il proprio `chat_id` tramite l'API `getUpdates`.
5. Inserire in `secrets.yaml`:

   ```yaml
   telegram_send_url: "https://api.telegram.org/botTOKEN/sendMessage"
   telegram_chat_id: "CHAT_ID"
   ```

6. Compilare e installare il firmware.
7. Premere `Test Telegram diretto`.
8. Controllare `Ultimo invio Telegram`: HTTP 200 indica successo; in caso di
   errore viene mostrata anche la descrizione restituita da Telegram.
9. Verificare che `Notifiche Telegram abilitate` sia acceso.

Il token è una credenziale: non va inserito nel file pubblico, nei log o nel
repository GitHub.

Telegram viene usato per allarmi, ripristini ed eventi tecnici anche quando
Home Assistant è collegato. L'interruttore dedicato disabilita tutti gli invii
automatici; il pulsante di test resta sempre disponibile. La perdita Ethernet
viene notificata appena la rete torna disponibile; la perdita di Home Assistant
viene confermata dopo 30 secondi per evitare falsi allarmi.

## Email diretta con Brevo

1. Creare un account Brevo.
2. In `Impostazioni > Mittenti e IP > Mittenti`, registrare e verificare
   l'indirizzo che invierà gli allarmi.
3. In `SMTP & API > Chiavi API`, creare una nuova chiave API.
4. Copiare `secrets.example.yaml` come `secrets.yaml` e compilare:

   ```yaml
   brevo_api_key: "xkeysib-..."
   brevo_sender_email: "mittente-verificato@example.com"
   brevo_sender_name: "Alarm Contact Collector"
   brevo_recipient_email: "destinatario@example.com"
   brevo_recipient_name: "Roberto"
   ```

5. Compilare e installare il firmware.
6. Premere `Test email Brevo`.
7. Controllare `Ultimo invio email Brevo`: HTTP 201 indica che Brevo ha
   accettato il messaggio.
8. Verificare che `Notifiche email Brevo abilitate` sia acceso.

La chiave API resta in `secrets.yaml` e non deve essere pubblicata. L'indirizzo
del mittente deve corrispondere a un mittente verificato in Brevo.

Telegram e Brevo sono indipendenti: puoi abilitarne uno solo o entrambi. Gli
invii automatici non dipendono dalla connessione Home Assistant; i pulsanti di
test funzionano sempre.

## Storico eventi locale

Lo storico rimane nella scheda anche dopo un riavvio o una mancanza di
alimentazione. Sostituire `<IP_SCHEDA>` con l'indirizzo Ethernet visualizzato
in Home Assistant o assegnato dal router:

- `http://<IP_SCHEDA>/history` mostra lo storico;
- `http://<IP_SCHEDA>/history.csv` scarica tutti gli eventi in CSV;
- `http://<IP_SCHEDA>/history.json` restituisce gli eventi in JSON.

La pagina mostra prima gli eventi più recenti. Quando vengono raggiunte 500
righe, il nuovo evento sostituisce automaticamente quello più vecchio. Sono
registrati avvii e relativa causa, allarmi, ripristini, esclusioni, modifiche
NO/NC, collegamenti di rete e Home Assistant, comandi e risultati degli invii
Telegram/Brevo. Le interruzioni Ethernet inferiori a 10 secondi non vengono
salvate, per evitare eventi inutili e scritture eccessive.

Il pulsante `Cancella storico` richiede una conferma dal browser. La partizione
dedicata viene preservata dai normali aggiornamenti OTA, ma viene cancellata da
una cancellazione completa della flash o da modifiche incompatibili alla tabella
delle partizioni.

## LED

| Colore | Significato |
| --- | --- |
| Verde | Regolare e Home Assistant collegato |
| Blu | Home Assistant non collegato |
| Rosso lampeggiante | Allarme attivo |
| Giallo | Allarme ripristinato ma memoria presente |
| Viola | Almeno una centrale esclusa |

## Installazione

Usare ESPHome 2026.7.0 o successivo:

```bash
esphome run alarm-collector-advanced.yaml
```

Il firmware non è una centrale di sicurezza certificata e non sostituisce le
funzioni, certificazioni o comunicazioni proprie delle centrali collegate.
