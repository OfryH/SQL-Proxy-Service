FROM ubuntu:22.04

RUN apt update && apt install -y \
    cmake \
    g++ \
    make \
    default-libmysqlclient-dev

WORKDIR /app

COPY . .

RUN cmake -B build \
    && cmake --build build

CMD ["./build/sql_proxy"]