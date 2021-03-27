![Grafana](https://media.atlantishq.de/pictures/grafana_github_board.png)

# Setup/Dependencies

- [Generic ESP 8266](https://github.com/esp8266/Arduino#installing-with-boards-manager)
- SimpleDHT >= 1.0.14
- Adafruid GFX >= 1.10.6
- Adafruit SSD1306 >= 2.4.3

Rename *settings.h.example* to *settings.h* and adjust settings accordingly.

# Influx-DB Setup

    apt install influxdb
    systemctl enable influxd
    systemctl start  influxd
    
    influx
    > create user esp with password SECRET;
    > create database sensors;
    > grant all on sensors to esp;
    > exit

# Nginx as reverse proxy
Nginx can be used for *TLS* and *Basic Auth*:

    server {
    
        listen 8443 ssl;
    
        auth_basic "Influx";
        auth_basic_user_file "/path/to/authfile";
    
        location / {
            proxy_set_header X-REAL-HOSTNAME $host;
            proxy_pass http://localhost:8086;
        }
    }

Here is a [tutorial](https://medium.com/anti-clickbait-coalition/hassle-free-ssl-with-nginx-f34ddcacf197) about SSL/TLS with Let's Encrypt in nginx. To create the passfile do:

    echo -n "USER:" > /path/to/authfile
    mkpasswd -m sha-512 PASSWORD >> /path/to/authfile

# Visualization

- [Grafana](https://grafana.com/)

