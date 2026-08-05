# Home Assistant remoto

Il file `packages/alarm_contact_collector.yaml` va copiato senza modifiche in
`/config/packages/` su ciascuna istanza Home Assistant.

Nel `configuration.yaml` deve essere presente:

```yaml
homeassistant:
  packages: !include_dir_named packages
```

Nel `secrets.yaml` di ogni istanza aggiungere un ID diverso:

```yaml
alarm_collector_webhook_id: "GENERA_UN_ID_CASUALE_DI_ALMENO_64_CARATTERI"
```

La URL da configurare nel firmware è:

```text
https://NOME-ISTANZA/api/webhook/VALORE_DEL_WEBHOOK_ID
```

Dopo aver copiato il package, eseguire il controllo configurazione e riavviare
Home Assistant. Il webhook è accessibile da Internet e il suo ID deve essere
trattato come una password.

## Dashboard Lovelace

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
