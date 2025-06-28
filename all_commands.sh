#!/bin/bash

bash build_app.sh
bash build_container.sh
podman compose up -d
bash test_app.sh
