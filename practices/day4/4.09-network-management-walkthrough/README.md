# 4.09 – Network Management und Connection Manager

Das vollständige Demo registriert einen `net_mgmt`-Callback für
`NET_EVENT_L4_CONNECTED` und `NET_EVENT_L4_DISCONNECTED`. Der Callback übernimmt nur den
neuen Zustand und reicht die Verarbeitung an ein Work Item weiter. Mit
`conn_mgr_mon_resend_status()` wird zusätzlich der aktuelle Monitorstatus angefordert.

```sh
west build -p always -b native_sim \
  training-zephyr/practices/day4/4.09-network-management-walkthrough/demo \
  -d build/day4-4.09
./build/day4-4.09/zephyr/zephyr.exe
```

Die zwei explizit ausgelösten Events sind deterministische Lehrstimuli. Sie ersetzen nicht
die Bewertung einer realen Netzwerkschnittstelle, zeigen aber denselben Eventpfad. Für den
Walkthrough gibt es bewusst kein `start`/`solution`-Paar.
