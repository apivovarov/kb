
# Install DuckDNS service on Linux host

## DuckDNS

Login to [DuckDNS](https://www.duckdns.org/) and create new domain.

### Script to update IP

File: `/usr/local/bin/duckdns.sh`

Content:
```bash
#!/bin/bash
# Update DuckDNS with a curl command
echo url="https://www.duckdns.org/update?domains=<domain>&token=<token>&ip=" | /usr/bin/curl -k -o /var/log/duckdns.log -K -
```

Make it executable
```bash
chmod +x /usr/local/bin/duckdns.sh
```


### SystemD on-start service

File: `/etc/systemd/system/duckdns.service`

Content:
```
[Unit]
Description=Update DuckDNS IP on Startup
After=network.target

[Service]
ExecStart=/usr/local/bin/duckdns.sh
Type=oneshot
RemainAfterExit=yes

[Install]
WantedBy=multi-user.target
```

### Enable and Start the service

```bash
sudo systemctl enable duckdns.service

sudo systemctl start duckdns.service

sudo systemctl status duckdns.service
```

### Check log

```bash
cat /var/log/duckdns.log
```
