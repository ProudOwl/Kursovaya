FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем всё необходимое
RUN apt-get update && apt-get install -y \
    build-essential cmake libsfml-dev libpqxx-dev \
    postgresql postgresql-client postgresql-contrib \
    x11-apps xauth sudo wget \
    && rm -rf /var/lib/apt/lists/*

# Настраиваем PostgreSQL
RUN echo "host all  all    0.0.0.0/0  md5" >> /etc/postgresql/14/main/pg_hba.conf && \
    echo "listen_addresses='*'" >> /etc/postgresql/14/main/postgresql.conf

# Создаём пользователя с UID 1000 (стандартный для пользователей Linux)
RUN useradd -m -u 1000 -s /bin/bash astrouser && \
    echo "astrouser ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Копируем исходный код
WORKDIR /app
COPY src/ /app/src/
COPY CMakeLists.txt /app/
COPY init_database.sql /app/

# Компилируем игру
RUN mkdir -p /app/build && \
    cd /app/build && \
    cmake .. && \
    make -j$(nproc) && \
    chmod +x /app/build/astrolearn

# Копируем скрипт запуска
COPY entrypoint.sh /app/entrypoint.sh
RUN chmod +x /app/entrypoint.sh

# Устанавливаем переменные окружения по умолчанию
ENV DISPLAY=:0
ENV XAUTHORITY=/home/astrouser/.Xauthority

# Создаём директорию для X11 авторизации
RUN mkdir -p /home/astrouser && \
    chown -R astrouser:astrouser /home/astrouser

USER astrouser

# Точка входа
ENTRYPOINT ["/app/entrypoint.sh"]
