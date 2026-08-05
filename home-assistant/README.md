# Home Assistant remoto

Il file `packages/alarm_contact_collector.yaml` va copiato senza modifiche in
`/config/packages/` su ciascuna istanza Home Assistant.

Nel `configuration.yaml` deve essere presente:

```yaml
homeassistant:
  packages: !include_dir_named packages
```

Nel `secrets.yaml` di ogni istanza aggiungere un ID e un token differenti:

```yaml
alarm_collector_webhook_id: "GENERA_UN_ID_CASUALE_DI_ALMENO_64_CARATTERI"
alarm_collector_payload_token: "GENERA_UN_SECONDO_TOKEN_LUNGO_E_CASUALE"
```

La URL da configurare nel firmware è:

```text
https://NOME-ISTANZA/api/webhook/VALORE_DEL_WEBHOOK_ID
```

Dopo aver copiato il package, eseguire il controllo configurazione e riavviare
Home Assistant. Il webhook è accessibile da Internet: ID e token payload devono
essere trattati come password. Il token deve coincidere con quello configurato
per la stessa destinazione nel firmware.

## Dashboard Lovelace

### Installazione dalla sola interfaccia grafica

Non è necessario modificare `configuration.yaml` o riavviare Home Assistant:

1. aprire **Impostazioni → Dashboard**;
2. scegliere **Aggiungi dashboard → Nuova dashboard da zero**;
3. impostare titolo `Alarm Collector` e icona `mdi:shield-home-outline`;
4. aprire la nuova dashboard;
5. menu con i tre puntini → **Modifica dashboard**;
6. nuovamente tre puntini → **Editor configurazione raw**;
7. cancellare il contenuto proposto e incollare tutto il contenuto di
   `dashboards/alarm-contact-collector.yaml`;
8. premere **Salva**.

Questo metodo salva la dashboard nella configurazione interna di Home Assistant
e può essere ripetuto nello stesso modo su CED HA e Ufficio HA.

File pronto da copiare:

```text
https://raw.githubusercontent.com/robertovendrame/esphome-waveshare-esp32-s3-poe-eth-8di-8ro/main/home-assistant/dashboards/alarm-contact-collector.yaml
```

### Installazione YAML alternativa

Copiare `dashboards/alarm-contact-collector.yaml` in:

```text
/config/dashboards/alarm-contact-collector.yaml
```

Nel `configuration.yaml` aggiungere, oppure integrare nell'eventuale sezione
`lovelace:` già presente:

```yaml
lovelace:
  dashboards:
    alarm-collector:
      mode: yaml
      title: Alarm Collector
      icon: mdi:shield-home-outline
      show_in_sidebar: true
      require_admin: false
      filename: dashboards/alarm-contact-collector.yaml
```

Eseguire nuovamente il controllo configurazione e riavviare Home Assistant.
La stessa dashboard può essere usata senza modifiche sia su CED HA sia su
Ufficio HA.

Lo storico persistente della scheda è disponibile agli indirizzi `/history`,
`/history.csv` e `/history.json`. Il percorso `/events` è riservato allo stream
live del web server ESPHome e non rappresenta lo storico salvato in memoria.
