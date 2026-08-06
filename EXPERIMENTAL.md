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
  `/help`) da chat privata o gruppo autorizzato, offset anti-duplicazione e audit
  persistente con ID chat, ID utente e username;
- comandi amministrativi `/tacita` e `/reset_memorie`, limitati agli ID presenti
  nella lista admin e protetti da codice monouso valido 60 secondi e legato al
  singolo utente e alla chat;
- invio autonomo degli eventi a due istanze Home Assistant tramite webhook HTTPS,
  con destinazioni disabilitate di default, token payload separati e diagnostica;
- attesa della reale disponibilità Internet dopo un blackout, con conservazione
  del report di riavvio in RAM e ritentativo automatico ogni 30 secondi;
- heartbeat ogni 5 minuti verso i Home Assistant remoti con stato completo,
  temperatura, uptime, rete, NTP e versione firmware;
- code di consegna indipendenti per Telegram, Brevo e i due Home Assistant,
  con conferma HTTP, classificazione degli errori e backoff progressivo;
- comunicazione autonoma verso più istanze Home Assistant;
- gestione di endpoint, autenticazione, ritentativi e coda offline;
- comandi Telegram con elenco mittenti autorizzati;
- comandi iniziali solo in lettura: stato, allarmi, rete, uptime e storico;
- audit persistente di ogni comando ricevuto e relativo risultato;
- protezione da duplicazioni, replay e richieste troppo frequenti;
- mantenimento del funzionamento locale in assenza di servizi esterni.

## Notifiche Home Assistant antincendio

- DI1 notificato come avvio/arresto della pompa dell'anello antincendio;
- DI2 notificato come allarme/ripristino generico della centralina pompa;
- promemoria ripetuti finché i contatti rimangono attivi;
- avviso di perdita e ripristino del concentratore;
- notifica persistente sempre disponibile e servizio push configurabile;
- interruttore generale per disabilitare gli avvisi Home Assistant.

## Regole di sicurezza

- nessun token, URL privato o chiave nel repository;
- nessun comando ai relè finché non viene progettata una conferma esplicita;
- Telegram accetta comandi solo dalla chat privata o dal gruppo autorizzati e
  limita i comandi operativi agli ID utente presenti nella lista amministratori;
- tacitazione e cancellazione memorie richiedono un codice monouso di conferma;
- le comunicazioni verso Home Assistant usano HTTPS e secret separati;
- ogni funzione sperimentale deve poter essere disabilitata localmente;
- ogni modifica deve superare GitHub Actions prima del test sulla scheda.

## Candidati successivi

- invio eventi a due o più Home Assistant tramite webhook;
- coda persistente degli eventi non consegnati;
- heartbeat e diagnostica remota;
- configurazione di più destinazioni senza ricompilare;
- esportazione e backup automatico dello storico;
- comandi Telegram amministrativi aggiuntivi, sempre con conferma a due passaggi;
- aggiornamenti firmware controllati e rollback.
