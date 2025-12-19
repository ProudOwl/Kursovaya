#!/bin/bash

if [ -z "$DISPLAY" ]; then
    if [ -f /etc/resolv.conf ] && grep -q "nameserver" /etc/resolv.conf; then
        export DISPLAY=$(grep nameserver /etc/resolv.conf | awk '{print $2}'):0
    else
        export DISPLAY=:0
    fi
fi

echo "========================================"
echo "  AstroLearn - Запуск игры"
echo "  DISPLAY: $DISPLAY"
echo "========================================"

mkdir -p /app/saves
chmod 777 /app/saves

sudo service postgresql start

sudo -u postgres psql -c "ALTER USER postgres WITH PASSWORD 'postgres';"

for i in {1..10}; do
    if sudo -u postgres psql -c "SELECT 1;" > /dev/null 2>&1; then
        break
    fi
    sleep 2
done

if ! sudo -u postgres psql -lqt | grep -qw astrolearn; then
    sudo -u postgres psql -c "CREATE DATABASE astrolearn;"
    sudo -u postgres psql -d astrolearn -f /app/init_database.sql
fi

sudo service postgresql restart
sleep 2

cd /app/build
exec ./astrolearn
