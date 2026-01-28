FROM ubuntu:22.04

WORKDIR /workspace

COPY . /workspace

RUN apt-get update && \
    apt-get install -y --no-install-recommends python3

ENTRYPOINT ["python3", "scripts/create_firmware.py"]
