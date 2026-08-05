# Experimental branch

Questo branch è l'ambiente di laboratorio del progetto. Il firmware stabile
rimane nel branch `main`; le funzioni vengono riportate su `main` solo dopo
compilazione e test reali sulla Waveshare ESP32-S3-POE-ETH-8DI-8RO.

## Prima fase

- report Telegram/email al primo avvio, ai riavvii e al ripristino Ethernet;
- IP locale e pubblico nel report di rete;
- durata offline esatta durante il funzionamento e stimata entro circa 5 minuti
  dopo un'interruzione improvvisa dell'alimentazione;
- sincronizzazione autonoma SNTP ogni 6 ore con aggiornamento del RTC hardware,
  senza dipendere dall'orario fornito da Home Assistant;
- comandi Telegram in sola lettura (`/stato`, `/allarmi`, `/rete`, `/storico`,
  `/help`) con `chat_id` autorizzato, offset anti-duplicazione e audit persistente;
- invio autonomo degli eventi a due istanze Home Assistant tramite webhook HTTPS,
  con destinazioni disabilitate di default e diagnostica separata;
- package Home Assistant riutilizzabile per istanze remote, con entità di stato,
  contatore eventi, logbook e rilancio su event bus;
- dashboard Lovelace dedicata, realizzata esclusivamente con card native;
- comunicazione autonoma verso più istanze Home Assistant;
- gestione di endpoint, autenticazione, ritentativi e coda offline;
- comandi Telegram con elenco mittenti autorizzati;
- comandi iniziali solo in lettura: stato, allarmi, rete, uptime e storico;
- audit persistente di ogni comando ricevuto e relativo risultato;
- protezione da duplicazioni, replay e richieste troppo frequenti;
- mantenimento del funzionamento locale in assenza di servizi esterni.

## Regole di sicurezza

- nessun token, URL privato o chiave nel repository;
- nessun comando ai relè finché non viene progettata una conferma esplicita;
- Telegram accetta comandi solo da `chat_id` autorizzati;
- le comunicazioni verso Home Assistant usano HTTPS e secret separati;
- ogni funzione sperimentale deve poter essere disabilitata localmente;
- ogni modifica deve superare GitHub Actions prima del test sulla scheda.

## Candidati successivi

- invio eventi a due o più Home Assistant tramite webhook;
- coda persistente degli eventi non consegnati;
- heartbeat e diagnostica remota;
- configurazione di più destinazioni senza ricompilare;
- esportazione e backup automatico dello storico;
- comandi Telegram amministrativi con conferma a due passaggi;
- aggiornamenti firmware controllati e rollback.
