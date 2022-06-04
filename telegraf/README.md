## Telegraf config
- Set the correct InfluxDB database name in /etc/telegraf/telegraf.conf.
- Set the correct InfluxDB database name in output.conf
- Set the correct path to the Python script in source_python.conf
- Run telegraf:
```
telegraf --config-directory . --config /etc/telegraf/telegraf.conf --debug
```
