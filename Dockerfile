FROM debian:bookworm-slim

RUN apt-get update && apt-get install -y --no-install-recommends g++ make cmake && rm -rf /var/lib/apt/lists/*


WORKDIR /TP2-BD1-AlMdBar
COPY . /TP2-BD1-AlMdBar/


RUN make build

VOLUME ["/data"]

ENV CSV_PATH=/data/input.csv \
    DATA_DIR=/data/db \
    LOG_LEVEL=info


CMD ["bash", "-lc", "echo 'Use: docker run ... upload|findrec|seek1|seek2'; ls -l bin/"]


