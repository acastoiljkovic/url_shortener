#!/bin/bash

podman compose down
podman image rm url_shortener -f
podman image rm url_shortener:latest -f
podman image rm url_shortener_url_shortener:latest -f
podman build -t url_shortener .
